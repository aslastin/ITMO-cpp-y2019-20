#include "number_storage.h"

#include <memory>
#include <algorithm>


// Helpful functions

namespace {
    flexible_data* allocate_buffer(size_t size) {
        auto* buf = static_cast<flexible_data*>(operator new (sizeof(flexible_data) + sizeof(number_storage::number_t) * size));
        buf->ref_counter = 1;
        buf->capacity = size;
        return buf;
    }
}

// Private methods

void number_storage::init_unique_dynamic(size_t capacity) {
    flexible_data* buf = allocate_buffer(capacity);
    if (sz.is_big) {
        std::copy(dynamic_data->data, dynamic_data->data + sz.size, buf->data);
    } else {
        std::copy(static_data, static_data + sz.size, buf->data);
    }
    clr();
    sz.is_big = true;
    dynamic_data = buf;
}

void number_storage::separate() {
    if (sz.is_big && dynamic_data->ref_counter > 1) {
        init_unique_dynamic(dynamic_data->capacity);
    }
}

void number_storage::clr() {
    if (sz.is_big) {
        --dynamic_data->ref_counter;
        if (dynamic_data->ref_counter == 0) {
            delete dynamic_data;
        }
    }
}

// Public methods

number_storage::number_storage(size_t size, number_t val) : sz(size, size > MAX_STATIC_SIZE) {
    if (size <= MAX_STATIC_SIZE) {
        std::fill(static_data, static_data + sz.size, val);
    } else {
        dynamic_data = allocate_buffer(size);
        std::fill(dynamic_data->data, dynamic_data->data + sz.size, val);
    }
}

number_storage::number_storage(number_storage const& other) : sz(other.sz) {
    if (other.size() <= MAX_STATIC_SIZE) {
        sz.is_big = false;
        std::copy(other.begin(), other.end(), static_data);
    } else {
        sz.is_big = true;
        dynamic_data = other.dynamic_data;
        ++dynamic_data->ref_counter;
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
    if (sz.is_big) {
        separate();
        return dynamic_data->data[i];
    }
    return static_data[i];
}

number_storage::number_t const& number_storage::operator[](size_t i) const {
    return sz.is_big ?  dynamic_data->data[i] : static_data[i];
}

number_storage::iterator number_storage::begin() {
    if (sz.is_big) {
        separate();
        return dynamic_data->data;
    }
    return static_data;
}

number_storage::iterator number_storage::end() {
    if (sz.is_big) {
        separate();
        return dynamic_data->data + sz.size;
    }
    return static_data + sz.size;
}

number_storage::const_iterator number_storage::begin() const {
    return sz.is_big ? dynamic_data->data : static_data;
}

number_storage::const_iterator number_storage::end() const {
    return sz.is_big ? dynamic_data->data + sz.size : static_data + sz.size;
}

size_t number_storage::size() const {
    return sz.size;
}

void number_storage::resize(size_t size, number_t val) {
    if ((sz.is_big && size > dynamic_data->capacity) || (!sz.is_big && size > MAX_STATIC_SIZE)) {
        init_unique_dynamic(INCREASE_CAPACITY * size);
    }
    if (size > sz.size) {
        if (sz.is_big) {
            std::fill(dynamic_data->data + sz.size, dynamic_data->data + size, val);
        } else {
            std::fill(static_data + sz.size, static_data + size, val);
        }
    }
    sz.size = size;
}

number_storage::number_t& number_storage::back() {
    if (sz.is_big) {
        separate();
    }
    return sz.is_big ? dynamic_data->data[sz.size - 1] : static_data[sz.size - 1];
}

number_storage::number_t const& number_storage::back() const {
    return sz.is_big ? dynamic_data->data[sz.size - 1] : static_data[sz.size - 1];
}

void number_storage::push_back(number_t const& val) {
    if (!sz.is_big && sz.size + 1 <= MAX_STATIC_SIZE) {
        static_data[sz.size] = val;
    } else {
        if (sz.is_big && dynamic_data->capacity >= sz.size + 1) {
            separate();
        } else {
            init_unique_dynamic(INCREASE_CAPACITY * (sz.size + 1));
        }
        dynamic_data->data[sz.size] = val;
    }
    ++sz.size;
}

void number_storage::pop_back() {
    --sz.size;
}

bool number_storage::empty() const {
    return sz.size == 0;
}

void number_storage::swap(number_storage& other) {
    if (sz.is_big) {
        if (other.sz.is_big) {
            std::swap(dynamic_data, other.dynamic_data);
        } else {
            flexible_data* tmp = dynamic_data;
            std::copy(other.static_data, other.static_data + MAX_STATIC_SIZE, static_data);
            other.dynamic_data = tmp;
        }
    } else {
        if (other.sz.is_big) {
            flexible_data* tmp = other.dynamic_data;
            std::copy(static_data, static_data + MAX_STATIC_SIZE, other.static_data);
            dynamic_data = tmp;
        } else {
            std::swap_ranges(static_data, static_data + MAX_STATIC_SIZE, other.static_data);
        }
    }
    std::swap(sz, other.sz);
}
