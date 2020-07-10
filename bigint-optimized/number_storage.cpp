#include "number_storage.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>


// Private methods

void number_storage::init_dynamic(const_iterator it, const_iterator it_) {
    container tmp(it_ - it);
    size_t i = 0;
    for (auto iter = it; iter != it_; ++iter) {
        tmp[i++] = *iter;
    }
    if (static_data_size_ == 255) {
        if (dynamic_data_->second == 1) {
            delete dynamic_data_;
        } else {
            --dynamic_data_->second;
        }
    }
    dynamic_data_ = new type_dd(container(tmp), 1);
    static_data_size_ = 255;
}

void number_storage::into_dynamic() {
    if (static_data_size_ != 255) {
        init_dynamic(begin(), end());
    }
}

// Public methods

number_storage::number_storage() {
    for (size_t i = 0; i != MAX_STATIC_SIZE; ++i) {
        static_data_[i] = 0;
    }
    static_data_size_ = 0;
}

number_storage::number_storage(size_t size) : number_storage() {
    if (size <= MAX_STATIC_SIZE) {
        static_data_size_ = size;
    } else {
        dynamic_data_ = new type_dd(container(size), 1);
        static_data_size_ = 255;
    }
}

number_storage::number_storage(number_storage const& other) {
    if (other.size() <= MAX_STATIC_SIZE) {
        for (size_t i = 0; i != other.size(); ++i) {
            static_data_[i] = other[i];
        }
        for (size_t i = other.size(); i != MAX_STATIC_SIZE; ++i) {
            static_data_[i] = 0;
        }
        static_data_size_ = other.size();
    } else {
        static_data_size_ = 255;
        dynamic_data_ = other.dynamic_data_;
        ++dynamic_data_->second;
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
    if (static_data_size_ == 255) {
        if (dynamic_data_->second == 1) {
            delete dynamic_data_;
        } else {
            --dynamic_data_->second;
        }
    }
}

typename number_storage::number_type& number_storage::operator[](size_t i) {
    if (static_data_size_ == 255 && dynamic_data_->second != 1) {
        init_dynamic(begin(), end());
    }
    return static_data_size_ == 255 ?  dynamic_data_->first[i] : static_data_[i];
}

typename number_storage::number_type const& number_storage::operator[](size_t i) const {
    return static_data_size_ == 255 ?  dynamic_data_->first[i] : static_data_[i];
}

typename number_storage::iterator number_storage::begin() {
    if (static_data_size_ == 255 && dynamic_data_->second != 1) {
        init_dynamic(&(*dynamic_data_->first.begin()), &(*dynamic_data_->first.end()));
    }
    return static_data_size_ == 255 ? &(*dynamic_data_->first.begin()) : static_data_;
}

typename number_storage::iterator number_storage::end() {
    if (static_data_size_ == 255 && dynamic_data_->second != 1) {
        init_dynamic(&(*dynamic_data_->first.begin()), &(*dynamic_data_->first.end()));
    }
    return static_data_size_ == 255 ? &(*dynamic_data_->first.end()) : static_data_ + static_data_size_;
}

typename number_storage::const_iterator number_storage::begin() const {
    return static_data_size_ == 255 ? &(*dynamic_data_->first.begin()) : static_data_;
}

typename number_storage::const_iterator number_storage::end() const {
    return static_data_size_ == 255 ? &(*dynamic_data_->first.end()) : static_data_ + static_data_size_;
}

size_t number_storage::size() const {
    return static_data_size_ == 255 ? dynamic_data_->first.size() : static_data_size_;
}

void number_storage::resize(size_t size) {
    if (size <= MAX_STATIC_SIZE && static_data_size_ != 255) {
        static_data_size_ = size;
        for (size_t i = size; i != MAX_STATIC_SIZE; ++i) {
            static_data_[i] = 0;
        }
    } else {
        init_dynamic(begin(), end());
        dynamic_data_->first.resize(size);
    }
}

typename number_storage::number_type& number_storage::back() {
    if (static_data_size_ == 255 && dynamic_data_->second != 1) {
        init_dynamic(begin(), end());
    }
    return static_data_size_ == 255 ? dynamic_data_->first.back() : static_data_[static_data_size_ - 1];
}

typename number_storage::number_type const& number_storage::back() const {
    return static_data_size_ == 255 ? dynamic_data_->first.back() : static_data_[static_data_size_ - 1];
}

void number_storage::push_back(uint32_t const& val) {
    if (static_data_size_ != 255 && static_data_size_ + 1 <= MAX_STATIC_SIZE) {
        static_data_[static_data_size_++] = val;
    } else {
        into_dynamic();
        if (dynamic_data_->second != 1) {
            init_dynamic(begin(), end());
        }
        dynamic_data_->first.push_back(val);
    }
}

void number_storage::pop_back() {
    if (static_data_size_ != 255) {
        static_data_[--static_data_size_] = 0;
    } else {
        if (dynamic_data_->second != 1) {
            init_dynamic(begin(), end());
        }
        dynamic_data_->first.pop_back();
    }
}

bool number_storage::empty() const {
    return static_data_size_ == 255 ? dynamic_data_->first.empty() : static_data_size_ == 0;
}

void number_storage::swap(number_storage& other) {
    if (static_data_size_ == 255) {
        if (other.static_data_size_ != 255) {
            other.into_dynamic();
        }
        std::swap(dynamic_data_, other.dynamic_data_);
    } else {
        if (other.static_data_size_ == 255) {
            into_dynamic();
            std::swap(dynamic_data_, other.dynamic_data_);
        } else {
            for (size_t i = 0; i != MAX_STATIC_SIZE; ++i) {
                std::swap(static_data_[i], other.static_data_[i]);
            }
            std::swap(static_data_size_, other.static_data_size_);
        }
    }
}