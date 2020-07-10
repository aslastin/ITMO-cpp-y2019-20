#pragma once

#include <vector>
#include <cstdint>
#include <utility>


struct number_storage {
    typedef uint32_t number_type;
    typedef number_type* iterator;
    typedef number_type const* const_iterator;
    typedef std::vector<number_type> container;
    typedef std::pair<container, size_t> type_dd;

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
    void clear();
    void swap(number_storage&);

private:
    static size_t const MAX_STATIC_SIZE = (sizeof(container) + sizeof(size_t)) / sizeof(number_type);
    union {
        number_type static_data_[MAX_STATIC_SIZE];
        type_dd* dynamic_data_;
    };
    uint8_t static_data_size_;

    void init_dynamic(const_iterator it, const_iterator it_);
    void into_dynamic();
};