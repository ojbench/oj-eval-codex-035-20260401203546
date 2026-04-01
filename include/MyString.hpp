#ifndef SIMPLE_STRING_SIMPLESTRING_HPP
#define SIMPLE_STRING_SIMPLESTRING_HPP

#include <stdexcept>
#include <cstring>

class MyString {
private:
    union {
        char* heap_ptr;
        char small_buffer[16];
    };
    size_t len_ = 0;
    size_t cap_ = 15; // excluding null terminator
    bool is_small_ = true;

    char* data_ptr() { return is_small_ ? small_buffer : heap_ptr; }
    const char* data_ptr() const { return is_small_ ? small_buffer : heap_ptr; }

    void ensure_capacity(size_t new_capacity) {
        if (new_capacity <= capacity()) return;
        size_t target = cap_ < 16 ? 16 : cap_;
        while (target < new_capacity) {
            size_t grow = target + target / 2; // 1.5x
            if (grow <= target) { grow = new_capacity; }
            target = grow;
        }
        char* new_heap = new char[target + 1];
        if (len_ > 0) std::memcpy(new_heap, data_ptr(), len_);
        new_heap[len_] = '\0';
        if (!is_small_) delete[] heap_ptr;
        heap_ptr = new_heap;
        is_small_ = false;
        cap_ = target;
    }

public:
    MyString() {
        is_small_ = true; len_ = 0; cap_ = 15; small_buffer[0] = '\0';
    }

    MyString(const char* s) {
        if (!s) { is_small_ = true; len_ = 0; cap_ = 15; small_buffer[0] = '\0'; return; }
        size_t n = std::strlen(s);
        if (n <= 15) {
            is_small_ = true; cap_ = 15; len_ = n;
            if (n) std::memcpy(small_buffer, s, n);
            small_buffer[n] = '\0';
        } else {
            is_small_ = false; cap_ = n;
            heap_ptr = new char[cap_ + 1];
            std::memcpy(heap_ptr, s, n);
            heap_ptr[n] = '\0';
            len_ = n;
        }
    }

    MyString(const MyString& other) {
        len_ = other.len_;
        cap_ = other.is_small_ ? 15 : other.cap_;
        is_small_ = other.is_small_;
        if (is_small_) {
            if (len_) std::memcpy(small_buffer, other.small_buffer, len_);
            small_buffer[len_] = '\0';
        } else {
            heap_ptr = new char[cap_ + 1];
            if (len_) std::memcpy(heap_ptr, other.heap_ptr, len_);
            heap_ptr[len_] = '\0';
        }
    }

    MyString(MyString&& other) noexcept {
        len_ = other.len_;
        cap_ = other.is_small_ ? 15 : other.cap_;
        is_small_ = other.is_small_;
        if (is_small_) {
            if (len_) std::memcpy(small_buffer, other.small_buffer, len_);
            small_buffer[len_] = '\0';
        } else {
            heap_ptr = other.heap_ptr;
            if (heap_ptr) heap_ptr[len_] = '\0';
            other.heap_ptr = nullptr;
        }
        other.is_small_ = true; other.len_ = 0; other.cap_ = 15; other.small_buffer[0] = '\0';
    }

    MyString& operator=(MyString&& other) noexcept {
        if (this == &other) return *this;
        if (!is_small_ && heap_ptr) delete[] heap_ptr;
        len_ = other.len_; cap_ = other.is_small_ ? 15 : other.cap_; is_small_ = other.is_small_;
        if (is_small_) {
            if (len_) std::memcpy(small_buffer, other.small_buffer, len_);
            small_buffer[len_] = '\0';
        } else {
            heap_ptr = other.heap_ptr; if (heap_ptr) heap_ptr[len_] = '\0'; other.heap_ptr = nullptr;
        }
        other.is_small_ = true; other.len_ = 0; other.cap_ = 15; other.small_buffer[0] = '\0';
        return *this;
    }

    MyString& operator=(const MyString& other) {
        if (this == &other) return *this;
        if (!is_small_) { delete[] heap_ptr; }
        len_ = other.len_; cap_ = other.is_small_ ? 15 : other.cap_; is_small_ = other.is_small_;
        if (is_small_) {
            if (len_) std::memcpy(small_buffer, other.small_buffer, len_);
            small_buffer[len_] = '\0';
        } else {
            heap_ptr = new char[cap_ + 1];
            if (len_) std::memcpy(heap_ptr, other.heap_ptr, len_);
            heap_ptr[len_] = '\0';
        }
        return *this;
    }

    ~MyString() {
        if (!is_small_) { delete[] heap_ptr; heap_ptr = nullptr; }
    }

    const char* c_str() const { return data_ptr(); }
    size_t size() const { return len_; }
    size_t capacity() const { return is_small_ ? 15 : cap_; }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity()) return; ensure_capacity(new_capacity);
    }

    void resize(size_t new_size) {
        if (new_size <= capacity()) {
            if (!is_small_ && new_size <= 15) {
                size_t copy_n = new_size < len_ ? new_size : len_;
                char tmp[16]; if (copy_n) std::memcpy(tmp, heap_ptr, copy_n); if (copy_n < 16) tmp[copy_n] = '\0';
                delete[] heap_ptr; is_small_ = true; cap_ = 15; len_ = copy_n;
                if (copy_n) std::memcpy(small_buffer, tmp, copy_n); small_buffer[copy_n] = '\0';
            } else {
                if (new_size > len_) { char* d = data_ptr(); std::memset(d + len_, '\0', new_size - len_); }
                len_ = new_size; data_ptr()[len_] = '\0';
            }
        } else {
            ensure_capacity(new_size);
            if (new_size > len_) std::memset(heap_ptr + len_, '\0', new_size - len_);
            len_ = new_size; heap_ptr[len_] = '\0';
        }
    }

    char& operator[](size_t index) {
        if (index >= len_) throw std::out_of_range("index out of range");
        return is_small_ ? small_buffer[index] : heap_ptr[index];
    }

    MyString operator+(const MyString& rhs) const {
        MyString res; size_t total = len_ + rhs.len_;
        if (total <= 15) {
            res.is_small_ = true; res.cap_ = 15; res.len_ = total;
            if (len_) std::memcpy(res.small_buffer, data_ptr(), len_);
            if (rhs.len_) std::memcpy(res.small_buffer + len_, rhs.data_ptr(), rhs.len_);
            res.small_buffer[total] = '\0';
        } else {
            res.is_small_ = false; res.cap_ = total; res.heap_ptr = new char[res.cap_ + 1];
            if (len_) std::memcpy(res.heap_ptr, data_ptr(), len_);
            if (rhs.len_) std::memcpy(res.heap_ptr + len_, rhs.data_ptr(), rhs.len_);
            res.heap_ptr[total] = '\0'; res.len_ = total;
        }
        return res;
    }

    void append(const char* str) {
        if (!str) return; size_t m = std::strlen(str); if (m == 0) return;
        size_t new_len = len_ + m; if (new_len > capacity()) ensure_capacity(new_len);
        char* d = data_ptr(); std::memcpy(d + len_, str, m); len_ = new_len; d[len_] = '\0';
    }

    const char& at(size_t pos) const {
        if (pos >= len_) throw std::out_of_range("at out of range");
        return is_small_ ? small_buffer[pos] : heap_ptr[pos];
    }

    class const_iterator;
    class iterator {
    private:
        MyString* owner_ = nullptr; size_t idx_ = 0;
    public:
        iterator() = default; iterator(MyString* o, size_t i) : owner_(o), idx_(i) {}
        iterator& operator++() { ++idx_; return *this; }
        iterator operator++(int) { iterator tmp(*this); ++(*this); return tmp; }
        iterator& operator--() { --idx_; return *this; }
        iterator operator--(int) { iterator tmp(*this); --(*this); return tmp; }
        char& operator*() const { return (*owner_)[idx_]; }
        bool operator==(const iterator& other) const { return owner_==other.owner_ && idx_==other.idx_; }
        bool operator!=(const iterator& other) const { return !(*this==other); }
        bool operator==(const const_iterator& other) const;
        bool operator!=(const const_iterator& other) const;
    };

    class const_iterator {
    private:
        const MyString* owner_ = nullptr; size_t idx_ = 0;
    public:
        const_iterator() = default; const_iterator(const MyString* o, size_t i) : owner_(o), idx_(i) {}
        const_iterator& operator++() { ++idx_; return *this; }
        const_iterator operator++(int) { const_iterator tmp(*this); ++(*this); return tmp; }
        const_iterator& operator--() { --idx_; return *this; }
        const_iterator operator--(int) { const_iterator tmp(*this); --(*this); return tmp; }
        const char& operator*() const { return owner_->at(idx_); }
        bool operator==(const const_iterator& other) const { return owner_==other.owner_ && idx_==other.idx_; }
        bool operator!=(const const_iterator& other) const { return !(*this==other); }
        friend class iterator;
    };

    // cross-iterator comparisons are defined after the class

public:
    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, len_); }
    const_iterator cbegin() const { return const_iterator(this, 0); }
    const_iterator cend() const { return const_iterator(this, len_); }
};

// Define cross-iterator comparisons out of class with proper scope
inline bool MyString::iterator::operator==(const MyString::const_iterator& other) const {
    return owner_ == other.owner_ && idx_ == other.idx_;
}
inline bool MyString::iterator::operator!=(const MyString::const_iterator& other) const {
    return !(*this == other);
}

#endif
