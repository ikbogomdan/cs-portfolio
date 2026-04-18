#include <type_traits>
#include <memory>
#include <algorithm>

template <size_t SIZE_>
class StackStorage {
public:
    std::byte mem_array_[SIZE_];
    void* pos_ = mem_array_;
    size_t cap_ = SIZE_;
};

template <typename T, size_t N>
class StackAllocator {
public:

    StackStorage<N>& storage_;
    using value_type = T;

    template <size_t SIZE_>
    StackAllocator(StackStorage<SIZE_>& a): storage_(a){}

    template <typename U, size_t M>
    StackAllocator(const StackAllocator<U, M>& a): storage_(a.storage_) {}

    StackAllocator& operator=(const StackAllocator& other_) {
        storage_ = other_.storage_;
        return *this;
    }

    T* allocate(size_t n) {
        void* tmp_ = storage_.pos_;
        std::align(alignof(T), sizeof(T), tmp_, storage_.cap_);
        storage_.pos_ = reinterpret_cast<std::byte*>(tmp_) + n * sizeof(T);
        return reinterpret_cast<T*>(tmp_);
    }

    bool operator==(const StackAllocator& other_) {
        return storage_.mem_array_ == other_.storage_.mem_array_;
    }

    bool operator!=(const StackAllocator& other_) {
        return !(storage_.mem_array_ == other_.storage_.mem_array_);
    }

    void deallocate(void*, size_t) {}

    template <typename U>
    struct rebind { using other = StackAllocator<U, N>; };

    ~StackAllocator() {}
};

template <typename T, typename Allocator = std::allocator<T>>
class List {
public:

    struct BaseNode {
        BaseNode* prev_;
        BaseNode* next_;

        BaseNode(BaseNode* prev, BaseNode* next): prev_(prev), next_(next){}
    };

    struct Node: BaseNode {
        alignas(alignof(T)) std::byte value_[sizeof(T)];
        Node(BaseNode* prev, BaseNode* next): BaseNode(prev, next){}
    };

    using NodeAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    [[no_unique_address]] NodeAlloc alloc_;
    using DeclAlloc = std::allocator_traits<decltype(alloc_)>;
    BaseNode fake_node_{&fake_node_, &fake_node_};
    size_t size_ = 0;

    List(): alloc_ (Allocator()) {}

    List(const Allocator& alloc_insert_): alloc_(alloc_insert_) {}

    List(size_t cnt_): List(cnt_, Allocator()) {}

    List(const size_t cnt_, const T& to_add_, const Allocator& t): List(t) {
        for (size_t i = 0; i < cnt_; i++) {
            push_back(to_add_);
        }
    }

    List(const size_t cnt_, const Allocator& t): List(t) {
        for (size_t i = 0; i < cnt_; i++) {
            Node* new_ = alloc_.allocate(1);
            std::allocator_traits<NodeAlloc>::construct(alloc_, new_, fake_node_.prev_, &fake_node_);
            Allocator t_alloc(alloc_);
            std::allocator_traits<Allocator>::construct(t_alloc, reinterpret_cast<T*>(new_->value_));
            fake_node_.prev_->next_ = new_;
            fake_node_.prev_ = new_;
            ++size_;
        }
    }

    List(const size_t cnt_, const T& to_add_): List(Allocator()) {
        for (size_t i = 0; i < cnt_; i++) {
            push_back(to_add_);
        }
    }

    List(const List& other_):
        alloc_(std::allocator_traits<Allocator>::select_on_container_copy_construction(other_.alloc_)) {
        try {
            for (auto &x: other_) {
                push_back(x);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    List& operator=(const List& other_) {
        if (this == &other_) {
            return *this;
        }
        List copy_(alloc_);
        if (std::allocator_traits<Allocator>::
            propagate_on_container_copy_assignment::value) {
            copy_.alloc_ = other_.alloc_;
        }
        for (auto &x: other_) {
            copy_.push_back(x);
        }
        swap(copy_, true);
        return *this;
    }

    void link_node(const T& value_insert_, Node* new_, BaseNode* new_prev, BaseNode* new_next) {
        try {
            std::allocator_traits<NodeAlloc>::construct(alloc_, new_, new_prev, new_next);
            Allocator t_alloc(alloc_);
            std::allocator_traits<Allocator>::construct(t_alloc, reinterpret_cast<T*>(new_->value_), value_insert_);
        } catch (...) {
            alloc_.deallocate(new_, 1);
            throw;
        }
        new_prev->next_ = new_;
        new_next->prev_ = new_;
        ++size_;
    }

    void push_back(const T& value_insert_) {
        Node* new_ = alloc_.allocate(1);
        link_node(value_insert_, new_, fake_node_.prev_, &fake_node_);
    }

    void push_front(const T& value_insert_) {
        Node* new_ = alloc_.allocate(1);
        link_node(value_insert_, new_, &fake_node_, fake_node_.next_);
    }

    void unlink_node(Node* unlink_) {
        if (empty()) return;
        unlink_->prev_->next_ = unlink_->next_;
        unlink_->next_->prev_ = unlink_->prev_;
        Allocator t_alloc(alloc_);
        std::allocator_traits<Allocator>::destroy(t_alloc, reinterpret_cast<T*>(unlink_->value_));
        DeclAlloc::destroy(alloc_, unlink_);
        DeclAlloc::deallocate(alloc_, unlink_, 1);
        --size_;
        if (size_ == 0) {
            fake_node_.prev_ = &fake_node_;
            fake_node_.next_ = &fake_node_;
        }
    }

    void pop_front() {
        unlink_node(static_cast<Node*>(fake_node_.next_));
    }

    void pop_back() {
        unlink_node(static_cast<Node*>(fake_node_.prev_));
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return fake_node_.prev_ == &fake_node_;
    }

    Allocator get_allocator() const {
        return typename DeclAlloc::template rebind_alloc<T>(alloc_);
    }

    void swap_propagate_false(List& other_) {
        List copy_ = *this;
        clear();
        for (auto &x: other_) {
            push_back(x);
        }
        other_.clear();
        for (auto &x: copy_) {
            other_.push_back(x);
        }
    }

    void swap(List& other_, bool from_assignment = false) {

        if (std::allocator_traits<Allocator>::propagate_on_container_swap::value || from_assignment) {
            std::swap(alloc_, other_.alloc_);
        } else if (alloc_ != other_.alloc_) {
            swap_propagate_false(other_);
            return;
        }

        if (size_ == 0 && other_.size_ == 0) return;
        if (other_.size_ == 0) {
            other_.fake_node_.next_ = fake_node_.next_;
            other_.fake_node_.prev_ = fake_node_.prev_;
            fake_node_.next_->prev_ = &other_.fake_node_;
            fake_node_.prev_->next_ = &other_.fake_node_;
            fake_node_.next_ = fake_node_.prev_ = &fake_node_;
        }
        else if (size_ == 0) {
            fake_node_.next_ = other_.fake_node_.next_;
            fake_node_.prev_ = other_.fake_node_.prev_;
            other_.fake_node_.next_->prev_ = &fake_node_;
            other_.fake_node_.prev_->next_ = &fake_node_;
            other_.fake_node_.next_ = other_.fake_node_.prev_ = &other_.fake_node_;
        }
        else {
            std::swap(fake_node_.next_, other_.fake_node_.next_);
            std::swap(fake_node_.prev_, other_.fake_node_.prev_);
            fake_node_.next_->prev_ = &fake_node_;
            fake_node_.prev_->next_ = &fake_node_;
            other_.fake_node_.next_->prev_ = &other_.fake_node_;
            other_.fake_node_.prev_->next_ = &other_.fake_node_;
        }
        std::swap(size_, other_.size_);
    }

    void clear() {
        while(!empty()) {
            pop_back();
        }
    }

    ~List() {
        clear();
    }

    template <bool IsConst>
    class list_iterator {
    public:

        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<IsConst, const T, T>;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category =  std::bidirectional_iterator_tag;

        list_iterator(const list_iterator& other_): addr_ (other_.addr_) {}

        list_iterator(const BaseNode* assign_) : addr_(const_cast<BaseNode*>(assign_)) {}

        pointer operator->() const { return reinterpret_cast<pointer>(&(static_cast<Node*>(addr_)->value_));}

        reference operator*() const { return reinterpret_cast<reference>(static_cast<Node*>(addr_)->value_); }

        list_iterator& operator++() {
            addr_ = addr_->next_;
            return *this;
        }

        list_iterator operator++(int) {
            addr_ = addr_->next_;
            return list_iterator(addr_->prev_);
        }

        list_iterator& operator--() {
            addr_ = addr_->prev_;
            return *this;
        }

        list_iterator operator--(int) {
            addr_ = addr_->prev_;
            return list_iterator(addr_->next_);
        }

        list_iterator& operator=(const list_iterator& brother_) {
            addr_ = brother_.addr_;
            return *this;
        }

        bool operator==(const list_iterator& other_) const { return addr_ == other_.addr_; }
        bool operator!=(const list_iterator& other_) const { return !(*this == other_); }

        void swap(list_iterator& first_, list_iterator& second_) {
            std::swap(*first_, *second_);
        }

        operator list_iterator<true>() const {
            return list_iterator<true>(addr_);
        }

        BaseNode* addr_;
    };

    using iterator = list_iterator<false>;
    using const_iterator = list_iterator<true>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() { return iterator(fake_node_.next_); }

    const_iterator begin() const { return const_iterator(fake_node_.next_); }
    const_iterator cbegin() const { return const_iterator(fake_node_.next_); }

    iterator end() { return iterator(&fake_node_); }

    const_iterator end() const { return const_iterator(&fake_node_); }
    const_iterator cend() const { return const_iterator(&fake_node_); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const { return rbegin(); }

    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const { return rend(); }

    void insert(const_iterator add_, const T& value_insert_) {
        Node* new_ = alloc_.allocate(1);
        try {
            std::allocator_traits<NodeAlloc>::construct(alloc_, new_, add_.addr_->prev_,
            add_.addr_);
            Allocator t_alloc(alloc_);
            std::allocator_traits<Allocator>::construct(t_alloc, reinterpret_cast<T*>(new_->value_), value_insert_);
        } catch (...) {
            DeclAlloc::deallocate(alloc_, new_, 1);
            throw;
        }
        static_cast<Node*>(add_.addr_)->prev_->next_ = new_;
        static_cast<Node*>(add_.addr_)->prev_ = new_;
        ++size_;
    }

    void erase(const_iterator del_) {
        if (del_ == &fake_node_) {
            throw std::invalid_argument("RE");
        }
        static_cast<Node*>(del_.addr_)->prev_->next_ = static_cast<Node*>(del_.addr_)->next_;
        static_cast<Node*>(del_.addr_)->next_->prev_ = static_cast<Node*>(del_.addr_)->prev_;
        DeclAlloc::destroy(alloc_, static_cast<Node*>(del_.addr_));
        DeclAlloc::deallocate(alloc_, static_cast<Node*>(del_.addr_), 1);
        --size_;
    }
};

