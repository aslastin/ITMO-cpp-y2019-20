#pragma once

#include <cstdint>
#include <utility>

constexpr uint8_t SHIFT = sizeof(size_t) * 8 - 1;

struct flexible_buffer {
    size_t ref_counter;
    uint32_t data[];
};

// Специально уменьшаю capacity до 63 бит, чтобы сохранялся инвариант: capacity >= size.

struct flexible_data {
    size_t capacity : SHIFT;
    flexible_buffer* pb;
};

struct special_size {
    size_t size : SHIFT;
    size_t is_big : 1;
    special_size(size_t size, size_t is_big) : size(size), is_big(is_big)
    {}
    special_size() : special_size(0, 0)
    {}
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
    constexpr static size_t MAX_STATIC_SIZE = sizeof(flexible_data) / sizeof(number_t);
    constexpr static size_t INCREASE_CAPACITY = 2;

    // Выделил size и is_big специально в отдельный класс по причине проблем при копировании bitfields.
    // (в интернете более элегантного способа не нашел)
    special_size sz;
    union {
        number_t static_data[MAX_STATIC_SIZE];
        flexible_data dynamic_data;
    };

    void init_dynamic(size_t size);
    void separate();
    void clr();
};


