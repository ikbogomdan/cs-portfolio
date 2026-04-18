#include <array>
#include <iostream>
#include <iterator>
#include <memory>


const std::size_t DYNAMIC_CAPACITY = std::numeric_limits<std::size_t>::max();

template<bool>
    struct IsType {};

template<>
struct IsType<true> {
    size_t capacity_ = 0;
    IsType(size_t value_):capacity_(value_) {};
    IsType(){};
    IsType& operator=(size_t value_) { capacity_ = value_; return *this; };
};

template<>
struct IsType<false> {};

template <typename T, std::size_t Capacity = DYNAMIC_CAPACITY>
class CircularBuffer: IsType<Capacity == DYNAMIC_CAPACITY> {

    std::conditional_t<DYNAMIC_CAPACITY == Capacity, T*, std::array<std::byte[sizeof(T)], Capacity + 2>> arr_;
    size_t size_ = 0, first_ = 0;

public:

    explicit CircularBuffer(std::size_t capacity_in_) {
        if constexpr (Capacity == DYNAMIC_CAPACITY) {
            IsType<Capacity == DYNAMIC_CAPACITY>::capacity_ = capacity_in_;
            arr_ = reinterpret_cast<T*>(new std::byte[sizeof(T) * (capacity() + 2)]);
        } else {
            if (capacity_in_ != Capacity) {
                throw std::invalid_argument("CE");
            }
        }
    }

    CircularBuffer() {
        static_assert(Capacity != DYNAMIC_CAPACITY, "CE");
    }


    template <size_t Capacity_right>
    CircularBuffer(const CircularBuffer<T, Capacity_right>& other_) {
        static_assert(!((Capacity != DYNAMIC_CAPACITY) && (Capacity_right != Capacity)), "CE");
        if constexpr (Capacity == DYNAMIC_CAPACITY) {
            IsType<Capacity == DYNAMIC_CAPACITY>::capacity_ = other_.capacity_;
            arr_ = reinterpret_cast<T*>(new std::byte[sizeof(T) * (capacity() + 2)]);
        }
        first_ = 0;
        size_t i = 0;
        size_ = other_.size();
        try {
            for (; i < other_.size_; ++i) {
                new (get_address() + i) T(other_[i]);
            }
        } catch(...) {
            first_ = size_ = 0;
            for (size_t j = 0; j < i; ++j) {
                (get_address() + i)->~T();
            }
            if constexpr (Capacity == DYNAMIC_CAPACITY) {
                IsType<Capacity == DYNAMIC_CAPACITY>::capacity_ = 0;
            }
            throw;
        }
    }

    CircularBuffer(const CircularBuffer& other_) {
        if constexpr (Capacity == DYNAMIC_CAPACITY) {
            IsType<Capacity == DYNAMIC_CAPACITY>::capacity_ = other_.capacity_;
            arr_ = reinterpret_cast<T*>(new std::byte[sizeof(T) * (capacity() + 2)]);
        }
        first_ = 0;
        size_t i = 0;
        size_ = other_.size();
        try {
            for (; i < other_.size_; ++i) {
                new (get_address() + i) T(other_[i]);
            }
        } catch(...) {
            first_ = size_ = 0;
            for (size_t j = 0; j != i; ++j) {
                (get_address() + j)->~T();
            }
            if constexpr (Capacity == DYNAMIC_CAPACITY) {
                IsType<Capacity == DYNAMIC_CAPACITY>::capacity_ = 0;
            }
            throw;
        }
    }

    CircularBuffer& operator=(const CircularBuffer& other_) {
        if (this == &other_) {
            return *this;
        }
        while (!empty()) {
            pop_front();
        }
        first_ = 0;
        size_t i = 0;
        size_ = other_.size();
        try {
            for (; i < other_.size_; ++i) {
                new (get_address() + i) T(other_[i]);
            }
        } catch(...) {
            first_ = size_ = 0;
            for (size_t j = 0; j < i; ++j) {
                (get_address() + j)->~T();
            }
            if constexpr (Capacity == DYNAMIC_CAPACITY) {
                IsType<Capacity == DYNAMIC_CAPACITY>::capacity_ = 0;
            }
            throw;
        }
        return *this;
    }
    void clear() {
        while (!empty()) {
            pop_front();
        }
    }
    ~CircularBuffer() {
        clear();

        if constexpr (Capacity == DYNAMIC_CAPACITY) {
            delete[] reinterpret_cast<std::byte*>(arr_);
        }
    }

    void push_back(const T& value) {
        if (size_ == 0) {
            new (get_address()) T(value);
            ++size_;
            return;
        }
        if (size_ == capacity()) {
            (get_address() + first_)->~T();
            first_ = (first_ + 1) % (capacity() + 1);
        }
        size_t last_add_ = (std::min(size_ + 1, capacity()) + first_ - 1) % (capacity() + 1);
        try {
            new (get_address() + last_add_) T(value);
        } catch (...) {
            if (size_ == capacity()) {
                first_ = first_ == 0 ? capacity() : first_ - 1;
            }
            throw;
        }
        size_ = std::min(size_ + 1, capacity());
    }

    void push_front(const T& value) {
        if (size_ == 0) {
            new (get_address()) T(value);
            ++size_;
            return;
        }
        if (size_ == capacity()) {
            T* tmp = get_address() + last_elem_();
            (tmp)->~T();
        }
        first_ = first_ == 0 ? capacity() : first_ - 1;
        try {
            new (get_address() + first_) T(value);
        } catch (...) {
            first_ = (first_ + 1) % (capacity() + 1);
            throw;
        }
        size_ = std::min(size_ + 1, capacity());
    }
    void pop_front() {
        if (size_ == 0) {
            throw std::out_of_range("RE");
        }
        T* tmp = get_address() + first_;
        tmp->~T();
        first_ = (first_ + 1) % (capacity() + 1);
        --size_;
        if (size_ == 0) {
            first_ = 0;
        }
    }

    size_t last_elem_() const {
        return (size_ + first_ - 1) % (capacity() + 1);
    }
    void pop_back() {
        if (size_ == 0) {
            throw std::out_of_range("RE");
        }
        T* tmp = get_address() + last_elem_();
        tmp->~T();
        --size_;
        if (size_ == 0) {
            first_ = 0;
        }
    }

    template <bool>
    size_t capacity_helper() const {}

    template<>
    size_t capacity_helper<true>() const { return IsType<Capacity == DYNAMIC_CAPACITY>::capacity_; }

    template<>
    size_t capacity_helper<false>() const { return Capacity; }

    size_t capacity() const {
        return capacity_helper<DYNAMIC_CAPACITY == Capacity>();
    }

    size_t size() const { return size_; }

    bool empty() const { return size_ == 0; }

    bool full() const { return size_ == capacity(); }

    T& operator[](size_t index) {
        return get_address()[(index + first_) % (capacity() + 1)];
    }

    const T& operator[](size_t index) const {
        return get_address()[(index + first_) % (capacity() + 1)];
    }

    T& at(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("");
        }
        return get_address()[(index + first_) % (capacity() + 1)];
    }

    template <size_t>
    T* get_address_helper()  {
        return reinterpret_cast<T*>(&arr_[0]);
    }

    template <>
    T* get_address_helper<DYNAMIC_CAPACITY>() {
        return reinterpret_cast<T*>(arr_);
    }
    void* align_helper(const T* tmp) {
        T* tmp_non_const = const_cast<T*>(tmp); 
        return reinterpret_cast<void*>(tmp_non_const);
    }
    T* get_address() {
        const T* tmp = get_address_helper<Capacity>();
        void* to_align = align_helper(tmp);
        size_t fake_space = DYNAMIC_CAPACITY;
        std::align(alignof(T), sizeof(T), to_align, fake_space);
        return reinterpret_cast<T*>(to_align);
    }

    template <size_t>
    const T* get_address_helper() const {
        return reinterpret_cast<const T*>(&arr_[0]);
    }

    template <>
    const T* get_address_helper<DYNAMIC_CAPACITY>() const{
        return reinterpret_cast<const T*>(arr_);
    }

    const T* get_address() const{
        return get_address_helper<Capacity>();
    }


    template <bool IsConst, size_t ADDRESS>
    class circ_iterator:public IsType<ADDRESS == DYNAMIC_CAPACITY> {
    public:

        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<IsConst, const T, T>;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::random_access_iterator_tag;
        circ_iterator(pointer ptr_, size_t posin_, size_t variation_, size_t iter_first_)
        : ptr_{ptr_}, start_(iter_first_), pos_{posin_}
        {
            if constexpr (Capacity == DYNAMIC_CAPACITY) {
                IsType<ADDRESS == DYNAMIC_CAPACITY>::capacity_ = variation_;
            }
        }

        pointer operator->() const { return (ptr_ + pos_); }

        reference operator*() const { return *(ptr_ + pos_); }

        circ_iterator& operator++() {
            if (++pos_ >= iterator_cap() + 1) {
                pos_ = 0;
            }
            return *this;
        }

        circ_iterator(const circ_iterator& other_): ptr_ {other_.ptr_}, start_(other_.start_), pos_{other_.pos_} {
            if constexpr (Capacity == DYNAMIC_CAPACITY) {
                IsType<ADDRESS == DYNAMIC_CAPACITY>::capacity_ = other_.iterator_cap();
            }
        }

        circ_iterator& operator=(const circ_iterator& other_) {
            pos_ = other_.pos_;
            return *this;
        }

        circ_iterator& operator--() {
            pos_ = pos_ == 0 ? iterator_cap() : pos_ - 1;
            return *this;
        }

        circ_iterator& operator+=(size_t add_) {
            pos_ = (pos_ + add_) % (iterator_cap() + 1);
            return *this;
        }
        circ_iterator& operator-=(size_t sub_) {
            pos_ = ((pos_ - (sub_ % (iterator_cap() + 1))) + (iterator_cap() + 1)) % (iterator_cap() + 1);
            return *this;
        }
        circ_iterator operator+(size_t add_) const {
            circ_iterator to_ret_ = *this;
            to_ret_ += add_;
            return to_ret_;
        }

        circ_iterator operator-(size_t sub_) const {
            circ_iterator to_ret_ = *this;
            to_ret_ -= sub_;
            return to_ret_;
        }

        difference_type operator-(const circ_iterator& comp_) const {
            difference_type l1 = comp_.pos_ >= start_? comp_.pos_ - start_ : (iterator_cap() + 1 - start_) + comp_.pos_;
            difference_type l2 = pos_ >= start_? pos_ - start_ : (iterator_cap() + 1 - start_) + pos_;
            return l2 - l1;
        }

        bool operator<(const circ_iterator& comp_) const {
            difference_type diff = *this - comp_;
            return (diff < 0);
        }

        bool operator==(const circ_iterator& comp_) const { return pos_ == comp_.pos_; };

        bool operator!= (const circ_iterator& comp_) const { return !(*this == comp_); }

        bool operator>(const circ_iterator& comp_) const { return (comp_ < *this);  }

        bool operator>=(const circ_iterator& comp_) const { return !(*this < comp_); }

        bool operator<=(const circ_iterator& comp_) const { return !(*this > comp_); }

        size_t return_pos() const { return pos_; }


        template<bool>
        size_t iterator_cap_helper() const {
            return IsType<ADDRESS == DYNAMIC_CAPACITY>::capacity_;
        }

        template<>
        size_t iterator_cap_helper<false>() const {
            return Capacity;
        }

        size_t iterator_cap() const {
            return iterator_cap_helper<ADDRESS == DYNAMIC_CAPACITY>();
        }

        operator circ_iterator<true, ADDRESS>() const {
            return circ_iterator<true, ADDRESS>(ptr_, pos_, iterator_cap(), start_);
        }

        const pointer ptr_;
        const size_t start_;
        size_t pos_ = 0;
    };

    using iterator = circ_iterator<false, Capacity>;
    using const_iterator = circ_iterator<true, Capacity>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator(get_address(), first_, capacity(), first_);
    }

    const_iterator begin() const { return const_iterator(get_address(), first_, capacity(), first_); }
    const_iterator cbegin() const { return const_iterator(get_address(), first_, capacity(), first_); }

    iterator end() { return iterator(get_address(), (last_elem_() + 1), capacity(), first_); }

    const_iterator end() const { return const_iterator(get_address(), (last_elem_() + 1), capacity(), first_); }
    const_iterator cend() const { return const_iterator(get_address(), (last_elem_() + 1), capacity(), first_); }

    reverse_iterator rbegin() { return std::make_reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return std::make_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const { return std::make_reverse_iterator(cend()); }

    reverse_iterator rend() { return std::make_reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return std::make_reverse_iterator(begin()); }
    const_reverse_iterator crend() const  { return std::make_reverse_iterator(cbegin()); }

    void erase(iterator del_) {
        while (del_ != end() - 1) {
            std::swap(*del_, *(del_ + 1));
            ++del_;
        }
        pop_back();
    }

    void insert(iterator add_, const T& val_) {
        if ((size_ == capacity()) && (add_ == begin())) {
            return;
        }
        if (size_ == capacity()) {
            iterator tr_ = begin();
            --add_;
            while (tr_ != add_) {
                std::swap(*tr_, *(tr_ + 1));
                ++tr_;
            }
            *add_ = val_;
            return;
        }
        if (size_ == 0) {
            push_back(val_);
            return;
        }
        push_back(*begin());
        iterator new_last_ = end() - 1;
        while (new_last_ != add_) {
            std::swap(*new_last_, *(new_last_ - 1));
            --new_last_;
        }
        (*this)[add_.pos_] = val_;
    }

    size_t& cap_swap_() {
        return IsType<true>::capacity_;
    }

    void swap(CircularBuffer& other_) {
        std::swap(arr_, other_.arr_);
        if constexpr (Capacity == DYNAMIC_CAPACITY) {
            std::swap(other_.cap_swap_(),cap_swap_());
        }
        std::swap(other_.first_, first_);
        std::swap(other_.size_, size_);
    }

    friend iterator operator+(size_t add_, const iterator& me_) {
        return me_ + add_;
    }

};

