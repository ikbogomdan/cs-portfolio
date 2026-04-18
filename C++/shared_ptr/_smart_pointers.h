#include <type_traits>

template <typename T> struct ControlBlockShared;
template <typename T> struct ControlBlockStandart;
template <typename T> class WeakPtr;
template <typename T> class EnableSharedFromThis;

template <typename T>
struct deleter_base {
    void operator()(T* ptr) {
        delete ptr;
    }
};

struct BaseControlBlock {
    virtual void* get_from_block() = 0;
    virtual void destroy() = 0;
    virtual void kill() = 0;

    size_t shared_counter_ = 0;
    size_t weak_counter_ = 0;

    BaseControlBlock(size_t a, size_t b): shared_counter_(a), weak_counter_(b) {}

    int hit_w() {
        count_decrease_w();
        if (!weak_counter_ && !shared_counter_) {
            return 0;
        }
        return 1;
    }

    int hit() {
        count_decrease();
        if (!shared_counter_ && !weak_counter_) {
            return 0;
        }
        if (!shared_counter_) {
            return 1;
        }
        return 2;
    }

    size_t count_w() const { return weak_counter_; }

    size_t count() const { return shared_counter_; }

    void count_increase_w() {  ++weak_counter_; }

    void count_decrease_w() {
        if (weak_counter_ > 0) --weak_counter_;
    }

    void count_increase() { ++shared_counter_; }

    void count_decrease() {
        if (shared_counter_ > 0) --shared_counter_;
    }
};

template <typename T> class SharedPtr {
template <typename U> friend class WeakPtr;
template <typename U, typename... Args> friend SharedPtr <U> makeShared (Args&&... args);
template <typename U, typename Alloc, typename... Args> friend SharedPtr<U> allocateShared (const Alloc& alloc, Args&&... args);
template <typename U> struct ControlBlock;
template <typename U> friend class SharedPtr;

    T* ptr_ = nullptr;
    BaseControlBlock* control_block_ = nullptr;

    template <typename U, typename Alloc = std::allocator<U>, typename Deleter = deleter_base<U>>
    struct ControlBlockStandart: BaseControlBlock {

        ControlBlockStandart(size_t shared, size_t weak,
            U* ptr, Deleter del = Deleter(), Alloc alloc = Alloc()): BaseControlBlock(shared, weak),
        point_(ptr), deleter_(del), alloc_(alloc) {}

        U* point_ = nullptr;
        [[no_unique_address]] Deleter deleter_;
        [[no_unique_address]] Alloc alloc_;

        using  Suicide =
            typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockStandart<U, Alloc, Deleter>>;
        Suicide is_not_a_solution;

        void kill() override {
            std::allocator_traits<Suicide>::deallocate(is_not_a_solution, this, 1);
        }

        ~ControlBlockStandart() = delete;

        void destroy() override { deleter_(point_); }

        void* get_from_block() override {
            return nullptr;
        }
    };

    template <typename U, typename Alloc = std::allocator<U>>
    struct ControlBlockShared: BaseControlBlock {

        template <typename... Args>
        ControlBlockShared(size_t shared, size_t weak, Alloc alloc, Args&&... args)
        : BaseControlBlock(shared, weak),
          alloc_(alloc) {
            new (value_) U(std::forward<Args>(args)...);
        }

        ControlBlockShared(size_t shared, size_t weak): BaseControlBlock(shared, weak) {}

        ~ControlBlockShared() = delete;

        alignas(alignof(U)) std::byte value_[sizeof(U)];
        Alloc alloc_;
        using  Suicide = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockShared<U, Alloc>>;
        Suicide is_not_a_solution;

        void destroy() override {
            std::allocator_traits<Alloc>::destroy(alloc_, reinterpret_cast<U*>(&value_[0]));
        }

        void kill() override {
            std::allocator_traits<Suicide>::deallocate(is_not_a_solution, this, 1);
        }

        void* get_from_block() override { return &value_[0]; }

    };

    SharedPtr(BaseControlBlock* block): control_block_(block) {
        control_block_->count_increase();
    }

    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;

public:

    pointer operator->() const {
        return ptr_ ? ptr_ : reinterpret_cast<pointer>(control_block_->get_from_block());
    }
    reference operator*() const {
        return *(operator->());
    }

    template <typename U, typename Deleter>
    SharedPtr(U* ptr, Deleter deleter) {
        control_block_ = new ControlBlockStandart<U, std::allocator<U>, Deleter>{1, 0, ptr, deleter};
        ptr_ = static_cast<T*>(ptr);
    }

    template <typename U, typename Deleter, typename Allocator>
    SharedPtr(U* ptr, Deleter deleter, Allocator alloc) {
        using BlockType = ControlBlockStandart<U, Allocator, Deleter>;
        using BlockAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<BlockType>;
        BlockAlloc block_alloc(alloc);
        BlockType* block = block_alloc.allocate(1);
        try {
            new (block) BlockType(1, 0, ptr, deleter, alloc);
            control_block_ = block;
            ptr_ = static_cast<T*>(ptr);
        } catch (...) {
            block_alloc.deallocate(block, 1);
            throw;
        }
    }

    template<typename U>
    SharedPtr(const SharedPtr<U>& other, T* ptr) {
        static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U>, "CE");
        if (other.use_count() == 0) {
            ptr_ = nullptr;
            control_block_ = nullptr;
            return;
        }
        control_block_ = other.control_block_;
        ptr_ = ptr;
        control_block_->count_increase();
    }

    template <typename U>
    explicit SharedPtr (U* ptr) {
        static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U>, "CE");
        ptr_ = static_cast<T*>(ptr);
        control_block_ = new ControlBlockStandart<U>{1, 0, ptr_};
        if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
            ptr->weak_this = *this;
        }
    }

    SharedPtr (SharedPtr&& other) noexcept : ptr_(other.ptr_), control_block_(other.control_block_) {
        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    template <typename U>
    SharedPtr (SharedPtr<U>&& other) noexcept  {
        static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U>, "CE");
        ptr_ = static_cast<T*>(other.ptr_);
        control_block_ = other.control_block_;
        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    T* get() const {
        if (control_block_ == nullptr) {
            return nullptr;
        }
        return ptr_ ? ptr_ : reinterpret_cast<T*>(control_block_->get_from_block());
    }

    template <typename U>
    SharedPtr (const SharedPtr<U>& other)  {
        static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U>, "CE");
        if (other.use_count() == 0) {
            ptr_ = nullptr;
            control_block_ = nullptr;
            return;
        }
        control_block_ = other.control_block_;
        ptr_ = reinterpret_cast<T*>(other.get());
        control_block_->count_increase();
    }

    SharedPtr (const SharedPtr& other):
        ptr_(other.ptr_),
        control_block_(other.control_block_) {
        if (other.use_count() == 0) {
            ptr_ = nullptr;
            control_block_ = nullptr;
            return;
        }
        control_block_->count_increase();
    }

    size_t use_count() const {
        return control_block_ ? control_block_->count() : 0;;
    }

    void swap(SharedPtr& other) {
        std::swap(other.control_block_, control_block_);
        std::swap(other.ptr_, ptr_);
    }

    void reset() {
        if (control_block_) {
            int case_ = control_block_->hit();
            if (case_ < 2) {
                control_block_->destroy();
            }
            if (case_ == 0) {
                control_block_->kill();
            }
        }
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    ~SharedPtr () {
        reset();
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    void reset(T* other) {
        reset();
        ptr_ = other;
        control_block_ = new ControlBlockStandart<T>{1, 0, ptr_};
    }

    SharedPtr() {}

    SharedPtr& operator= (T* other) {
        reset();
        ptr_ = other;
        control_block_ = new ControlBlockStandart<T>{1, 0, ptr_};
        return *this;
    }

    SharedPtr& operator=(const SharedPtr& other) {
        reset();
        SharedPtr tmp(other);
        swap(tmp);
        return *this;
    }
    SharedPtr& operator= (SharedPtr&& other) {
        reset();
        swap(other);
        return *this;
    }
};

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
    BaseControlBlock* block_ =
        new typename SharedPtr<T>::template ControlBlockShared<T>(0, 0, std::allocator<T>{}, std::forward<Args>(args)...);
    return SharedPtr<T>(block_);
}


template <typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(const Alloc& alloc, Args&&... args) {
    using BlockType = typename SharedPtr<T>::template ControlBlockShared<T, Alloc>;
    using BlockAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<BlockType>;
    BlockAlloc block_alloc(alloc);
    BaseControlBlock* block = block_alloc.allocate(1);
    try {
        std::allocator_traits<BlockAlloc>::construct(
            block_alloc,
            static_cast<BlockType*>(block), 0, 0, alloc, std::forward<Args>(args)...);
        return SharedPtr<T>(block);
    } catch (...) {
        block_alloc.deallocate(static_cast<BlockType*>(block), 1);
        throw;
    }
}

template <typename T>
class WeakPtr {

    template <typename U> friend class WeakPtr;
    BaseControlBlock* control_block_ = nullptr;
    T* ptr_ = nullptr;

public:

    explicit WeakPtr(const SharedPtr<T>& other): control_block_(other.control_block_), ptr_(other.ptr_) {
        if (control_block_) {
            control_block_->count_increase_w();
        }
    }

    bool expired() const {
        if (control_block_ == nullptr) {
            return true;
        }
        return control_block_->count() == 0;
    }

    SharedPtr<T> lock() const {
        return SharedPtr<T>(control_block_);
    }

    size_t use_count() const {
        return control_block_->count();
    }

    WeakPtr(WeakPtr&& other) noexcept :control_block_(other.control_block_), ptr_(other.ptr_) {
        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    WeakPtr(const WeakPtr& other): control_block_(other.control_block_), ptr_(other.ptr_) {
        if (control_block_ != nullptr) {
            control_block_->count_increase_w();
        }
    }

    template <typename U>
    WeakPtr(const SharedPtr<U>& other) {
        if constexpr (std::is_same_v<T, U> || std::is_base_of_v<T, U>) {
            control_block_ = other.control_block_;
            ptr_ = other.ptr_;
            if (control_block_ != nullptr) {
                control_block_->count_increase_w();
            }
        }
    }

    template <typename U>
    WeakPtr(const WeakPtr<U>& other) {
        if constexpr (std::is_same_v<T, U> || std::is_base_of_v<T, U>) {
            control_block_ = other.control_block_;
            ptr_ = other.ptr_;
            if (control_block_ != nullptr) {
                control_block_->count_increase_w();
            }
        }
    }

    void reset() {
        if (control_block_ != nullptr) {
            if (!(control_block_->hit_w())) {
                control_block_->kill();
            }
        }
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    WeakPtr& operator=(const SharedPtr<T>& other) {
        reset();
        WeakPtr tmp(other);
        swap(tmp);
        return *this;
    }

    WeakPtr& operator= (const WeakPtr& other) {
        reset();
        WeakPtr tmp(other);
        swap(tmp);
        return *this;
    }

    void swap(WeakPtr& other) {
        std::swap(control_block_, other.control_block_);
        std::swap(ptr_, other.ptr_);
    }

    WeakPtr& operator= (WeakPtr&& other) {
        reset();
        WeakPtr tmp(std::move(other));
        swap(tmp);
        return *this;
    }

    WeakPtr(): control_block_(nullptr), ptr_(nullptr) {}

    ~WeakPtr() { reset(); }

};

template <typename T>
class EnableSharedFrom {
    WeakPtr<T> weak_this;
public:
    std::shared_ptr<T> lock() const {
        return weak_this.lock();
    }
};
