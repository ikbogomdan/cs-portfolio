#include <iostream>
#include <type_traits>
#include <tuple>
#include <vector>
#include <string>
#include <list>

template <typename... Ts>
concept for_non_expl = (std::is_default_constructible_v<Ts> && ...);

template<typename T>
auto copy_init_list_helper(int) -> decltype(T{}, std::true_type{}) {
    return std::true_type{};
}

template<typename T>
auto copy_init_list_helper(double) -> std::false_type {
    return std::false_type{};
}

template<typename T>
const bool copy_init_list_helper_v = decltype(copy_init_list_helper<T>(0))::value;

template <typename... Ts>
concept copy_init_list = (copy_init_list_helper_v<Ts> && ...);

template <typename... Ts>
concept dir_arg = (std::is_copy_constructible_v<Ts> && ...); // && (std::is_convertible_v<const Ts&, Ts> && ...);

template <typename... Ts>
concept dir_arg_expl = dir_arg<Ts...> && !(std::is_convertible_v<const Ts&, Ts> && ...);

template<typename...> struct Pack {};

template<typename... T, typename... U>
constexpr bool copy_const_other(Pack<T...>, Pack<U...>) {
    if constexpr(sizeof...(T) != sizeof...(U)) {
        return false;
    }
    if constexpr ((std::is_same_v<T, U> && ...)) {
        return (std::is_copy_constructible_v<T> && ...);
    } else {
        return false;
    }

}


template<typename... Ts>
concept copy_move = (std::is_move_constructible_v<Ts> && ...);

template<typename... Ts>
concept assign = (std::is_copy_assignable_v<Ts> && ...);

template<typename... Ts>
concept move_assign = (std::is_move_assignable_v<Ts> && ...);




template<typename... T, typename... U>
constexpr bool is_constructible_i(Pack<T...>, Pack<U...>) {
    if constexpr (sizeof...(T) != sizeof...(U)) {
        return false;
    } else {
        return (std::is_constructible_v<T, U> && ...);
    }
}

template<typename... T, typename... U>
constexpr bool is_constructible_i_expl(Pack<T...>, Pack<U...>) {
    if constexpr (sizeof...(T) != sizeof...(U)) {
        return false;
    } else {
        return (std::is_convertible_v<U, T> && ...);
    }
}

template<typename... T, typename... U>
constexpr bool is_assign_is(Pack<T...>, Pack<U...>) {
    if constexpr (sizeof...(T) != sizeof...(U)) {
        return false;
    } else {
        return (std::is_assignable_v<T&, const U&> && ...);
    }
}

template<typename... T, typename... U>
constexpr bool is_assign_is_move(Pack<T...>, Pack<U...>) {
    if constexpr (sizeof...(T) != sizeof...(U)) {
        return false;
    } else {
        return (std::is_assignable_v<T&, U> && ...);
    }
}

template<typename ...> class Tuple{};

template<typename... T, typename... U>
constexpr bool is_constructible_(Pack<T...> , Pack<U...>) {
    return (std::is_constructible_v<T, U> && ...);
}

template<typename... T, typename... U>
constexpr bool is_constructible_move_(Pack<T...> , Pack<U...>) {
    return (std::is_constructible_v<T, U&&> && ...);
}

template<typename... T, typename... U>
constexpr bool is_convertible_(Pack<T...> , Pack<U...>) {
    return (std::is_convertible_v<T, U> && ...);
}



template <typename T, typename... Ts>
class Tuple<T, Ts...> {

    template<typename... Utypes, typename... Otypes>
    void assign_helper(Tuple<Utypes...>& us_, const Tuple<Otypes...>& other_) {
        us_.value_ = other_.value_;
        if constexpr(sizeof...(Utypes) + sizeof...(Otypes) > 2) {
            assign_helper(us_.tail_value_, other_.tail_value_);
        }
    }

    template<typename... Utypes, typename... Otypes>
    void assign_helper_move(Tuple<Utypes...>& us_, Tuple<Otypes...>&& other_) {
        us_.value_ = get<0>(std::forward<decltype(other_)>(other_));
        if constexpr(sizeof...(Utypes) > 1) {
            assign_helper_move(us_.tail_value_, std::move(other_.tail_value_));
        }
    }


    template<size_t N, typename... Giorgio> friend decltype(auto) get(Tuple<Giorgio...>&);
    template<size_t N, typename... Giorgio> friend decltype(auto) get(const Tuple<Giorgio...>&);
    template<size_t N, typename... Giorgio> friend decltype(auto) get(Tuple<Giorgio...>&&);
    template<size_t N, typename... Giorgio> friend decltype(auto) get(const Tuple<Giorgio...>&&);
    template<typename Giovanni, typename... Giorgio> friend decltype(auto) get(Tuple<Giorgio...>&);
    template<typename Giovanni, typename... Giorgio> friend decltype(auto) get(const Tuple<Giorgio...>&);
    template<typename Giovanni, typename... Giorgio> friend decltype(auto) get(Tuple<Giorgio...>&&);
    template<typename Giovanni, typename... Giorgio> friend decltype(auto) get(const Tuple<Giorgio...>&&);
private:

     template<typename...> friend class Tuple;


    T value_;
    Tuple<Ts...> tail_value_;

public:

    Tuple& operator=(const Tuple& other_)
    requires assign<T, Ts...>
    {
        value_ = other_.value_;
        if constexpr(sizeof...(Ts)) {
            tail_value_ = other_.tail_value_;
        }
        return *this;
    }




    template<typename... U>
    requires (is_assign_is_move(Pack<T, Ts...>{}, Pack<U...>{}))
    Tuple& operator=(Tuple<U...>&& other_)
    {
        value_ = get<0>(std::forward<decltype(other_)>(other_));
        if constexpr(sizeof...(Ts)) {
            assign_helper_move(tail_value_, std::move(other_.tail_value_));
        }
        return *this;
    }

    template<typename Tt, typename Uu>
    Tuple& operator=(const std::pair<Tt, Uu>& other_)
    //requires (sizeof...(Ts) == 1)
    {
        value_ = std::get<0>(other_);
        tail_value_.value_ = std::get<1>(other_);
        return *this;
    }

    template<typename Tt, typename Uu>
    Tuple& operator=(std::pair<Tt, Uu>&& other_)
    //requires (sizeof...(Ts) == 1)
    {
        value_ = std::get<0>(std::move(other_));
        tail_value_.value_ = std::get<1>(std::move(other_));
        return *this;
    }

    template<typename... U>
    requires (is_assign_is(Pack<T, Ts...>{}, Pack<U...>{}))
    Tuple& operator=(const Tuple<U...>& other_)
    {
        value_ = other_.value_;
        if constexpr(sizeof...(Ts)) {
            assign_helper(tail_value_, other_.tail_value_);
        }
        return *this;
    }

    explicit (!((std::is_convertible_v<const Ts&, Ts> && ...) && std::is_convertible_v<const T&, T>))
    Tuple(const T& arg, const Ts&... args)
    requires (std::is_constructible_v<T, const T&> && (std::is_constructible_v<Ts, const Ts&> && ...))
    :value_(arg), tail_value_(args...) {}

    template<typename Type, typename... Types>
    explicit( (sizeof...(Types) == sizeof...(Ts)) && !(is_constructible_i_expl(Pack<T, Ts...>{}, Pack<Type,Types...>{})) )
    Tuple(Type&& arg, Types&&... args)
    requires (is_constructible_i(Pack<T, Ts...>{}, Pack<Type,Types...>{}))
    :value_(std::forward<Type>(arg)), tail_value_(std::forward<Types>(args)...) {}





    template<typename Type, typename... Types>
    explicit (!(std::is_convertible_v<Type, T> && (std::is_convertible_v<Types, Ts> && ...)))
    constexpr Tuple(const Tuple<Type, Types...>& other_)
    requires ((copy_const_other(Pack<T, Ts...>{}, Pack<Type, Types...>{})) || ((sizeof...(Types) == sizeof...(Ts)) &&
        (is_constructible_(Pack<T, Ts...>{}, Pack<const Type, const Types...>{})) && //is_constructible_(Pack<T, Ts...>{}, other_)
        ((sizeof...(Types) != 0) || (!std::is_convertible_v<decltype(other_), T> &&
         !std::is_constructible_v<T, decltype(other_)> && !std::is_same_v<T, Type>))) )
    : value_(get<0>(std::forward<decltype(other_)>(other_))), tail_value_(other_.tail_value_) {}

    Tuple( const Tuple& other ) = default;


    template<typename Type, typename... Types>
    explicit (!(std::is_convertible_v<Type&&, T> && (std::is_convertible_v<Types&&, Ts> && ...)))
    constexpr Tuple(Tuple<Type, Types...>&& other_)
    requires ( (sizeof...(Types) == sizeof...(Ts)) &&
        (is_constructible_move_(Pack<T, Ts...>{}, Pack<Type&&, Types&&...>{})) && //is_constructible_(Pack<T, Ts...>{}, other_)
        ((sizeof...(Types) != 0) || (!std::is_convertible_v<decltype(other_), T> &&
         !std::is_constructible_v<T, decltype(other_)> && !std::is_same_v<T, Type>)) )
    : value_(get<0>(std::forward<decltype(other_)>(other_))),
    tail_value_(std::move(other_.tail_value_)) {}

    explicit(!copy_init_list<Ts...>) Tuple()
    requires for_non_expl<T, Ts...> :value_(), tail_value_() {}

    template<typename U1, typename U2>
    Tuple(std::pair<U1, U2>&& p): value_(std::get<0>(std::move(p))), tail_value_(std::get<1>(std::move(p))) {};

    template<typename U1, typename U2 >
    Tuple(const std::pair<U1, U2>& p): value_(std::get<0>(p)), tail_value_(std::get<1>(p)) {}

    template<typename U1, typename U2 >
    Tuple(std::pair<U1, U2>& p): value_(std::get<0>(p)), tail_value_(std::get<1>(p)) {}



    Tuple(Tuple&& other_) requires copy_move<T, Ts...>
    : value_(std::move(other_.value_)), tail_value_(std::move(other_.tail_value_))  {}

};

template<> class Tuple<> {};

template<typename U1, typename U2>
Tuple(std::pair<U1, U2>&& p) -> Tuple<U1, U2>;

template<typename U1, typename U2>
Tuple(const std::pair<U1, U2>& p ) -> Tuple<U1, U2>;

//copypastecopypastecopypastecopypastecopypastecopypastecopypastecopypastecopypastecopypastecopypastecopypastecopypastecopypastecopypastecopypaste

template<size_t N, typename... Types>
decltype(auto) get(Tuple<Types...>& tuple_) {
    if constexpr (N == 0) {
        return (tuple_.value_);
    } else {
        return (get<N - 1>(tuple_.tail_value_));
    }
}

template<size_t N, typename... Types>
decltype(auto) get(const Tuple<Types...>& tuple_) {
    if constexpr (N == 0) {
        return (tuple_.value_);
    } else {
        return (get<N - 1>(tuple_.tail_value_));
    }
}

template<typename Type, typename... Types>
decltype(auto) get(Tuple<Types...>& tuple_) {
    if constexpr (std::is_same_v<Type, decltype(tuple_.value_)>) {
        return (tuple_.value_);
    } else {
        return (get<Type>(tuple_.tail_value_));
    }
}

template<typename Type, typename... Types>
decltype(auto) get(const Tuple<Types...>& tuple_) {
    if constexpr (std::is_same_v<Type, decltype(tuple_.value_)>) {
        return (tuple_.value_);
    } else {
        return (get<Type>(tuple_.tail_value_));
    }
}
template<size_t N, typename... Types>
decltype(auto) get(Tuple<Types...>&& tuple_) {
    if constexpr (N == 0) {
        if constexpr (std::is_lvalue_reference_v<decltype(tuple_.value_)>) {
            return (tuple_.value_);
        } else {
            return std::move(tuple_.value_);
        }
    } else {
        return std::move(get<N - 1>(tuple_.tail_value_));
    }
}
template<size_t N, typename... Types>
decltype(auto) get(const Tuple<Types...>&& tuple_) {
    if constexpr (N == 0) {
        if constexpr (std::is_lvalue_reference_v<decltype(tuple_.value_)>) {
            return (tuple_.value_);
        } else {
            return std::move(tuple_.value_);
        }
    } else {
        return std::move(get<N - 1>(tuple_.tail_value_));
    }
}
template<typename Type, typename... Types>
decltype(auto) get(Tuple<Types...>&& tuple_) {
    if constexpr (std::is_same_v<Type, decltype(tuple_.value_)>) {
        if constexpr (std::is_lvalue_reference_v<decltype(tuple_.value_)>) {
            return (tuple_.value_);
        } else {
            return std::move(tuple_.value_);
        }
    } else {
        return get<Type>(std::move(tuple_.tail_value_));
    }
}
template<typename Type, typename... Types>
decltype(auto) get(const Tuple<Types...>&& tuple_) {
    if constexpr (std::is_same_v<Type, decltype(tuple_.value_)>) {
        if constexpr (std::is_lvalue_reference_v<decltype(tuple_.value_)>) {
            return (tuple_.value_);
        } else {
            return std::move(tuple_.value_);
        }
    } else {
        return get<Type>(std::move(tuple_.tail_value_));
    }
}

template<typename... Types>
auto makeTuple(Types&&... args) -> Tuple<std::decay_t<Types>...> {
    return {std::forward<Types>(args)...};
}

template<typename... Types>
auto tie(Types&... args) -> Tuple<Types&...> {
    return {args...};
}

template<typename... Types>
auto forwardAsTuple(Types&&... args) -> Tuple<Types&&...> {
    return {std::forward<Types>(args)...};
}


