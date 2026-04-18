#include <queue>
#include <vector>


template <typename T, typename Allocator = std::allocator<T>>
class List {
public:

    struct BaseNode {
        BaseNode* prev_;
        BaseNode* next_;
    };
    struct Node: BaseNode {
        alignas(alignof(T)) std::byte value_[sizeof(T)];
        size_t hash_;

        Node(size_t hash) : hash_(hash) {}
        Node(): hash_(0) {}
    };

    using NodeAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    [[no_unique_address]] NodeAlloc alloc_;
    using DeclAlloc = std::allocator_traits<decltype(alloc_)>;
    BaseNode fake_node_{&fake_node_, &fake_node_};
    size_t size_ = 0;

    List(): alloc_ (Allocator()) {}

    explicit List(const Allocator& other_alloc_): alloc_(other_alloc_) {}

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

    List(List&& other) noexcept {
        clear();
        swap(other, true);
    }

    List& operator=(List&& other_) {
        if (this == &other_) {
            return *this;
        }
        clear();
        if ((std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value) ||
            (alloc_ == other_.alloc_)) {
            swap(other_, true);
        } else {
            List copy_(std::move(other_));
            swap(copy_, true);
        }
        return *this;
    }

    void push_back(const T& value_insert_) {
        Node* new_ = alloc_.allocate(1);
        new_->prev_ = fake_node_.prev_;
        new_->next_= &fake_node_;
        try {
            std::allocator_traits<NodeAlloc>::construct(alloc_, &new_->value_, value_insert_);
        } catch (...) {
            alloc_.deallocate(new_, 1);
            throw;
        }
        fake_node_.prev_->next_ = new_;
        fake_node_.prev_ = new_;
        ++size_;
    }

    size_t size() const { return size_; }

    bool empty() const { return fake_node_.prev_ == &fake_node_; }

    void swap(List& other_, bool from_assignment = false) {
        if (std::allocator_traits<Allocator>::propagate_on_container_swap::value || from_assignment) {
            std::swap(alloc_, other_.alloc_);
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

        pointer operator->() const { return reinterpret_cast<T*>(static_cast<Node*>(addr_)->value_);}

        reference operator*() const { return reinterpret_cast<reference>(static_cast<Node*>(addr_)->value_); }

        size_t hash_node_value() const { return static_cast<Node*>(addr_)->hash_; }

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

        list_iterator& operator=(const list_iterator& other_) {
            addr_ = other_.addr_;
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

    void link_node(BaseNode* add_, BaseNode* new_) {
        new_->prev_ = add_->prev_;
        new_->next_= add_;
        add_->prev_->next_ = new_;
        add_->prev_ = new_;
        ++size_;
    }

    void unlink_node(Node* unlink_) {
        if (empty()) return;
        unlink_->prev_->next_ = unlink_->next_;
        unlink_->next_->prev_ = unlink_->prev_;
        --size_;
        if (size_ == 0) {
            fake_node_.prev_ = &fake_node_;
            fake_node_.next_ = &fake_node_;
        }
    }

    void node_destroy(Node* destroy_) {
        Allocator t_alloc(alloc_);
        std::allocator_traits<Allocator>::destroy(t_alloc, reinterpret_cast<T*>(destroy_->value_));
        DeclAlloc::destroy(alloc_, destroy_);
        DeclAlloc::deallocate(alloc_, destroy_, 1);
    }

    void pop_back() {
        Node* tmp_ = static_cast<Node*>(fake_node_.prev_);
        unlink_node(static_cast<Node*>(fake_node_.prev_));
        node_destroy(tmp_);
    }

    Node* queue_front_() {
        Node* tmp = static_cast<Node*>(fake_node_.next_);
        unlink_node(tmp);
        return tmp;
    }

    iterator insert_node(iterator add_, Node* new_) {
        link_node(add_.addr_, new_);
        return iterator(new_);
    }

    template <typename BaseAlloc>
    void rebuild(std::queue<Node*>& node_q_, std::vector<BaseNode*, BaseAlloc>& elem_bucket_,
        size_t bucket_count_) {
        while (!node_q_.empty()) {
            Node* next_ = node_q_.front();
            node_q_.pop();
            size_t find_hash_ = next_->hash_;
            size_t pos_ = find_hash_ % bucket_count_;
            iterator pos_add_ = elem_bucket_[pos_] != nullptr ? elem_bucket_[pos_] : begin();
            insert_node(iterator(pos_add_), next_);
            elem_bucket_[pos_] = elem_bucket_[pos_] != nullptr ?
                elem_bucket_[pos_]->prev_ :  elem_bucket_[pos_] = begin().addr_;
        }
    }

    iterator erase(const_iterator del_) {
        if (del_ == &fake_node_) {
            throw std::invalid_argument("Invalid address");
        }
        iterator ret_ = iterator(del_.addr_->next_);
        unlink_node(static_cast<Node*>(del_.addr_));
        node_destroy(static_cast<Node*>(del_.addr_));
        return ret_;
    }
};

template <typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>,
typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {

    using list_scope_ = List<std::pair<const Key, Value>>;
    using size_alloc_ = typename Alloc::size_type;
    using NodeType = std::pair<const Key, Value>;
    using NodeValueAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<NodeType>;
    using ListAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<typename list_scope_::Node>;
    using PairAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<std::pair<const Key, Value>>;
    using BaseNodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<typename list_scope_::BaseNode*>;

    using iterator = list_scope_::iterator;
    using const_iterator = list_scope_::const_iterator;

    const Key& first_in_pair_(typename list_scope_::Node* return_) const {
        return (reinterpret_cast<NodeType*>(return_->value_))->first;
    }

    template <typename P>
    std::pair<iterator, bool> insert_helper(P&& add_) {
        return emplace(std::forward<P>(add_));
    }

    template <typename P>
    Value& brackets_(P&& key_) {
        iterator it = find(key_);
        if (it == end()) {
            return emplace(std::forward<P>(key_), Value()).first->second;
        }
        return it->second;
    }

    bool while_(size_t hash_pos_, iterator& find_iter_, const Key& key_) {
        while ((find_iter_ != elem_.end()) &&
            ((find_iter_.hash_node_value() % bucket_count_) == (hash_pos_))) {
            if (eq_(key_, (*find_iter_).first)) {
                return true;
            }
            ++find_iter_;
            }
        return false;
    }

    void check_load() {
        if (load_factor() > max_load_factor()) {
            rehash(bucket_count_ * 2 + 1);
        }
    }

public:

    iterator begin() { return elem_.begin(); }
    iterator end() { return elem_.end(); }
    const_iterator cbegin() const { return elem_.cbegin(); }
    const_iterator begin() const { return elem_.begin(); }
    const_iterator end() const {  return elem_.end(); }
    const_iterator cend() const {  return elem_.cend(); }

    void reserve(size_t new_size_) {
        if (new_size_ > bucket_count_) {
            rehash(new_size_);
        }
    }

    size_alloc_ size() const { return elem_.size(); }

    float load_factor() const { return (elem_.size() + 1) / bucket_count_; }

    float max_load_factor() const { return max_load_factor_; }

    size_alloc_ bucket_count() const {  return bucket_count_; }

    size_alloc_ max_bucket_count() const { return max_bucket_count_;  }

    void rehash(size_t new_size_) {
        std::queue<typename list_scope_::Node*> node_q_;
        elem_bucket_.assign(new_size_, nullptr);
        while (!elem_.empty()) {
            node_q_.push(elem_.queue_front_());
        }
        bucket_count_ = new_size_;
        elem_.template rebuild<BaseNodeAlloc>(node_q_, elem_bucket_, bucket_count_);
    }

    template <class P>
    std::pair<iterator, bool> insert(P&& value) {
        if constexpr (std::is_constructible<NodeType, P&&>::value == false) {
            throw std::invalid_argument("Invalid argument");
        }
        return emplace(std::forward<P>(value));
    }

    std::pair<iterator, bool> insert(const NodeType& add_) {
        return insert_helper(add_);
    }

    std::pair<iterator, bool> insert(NodeType&& add_) {
        return insert_helper(std::move(add_));
    }

    template <typename Input>
    void insert(Input first_, Input last_) {
        while (first_ != last_) {
            insert(*first_);
            ++first_;
        }
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        check_load();
        typename list_scope_::Node* to_add_ = node_list_.allocate(1);
        try {
            std::allocator_traits<decltype(node_list_)>::construct(node_list_, to_add_);
            std::allocator_traits<decltype(node_alloc_)>::construct(node_alloc_,
                    reinterpret_cast<NodeType*>(to_add_->value_), std::forward<Args>(args)...);
        } catch (...) {
            elem_.node_destroy(to_add_);
            std::allocator_traits<decltype(node_list_)>::deallocate(node_list_, to_add_, 1);
            throw;
        }
        iterator is_find_ = find(first_in_pair_(to_add_));
        if (is_find_ != elem_.end()) {
            return std::make_pair(iterator(is_find_), false);
        }
        size_t find_hash_ = hash_make_(first_in_pair_(to_add_));
        to_add_->hash_ = find_hash_;
        size_t pos_ = find_hash_ % bucket_count_;
        iterator pos_add_ = elem_bucket_[pos_] != nullptr ? elem_bucket_[pos_] : elem_.begin();
        elem_.insert_node(iterator(pos_add_), to_add_);
        elem_bucket_[pos_] = elem_bucket_[pos_] != nullptr ? elem_bucket_[pos_]->prev_ : elem_.begin().addr_;
        return std::make_pair(iterator(elem_bucket_[pos_]), true);
    }

    iterator find(const Key& key_) {
        size_t hash_pos_ = hash_make_(key_) % bucket_count_;
        iterator find_iter_ = elem_bucket_[hash_pos_] != nullptr ? elem_bucket_[hash_pos_] : elem_.end();
        if (while_(hash_pos_, find_iter_, key_)) {
            return find_iter_;
        }
        return elem_.end();
    }

    iterator erase (const_iterator del_) {
        size_t pos_ = del_.hash_node_value() % bucket_count_;
        iterator find_iter_ = elem_bucket_[pos_] == nullptr ? elem_.end() : iterator(elem_bucket_[pos_]);
        if (!while_(pos_, find_iter_, (*del_).first)) {
            return find_iter_;
        }
        find_iter_ = elem_.erase(find_iter_);
        elem_bucket_[pos_]  = find_iter_.hash_node_value() % bucket_count_  != pos_ ?
            elem_bucket_[pos_] = nullptr : elem_bucket_[pos_] = find_iter_.addr_;
        return find_iter_;
    }

    void erase(const_iterator first_, const_iterator last_) {
        while (first_ != last_) {
            first_ = erase(first_);
        }
    }

    Value& operator[] (Key&& key_) {
        return brackets_(std::forward<Key>(key_));
    }

    Value& operator[] (const Key& key_) {
        return brackets_(std::forward<Key>(key_));
    }

    Value& at(const Key& key_) {
        iterator it = find(key_);
        if (it == end()) {
            throw std::out_of_range("There is no such key");
        }
        return it->second;
    }

    const Value& at(const Key& key_) const {
        iterator it = find(key_);
        if (it == end()) {
            throw std::out_of_range("There is no such key");
        }
        return it->second;
    }

    explicit UnorderedMap (size_t bucket_in_) : bucket_count_(bucket_in_), elem_bucket_(bucket_in_, nullptr) {}

    UnorderedMap () noexcept: hash_make_(Hash()), bucket_count_(1), eq_(Equal()), elem_bucket_(1, nullptr) {}

    UnorderedMap (const UnorderedMap& other_):
    hash_make_(other_.hash_make_),
    node_alloc_(std::allocator_traits<Alloc>::select_on_container_copy_construction(other_.node_alloc_)),
    node_list_(std::allocator_traits<Alloc>::select_on_container_copy_construction(other_.node_list_)),
    bucket_count_(other_.bucket_count_),
    max_load_factor_(other_.max_load_factor_),
    eq_(other_.eq_),
    elem_bucket_(other_.bucket_count_, nullptr),
    pair_alloc_(std::allocator_traits<Alloc>::select_on_container_copy_construction(other_.pair_alloc_)) {
        auto it = other_.begin();
        try {
            for (;it != other_.end(); ++it) {
                emplace(*it);
            }
        } catch (...) {
            while (size()) {
                erase(it);
                --it;
            }
            throw;
        }
    }

    UnorderedMap& operator=(const UnorderedMap& other_) {
        if (this == &other_) {
            return *this;
        }
        UnorderedMap tmp_;
        if ((std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value)) {
            tmp_.node_alloc_ = other_.node_alloc_;
            tmp_.node_list_ = other_.node_list_;
            tmp_.pair_alloc_ = other_.pair_alloc_;
        }
        for (auto &x: other_) {
            tmp_.emplace(x);
        }
        swap(tmp_, true);
        return *this;
    }

    UnorderedMap (UnorderedMap&& other_) : hash_make_(other_.hash_make_), node_alloc_(std::move(other_.node_alloc_)),
    node_list_(std::move(other_.node_list_)),  bucket_count_(other_.bucket_count_),
    max_load_factor_(other_.max_load_factor_), eq_(other_.eq_), elem_bucket_(std::move(other_.bucket_count_)),
    pair_alloc_(std::move(other_.pair_alloc_)) { other_.clear(); }

    UnorderedMap& operator=(UnorderedMap&& other_) noexcept {
        if (this == &other_) {
            return *this;
        }
        clear();
        if ((std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value) ||
            (node_alloc_ == other_.node_alloc_)) {
            swap(other_, true);
            } else {
                UnorderedMap tmp_(std::move(other_));
                swap(tmp_, true);
            }
        return *this;
    }

    void clear() {
        elem_.clear();
        elem_bucket_.clear();
    }

    void swap(UnorderedMap& other_, bool from_assignment = false) {
        if (std::allocator_traits<Alloc>::propagate_on_container_swap::value || from_assignment) {
            std::swap(node_alloc_, other_.node_alloc_);
            std::swap(pair_alloc_, other_.pair_alloc_);
            std::swap(node_alloc_, other_.node_alloc_);
        }
        std::swap(hash_make_, other_.hash_make_);
        std::swap(max_load_factor_, other_.max_load_factor_);
        std::swap(max_bucket_count_, other_.max_bucket_count_);
        std::swap(elem_, other_.elem_);
        std::swap(eq_, other_.eq_);
        std::swap(bucket_count_, other_.bucket_count_);
        std::swap(elem_bucket_, other_.elem_bucket_);
    }

private:

    List<NodeType> elem_;

    [[no_unique_address]] Hash hash_make_;

    [[no_unique_address]] NodeValueAlloc node_alloc_;

    [[no_unique_address]] ListAlloc node_list_;

    size_alloc_ bucket_count_;

    size_alloc_ max_bucket_count_;

    float max_load_factor_ = 1.f;

    [[no_unique_address]] Equal eq_ = Equal();

    std::vector<typename list_scope_::BaseNode*, BaseNodeAlloc> elem_bucket_;

    [[no_unique_address]] PairAlloc pair_alloc_;

};