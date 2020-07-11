#include "number_storage.h"


#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>



// Helpful methods

static number_storage::buffer* allocate_buffer(size_t size) {
    auto buf = reinterpret_cast<number_storage::buffer*>(operator new (sizeof(buffer) + sizeof(number_storage::number_t) * size));
    buf->ref_counter_ = 1;
    return buf;
}

// Private methods

void number_storage::init_dynamic(size_t capacity) {
    buffer* buf = allocate_buffer(capacity);
    if (sz_.is_big_ == 1) {
        std::copy(dynamic_data_.pb->data_, dynamic_data_.pb->data_ + sz_.size_, buf->data_);
    } else {
        std::copy(static_data_, static_data_ + sz_.size_, buf->data_);
    }
    clr();
    sz_.is_big_ = 1;
    dynamic_data_.pb = buf;
    dynamic_data_.capacity_ = capacity;
}

void number_storage::init_dynamic() {
    init_dynamic(sz_.size_);
}

void number_storage::separate() {
    if (sz_.is_big_ == 1 && dynamic_data_.pb->ref_counter_ > 1) {
        init_dynamic(dynamic_data_.capacity_);
    }
}

void number_storage::clr() {
    if (sz_.is_big_ == 1) {
        --dynamic_data_.pb->ref_counter_;
        if (dynamic_data_.pb->ref_counter_ == 0) {
            delete dynamic_data_.pb;
        }
    }
}

// Public methods


number_storage::number_storage(size_t size, number_t val) : sz_(size, 0) {
    if (size <= MAX_STATIC_SIZE) {
        std::fill(static_data_, static_data_ + sz_.size_, val);
    } else {
        sz_.is_big_ = 1;
        dynamic_data_.capacity_ = size;
        dynamic_data_.pb = allocate_buffer(size);
        std::fill(dynamic_data_.pb->data_, dynamic_data_.pb->data_ + sz_.size_, val);
    }
}

number_storage::number_storage(size_t size) : number_storage(size, 0)
{}

number_storage::number_storage(number_storage const& other) : sz_(other.sz_) {
    if (other.size() <= MAX_STATIC_SIZE) {
        sz_.is_big_ = 0;
        std::copy(other.begin(), other.end(), static_data_);
    } else {
        sz_.is_big_ = 1;
        dynamic_data_.capacity_ = other.dynamic_data_.capacity_;
        dynamic_data_.pb = other.dynamic_data_.pb;
        ++dynamic_data_.pb->ref_counter_;
    }
}

number_storage& number_storage::operator=(number_storage const& other) {
    if (this != & other) {
        number_storage tmp(other);
        swap(tmp);
    }
    return *this;
}

number_storage::~number_storage() {
    clr();
}

number_storage::number_t& number_storage::operator[](size_t i) {
    if (sz_.is_big_ == 1) {
        separate();
        return dynamic_data_.pb->data_[i];
    }
    return static_data_[i];
}

number_storage::number_t const& number_storage::operator[](size_t i) const {
    return sz_.is_big_ == 1 ?  dynamic_data_.pb->data_[i] : static_data_[i];
}

typename number_storage::iterator number_storage::begin() {
    if (sz_.is_big_ == 1) {
        separate();
        return dynamic_data_.pb->data_;
    }
    return static_data_;
}

typename number_storage::iterator number_storage::end() {
    if (sz_.is_big_ == 1) {
        separate();
        return dynamic_data_.pb->data_ + sz_.size_;
    }
    return static_data_ + sz_.size_;
}

typename number_storage::const_iterator number_storage::begin() const {
    return sz_.is_big_ == 1 ? dynamic_data_.pb->data_ : static_data_;
}

typename number_storage::const_iterator number_storage::end() const {
    return (sz_.is_big_ == 1 ? dynamic_data_.pb->data_ : static_data_) + sz_.size_;
}

size_t number_storage::size() const {
    return sz_.size_;
}

void number_storage::resize(size_t size, number_t val) {
    if ((sz_.is_big_ == 1 && size > dynamic_data_.capacity_) || (sz_.is_big_ == 0 && size > MAX_STATIC_SIZE)) {
        init_dynamic(size);
    }
    if (sz_.is_big_ == 1) {
        std::fill(dynamic_data_.pb->data_ + sz_.size_, dynamic_data_.pb->data_ + size, val);
    } else {
        std::fill(static_data_ + sz_.size_, static_data_ + size, val);
    }
    sz_.size_ = size;
}

void number_storage::resize(size_t size) {
    resize(size, 0);
}

number_storage::number_t& number_storage::back() {
    if (sz_.is_big_ == 1) {
        separate();
    }
    return sz_.is_big_ == 1 ? dynamic_data_.pb->data_[sz_.size_ - 1] : static_data_[sz_.size_ - 1];
}

number_storage::number_t const& number_storage::back() const {
    return sz_.is_big_ == 1 ? dynamic_data_.pb->data_[sz_.size_ - 1] : static_data_[sz_.size_ - 1];
}

void number_storage::push_back(number_t const& val) {
    ++sz_.size_;
    if (sz_.is_big_ != 1 && sz_.size_ <= MAX_STATIC_SIZE) {
        static_data_[sz_.size_ - 1] = val;
    } else {
        if (sz_.is_big_ == 1 && dynamic_data_.capacity_ >= sz_.size_) {
            separate();
        } else {
            init_dynamic(2 * sz_.size_);
        }
        dynamic_data_.pb->data_[sz_.size_ - 1] = val;
    }
}

void number_storage::pop_back() {
    --sz_.size_;
}

bool number_storage::empty() const {
    return sz_.size_ == 0;
}

void number_storage::swap(number_storage& other) {
    if (sz_.is_big_ == 1) {
        if (other.sz_.is_big_ == 1) {
            std::swap(dynamic_data_, other.dynamic_data_);
        } else {
            flexible_data tmp = dynamic_data_;
            std::copy(other.static_data_, other.static_data_ + MAX_STATIC_SIZE, static_data_);
            other.dynamic_data_ = tmp;
        }
    } else {
        if (other.sz_.is_big_ == 1) {
            flexible_data tmp = other.dynamic_data_;
            std::copy(static_data_, static_data_ + MAX_STATIC_SIZE, other.static_data_);
            dynamic_data_ = tmp;
        } else {
            std::swap_ranges(static_data_, static_data_ + MAX_STATIC_SIZE, other.static_data_);
        }
    }
    std::swap(sz_, other.sz_);
}
