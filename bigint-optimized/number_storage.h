#pragma once

#include <cstdint>
#include <utility>
#include <cstddef>


struct special_size {
    constexpr static uint8_t SIZE_BITS = sizeof(size_t) * 8 - 1;
    size_t size : SIZE_BITS;
    bool is_big : 1;
    special_size(size_t size, bool is_big) : size(size), is_big(is_big)
    {}
    special_size() : special_size(0, false)
    {}
};

// Специально уменьшаю capacity до 63 бит, чтобы сохранялся инвариант: capacity >= size.

struct flexible_data {
    size_t capacity : special_size::SIZE_BITS;
    size_t ref_counter;
    uint32_t data[];
};


struct number_storage {
    using number_t = uint32_t;
    using big_number_t = uint64_t;
    using iterator = number_t *;
    using const_iterator = number_t const*;

    number_storage() = default;
    explicit number_storage(size_t size, number_t val = 0);
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

    // Если size' > size : i = size ... size' - 1 : elements[i] = val
    void resize(size_t size, number_t val = 0);

    number_t& back();
    number_t const& back() const;
    void push_back(number_t const&);
    void pop_back();

    bool empty() const;
    void swap(number_storage&);

 private:
    constexpr static uint8_t MAX_STATIC_SIZE = sizeof(flexible_data*) / sizeof(number_t);
    constexpr static uint8_t INCREASE_CAPACITY = 2;

    special_size sz;
    union {
        number_t static_data[MAX_STATIC_SIZE];
        flexible_data* dynamic_data;
    };

    void init_unique_dynamic(size_t size);
    void separate();
    void clr();
};



