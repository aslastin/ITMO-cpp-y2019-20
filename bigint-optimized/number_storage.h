#pragma once

#include <vector>
#include <cstdint>
#include <utility>


namespace {
    constexpr uint8_t SHIFT = sizeof(size_t) * 8 - 1;

    struct buffer {
        size_t ref_counter_;
        uint32_t data_[];
    };

    struct flexible_data {
        size_t capacity_ : SHIFT;
        buffer* pb;
    };

    struct special_size {
        size_t size_ : SHIFT;
        size_t is_big_ : 1;
        special_size(size_t size, size_t is_big) : size_(size), is_big_(is_big)
        {}
        special_size() : special_size(0, 0)
        {}
    };
}

struct number_storage {
    using number_t = uint32_t;
    using big_number_t = uint64_t;
    using iterator = number_t *;
    using const_iterator = number_t const*;
    using buffer = buffer;

    number_storage() = default;
    number_storage(size_t size, number_t val);
    explicit number_storage(size_t size);
    number_storage(number_storage const& other);
    number_storage& operator=(number_storage const& other);

    ~number_storage();

    number_t& operator[](size_t i);
    number_t const& operator[](size_t i) const;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    size_t size() const;

    void resize(size_t size, number_t val);
    void resize(size_t size);

    number_t& back();
    number_t const& back() const;
    void push_back(number_t const&);
    void pop_back();

    bool empty() const;
    void swap(number_storage&);

 private:
    constexpr static size_t MAX_STATIC_SIZE = sizeof(flexible_data) / sizeof(number_t);
    special_size sz_;
    union {
        number_t static_data_[MAX_STATIC_SIZE];
        flexible_data dynamic_data_;
    };

    void init_dynamic(size_t size);
    void init_dynamic();
    void separate();
    void clr();
};

