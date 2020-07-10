#include "number_storage.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>


// Private methods

void number_storage::init_dynamic() {
    container tmp = static_data_size_ == FLAG ? dynamic_data_->first : container(static_data_, static_data_ + static_data_size_);
    clr();
    dynamic_data_ = new type_dd(container(tmp), 1);
    static_data_size_ = FLAG;
}

void number_storage::separate() {
    if (dynamic_data_->second != 1) {
        init_dynamic();
    }
}

void number_storage::into_dynamic() {
    if (static_data_size_ != FLAG) {
        init_dynamic();
    }
}

void number_storage::clr() {
    if (static_data_size_ == FLAG) {
        if (dynamic_data_->second == 1) {
            delete dynamic_data_;
        } else {
            --dynamic_data_->second;
        }
    }
}

// Public methods

number_storage::number_storage() {
    static_data_size_ = 0;
}

number_storage::number_storage(size_t size) {
    if (size <= MAX_STATIC_SIZE) {
        for (size_t i = 0; i != MAX_STATIC_SIZE; ++i) {
            static_data_[i] = 0;
        }
        static_data_size_ = size;
    } else {
        dynamic_data_ = new type_dd(container(size), 1);
        static_data_size_ = FLAG;
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
        static_data_size_ = FLAG;
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
    clr();
}

typename number_storage::number_type& number_storage::operator[](size_t i) {
    if (static_data_size_ == FLAG) {
        separate();
        return dynamic_data_->first[i];
    }
    return static_data_[i];
}

typename number_storage::number_type const& number_storage::operator[](size_t i) const {
    return static_data_size_ == FLAG ?  dynamic_data_->first[i] : static_data_[i];
}

typename number_storage::iterator number_storage::begin() {
    if (static_data_size_ == FLAG) {
        separate();
        return dynamic_data_->first.data();
    }
    return static_data_;
}

typename number_storage::iterator number_storage::end() {
    if (static_data_size_ == FLAG) {
        separate();
        return dynamic_data_->first.data() + dynamic_data_->first.size();
    }
    return static_data_ + static_data_size_;
}

typename number_storage::const_iterator number_storage::begin() const {
    return static_data_size_ == FLAG ? dynamic_data_->first.data() : static_data_;
}

typename number_storage::const_iterator number_storage::end() const {
    return static_data_size_ == FLAG ? dynamic_data_->first.data() + dynamic_data_->first.size() : static_data_ + static_data_size_;
}

size_t number_storage::size() const {
    return static_data_size_ == FLAG ? dynamic_data_->first.size() : static_data_size_;
}

void number_storage::resize(size_t size) {
    if (static_data_size_ != FLAG) {
        if (size <= MAX_STATIC_SIZE) {
            static_data_size_ = size;
            for (size_t i = size; i != MAX_STATIC_SIZE; ++i) {
                static_data_[i] = 0;
            }
        } else {
            into_dynamic();
            dynamic_data_->first.resize(size);
        }
    }
}

typename number_storage::number_type& number_storage::back() {
    if (static_data_size_ == FLAG) {
        separate();
    }
    return static_data_size_ == FLAG ? dynamic_data_->first.back() : static_data_[static_data_size_ - 1];
}

typename number_storage::number_type const& number_storage::back() const {
    return static_data_size_ == FLAG ? dynamic_data_->first.back() : static_data_[static_data_size_ - 1];
}

void number_storage::push_back(uint32_t const& val) {
    if (static_data_size_ != FLAG && static_data_size_ <= MAX_STATIC_SIZE - 1) {
        static_data_[static_data_size_++] = val;
    } else {
        into_dynamic();
        separate();
        dynamic_data_->first.push_back(val);
    }
}

void number_storage::pop_back() {
    if (static_data_size_ != FLAG) {
        --static_data_size_;
    } else {
        separate();
        dynamic_data_->first.pop_back();
    }
}

bool number_storage::empty() const {
    return static_data_size_ == FLAG ? dynamic_data_->first.empty() : static_data_size_ == 0;
}

void number_storage::swap(number_storage& other) {
    if (static_data_size_ == FLAG) {
        if (other.static_data_size_ == FLAG) {
            std::swap(dynamic_data_, other.dynamic_data_);
        } else {
            type_dd *p = dynamic_data_;
            for (size_t i = 0; i != MAX_STATIC_SIZE; ++i) {
                static_data_[i] = other.static_data_[i];
            }
            other.dynamic_data_ = p;
            static_data_size_ = other.static_data_size_;
            other.static_data_size_ = FLAG;
        }
    } else {
        if (other.static_data_size_ == FLAG) {
            type_dd *p = other.dynamic_data_;
            for (size_t i = 0; i != MAX_STATIC_SIZE; ++i) {
                other.static_data_[i] = static_data_[i];
            }
            dynamic_data_ = p;
            other.static_data_size_ = static_data_size_;
            static_data_size_ = FLAG;
        } else {
            for (size_t i = 0; i != MAX_STATIC_SIZE; ++i) {
                std::swap(static_data_[i], other.static_data_[i]);
            }
            std::swap(static_data_size_, other.static_data_size_);
        }
    }
}
