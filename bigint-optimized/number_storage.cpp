#include "number_storage.h"

#include <memory>
#include <algorithm>


// Helpful functions

static flexible_buffer* allocate_buffer(size_t size) {
    auto buf = reinterpret_cast<flexible_buffer*>(operator new (sizeof(flexible_buffer) + sizeof(number_storage::number_t) * size));
    buf->ref_counter = 1;
    return buf;
}

// Private methods

void number_storage::init_dynamic(size_t capacity) {
    flexible_buffer* buf = allocate_buffer(capacity);
    if (sz.is_big == 1) {
        std::copy(dynamic_data.pb->data, dynamic_data.pb->data + sz.size, buf->data);
    } else {
        std::copy(static_data, static_data + sz.size, buf->data);
    }
    clr();
    sz.is_big = 1;
    dynamic_data.pb = buf;
    dynamic_data.capacity = capacity;
}

void number_storage::separate() {
    if (sz.is_big == 1 && dynamic_data.pb->ref_counter > 1) {
        init_dynamic(dynamic_data.capacity);
    }
}

void number_storage::clr() {
    if (sz.is_big == 1) {
        --dynamic_data.pb->ref_counter;
        if (dynamic_data.pb->ref_counter == 0) {
            delete dynamic_data.pb;
        }
    }
}

// Public methods

number_storage::number_storage(size_t size, number_t val) : sz(size, 0) {
    if (size <= MAX_STATIC_SIZE) {
        std::fill(static_data, static_data + sz.size, val);
    } else {
        sz.is_big = 1;
        dynamic_data.capacity = size;
        dynamic_data.pb = allocate_buffer(size);
        std::fill(dynamic_data.pb->data, dynamic_data.pb->data + sz.size, val);
    }
}

number_storage::number_storage(number_storage const& other) : sz(other.sz) {
    if (other.size() <= MAX_STATIC_SIZE) {
        sz.is_big = 0;
        std::copy(other.begin(), other.end(), static_data);
    } else {
        sz.is_big = 1;
        dynamic_data.capacity = other.dynamic_data.capacity;
        dynamic_data.pb = other.dynamic_data.pb;
        ++dynamic_data.pb->ref_counter;
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
    if (sz.is_big == 1) {
        separate();
        return dynamic_data.pb->data[i];
    }
    return static_data[i];
}

number_storage::number_t const& number_storage::operator[](size_t i) const {
    return sz.is_big == 1 ?  dynamic_data.pb->data[i] : static_data[i];
}

typename number_storage::iterator number_storage::begin() {
    if (sz.is_big == 1) {
        separate();
        return dynamic_data.pb->data;
    }
    return static_data;
}

typename number_storage::iterator number_storage::end() {
    if (sz.is_big == 1) {
        separate();
        return dynamic_data.pb->data + sz.size;
    }
    return static_data + sz.size;
}

typename number_storage::const_iterator number_storage::begin() const {
    return sz.is_big == 1 ? dynamic_data.pb->data : static_data;
}

typename number_storage::const_iterator number_storage::end() const {
    return sz.is_big == 1 ? dynamic_data.pb->data + sz.size : static_data + sz.size;
}

size_t number_storage::size() const {
    return sz.size;
}

void number_storage::resize(size_t size, number_t val) {
    if ((sz.is_big == 1 && size > dynamic_data.capacity) || (sz.is_big == 0 && size > MAX_STATIC_SIZE)) {
        init_dynamic(INCREASE_CAPACITY * size);
    }
    if (size > sz.size) {
        if (sz.is_big == 1) {
            std::fill(dynamic_data.pb->data + sz.size, dynamic_data.pb->data + size, val);
        } else {
            std::fill(static_data + sz.size, static_data + size, val);
        }
    }
    sz.size = size;
}

number_storage::number_t& number_storage::back() {
    if (sz.is_big == 1) {
        separate();
    }
    return sz.is_big == 1 ? dynamic_data.pb->data[sz.size - 1] : static_data[sz.size - 1];
}

number_storage::number_t const& number_storage::back() const {
    return sz.is_big == 1 ? dynamic_data.pb->data[sz.size - 1] : static_data[sz.size - 1];
}

void number_storage::push_back(number_t const& val) {
    if (sz.is_big != 1 && sz.size + 1 <= MAX_STATIC_SIZE) {
        static_data[sz.size] = val;
    } else {
        if (sz.is_big == 1 && dynamic_data.capacity >= sz.size + 1) {
            separate();
        } else {
            init_dynamic(INCREASE_CAPACITY * (sz.size + 1));
        }
        dynamic_data.pb->data[sz.size] = val;
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
    if (sz.is_big == 1) {
        if (other.sz.is_big == 1) {
            std::swap(dynamic_data, other.dynamic_data);
        } else {
            flexible_data tmp = dynamic_data;
            std::copy(other.static_data, other.static_data + MAX_STATIC_SIZE, static_data);
            other.dynamic_data = tmp;
        }
    } else {
        if (other.sz.is_big == 1) {
            flexible_data tmp = other.dynamic_data;
            std::copy(static_data, static_data + MAX_STATIC_SIZE, other.static_data);
            dynamic_data = tmp;
        } else {
            std::swap_ranges(static_data, static_data + MAX_STATIC_SIZE, other.static_data);
        }
    }
    std::swap(sz, other.sz);
}
