#pragma once

#include <vector>
#include <cstdint>
#include <utility>

struct number_storage {
    using number_type = uint32_t;
    using iterator = number_type *;
    using const_iterator = number_type const*;

    number_storage();
    number_storage(size_t size);
    number_storage(number_storage const& other);
    number_storage& operator=(number_storage const& other);

    ~number_storage();

    number_type& operator[](size_t i);
    number_type const& operator[](size_t i) const;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    size_t size() const;
    void resize(size_t size);

    number_type& back();
    number_type const& back() const;
    void push_back(number_type const&);
    void pop_back();

    bool empty() const;
    void swap(number_storage&);

 private:
    using container = std::vector<number_type>;
    using type_dd = std::pair<container, size_t>;

    static constexpr size_t MAX_STATIC_SIZE = (sizeof(type_dd)) / sizeof(number_type);
    static constexpr uint8_t FLAG = 255;
    union {
        number_type static_data_[MAX_STATIC_SIZE];
        type_dd* dynamic_data_;
    };
    uint8_t static_data_size_;
    void init_dynamic();
    void separate();
    void into_dynamic();
    void clr();
};
