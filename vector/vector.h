#pragma once

#include <cstddef>
#include <utility>

template <typename T>
struct vector
{
    typedef T* iterator;
    typedef T const* const_iterator;

    vector();                               // O(1) nothrow
    vector(vector const&);                  // O(N) strong
    vector& operator=(vector const& other); // O(N) strong

    ~vector();                              // O(N) nothrow

    T& operator[](size_t i);                // O(1) nothrow
    T const& operator[](size_t i) const;    // O(1) nothrow

    T* data();                              // O(1) nothrow
    T const* data() const;                  // O(1) nothrow
    size_t size() const;                    // O(1) nothrow

    T& front();                             // O(1) nothrow
    T const& front() const;                 // O(1) nothrow

    T& back();                              // O(1) nothrow
    T const& back() const;                  // O(1) nothrow
    void push_back(T const&);               // O(1)* strong
    void pop_back();                        // O(1) nothrow

    bool empty() const;                     // O(1) nothrow

    size_t capacity() const;                // O(1) nothrow
    void reserve(size_t);                   // O(N) strong
    void shrink_to_fit();                   // O(N) strong

    void clear();                           // O(N) nothrow

    void swap(vector&);                     // O(1) nothrow

    iterator begin();                       // O(1) nothrow
    iterator end();                         // O(1) nothrow

    const_iterator begin() const;           // O(1) nothrow
    const_iterator end() const;             // O(1) nothrow

    iterator insert(iterator pos, T const&); // O(N) weak
    iterator insert(const_iterator pos, T const&); // O(N) weak

    iterator erase(iterator pos);           // O(N) weak
    iterator erase(const_iterator pos);     // O(N) weak

    iterator erase(iterator first, iterator last); // O(N) weak
    iterator erase(const_iterator first, const_iterator last); // O(N) weak

private:
    void destroy_all(T* data, size_t i);
    void copy_construct_all(T* dst, T const* src, size_t size);
    size_t get_increased_capacity() const;
    void new_buffer(size_t new_capacity);
    void push_back_realloc(T const&);
    void full_clear();
    void init_fields(T* data, size_t size, size_t capacity);

private:
    T* data_;
    size_t size_;
    size_t capacity_;
};

// Private methods

template <typename T>
void vector<T>::destroy_all(T* data, size_t i)
{
    for (; i != 0; --i)
    {
        data[i - 1].~T();
    }
}

template <typename T>
void vector<T>::copy_construct_all(T* dst, T const* src, size_t size)
{
    size_t i = 0;
    try
    {
        for (; i != size; ++i)
        {
            new (dst + i) T(src[i]);
        }
    }
    catch (...)
    {
        destroy_all(dst, i);
        throw;
    }
}


template <typename T>
size_t vector<T>::get_increased_capacity() const
{
    return capacity_ ? 2 * capacity_ : 1;
}

template <typename T>
void vector<T>::new_buffer(size_t new_capacity)
{
    T* data = static_cast<T*>(operator new (sizeof(T) * new_capacity));
    size_t size = size_;
    try
    {
        copy_construct_all(data, data_, size_);
    }
    catch (...)
    {
        operator delete(data);
        throw;
    }
    full_clear();
    init_fields(data, size, new_capacity);
}

template <typename T>
void vector<T>::push_back_realloc(const T & val)
{
    size_t capacity = get_increased_capacity();
    T* data = static_cast<T*>(operator new (sizeof(T) * capacity));
    size_t size = size_;
    try
    {
        copy_construct_all(data, data_, size_);
        try
        {
            new (data + size_) T(val);
        }
        catch (...)
        {
            destroy_all(data, size_);
            throw;
        }
    }
    catch (...)
    {
        operator delete (data);
        throw;
    }
    full_clear();
    init_fields(data, size + 1, capacity);
}

template <typename T>
void vector<T>::full_clear()
{
    clear();
    if (data_)
    {
        operator delete (data_);
    }
    data_ = nullptr;
    capacity_ = 0;
}

template <typename T>
void vector<T>::init_fields(T* data, size_t size, size_t capacity)
{
    data_ = data;
    size_ = size;
    capacity_ = capacity;
}

// Public methods

template <typename T>
vector<T>::vector() : data_(nullptr), size_(0), capacity_(0)
{}

template <typename T>
vector<T>::vector(vector<T> const& other) : vector()
{
    if (other.size_ > 0)
    {
        T* data = static_cast<T*>(operator new (sizeof(T) * other.size_));
        try
        {
            copy_construct_all(data, other.data(), other.size_);
        }
        catch (...)
        {
            operator delete(data);
            throw;
        }
        init_fields(data, other.size_, other.size_);
    }
}

template <typename T>
vector<T>& vector<T>::operator=(vector<T> const& other)
{
    if (this != &other)
    {
        vector copy = other;
        swap(copy);
    }
    return *this;
}

template <typename T>
vector<T>::~vector()
{
    full_clear();
}

template <typename T>
T& vector<T>::operator[](size_t i)
{
    return data_[i];
}

template <typename T>
T const& vector<T>::operator[](size_t i) const
{
    return data_[i];
}

template <typename T>
T* vector<T>::data()
{
    return data_;
}

template <typename T>
T const* vector<T>::data() const
{
    return data_;
}

template <typename T>
size_t vector<T>::size() const
{
    return size_;
}

template <typename T>
T& vector<T>::front()
{
    return *data_;
}

template <typename T>
T const& vector<T>::front() const
{
    return *data_;
}

template <typename T>
T& vector<T>::back()
{
    return data_[size_ - 1];
}

template <typename T>
T const& vector<T>::back() const
{
    return data_[size_ - 1];
}

template <typename T>
void vector<T>::push_back(T const& val)
{
    if (size_ == capacity_)
    {
        push_back_realloc(val);
    }
    else
    {
        new (data_ + size_) T(val);
        ++size_;
    }
}

template <typename T>
void vector<T>::pop_back()
{
    data_[--size_].~T();
}

template <typename T>
bool vector<T>::empty() const
{
    return size_ == 0;
}

template <typename T>
size_t vector<T>::capacity() const
{
    return capacity_;
}

template <typename T>
void vector<T>::reserve(size_t new_capacity)
{
    if (new_capacity > capacity_)
    {
        new_buffer(new_capacity);
    }
}

template <typename T>
void vector<T>::shrink_to_fit()
{
    if (capacity_ > size_)
    {
        if (size_ == 0)
        {
            full_clear();
        }
        else
        {
            new_buffer(size_);
        }
    }
}

template <typename T>
void vector<T>::clear()
{
    if (size_)
    {
        destroy_all(data_, size_);
        size_ = 0;
    }
}

template <typename T>
void vector<T>::swap(vector<T>& other)
{
    using std::swap;
    swap(data_, other.data_);
    swap(size_, other.size_);
    swap(capacity_, other.capacity_);
}

template <typename T>
typename vector<T>::iterator vector<T>::begin()
{
    return data_;
}

template <typename T>
typename vector<T>::iterator vector<T>::end()
{
    return data_ + size_;
}

template <typename T>
typename vector<T>::const_iterator vector<T>::begin() const
{
    return data_;
}

template <typename T>
typename vector<T>::const_iterator vector<T>::end() const
{
    return data_ + size_;
}

template <typename T>
typename vector<T>::iterator vector<T>::insert(iterator pos, T const& val)
{
    return insert(static_cast<const_iterator>(pos), val);
}

template <typename T>
typename vector<T>::iterator vector<T>::insert(const_iterator pos, T const& val)
{
    size_t at = pos - begin();
    push_back(val);
    size_t i = size_ - 1;
    while (i != at)
    {
        std::swap(data_[i - 1], data_[i]);
        --i;
    }
    return begin() + at;
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(iterator pos)
{
    return erase(static_cast<const_iterator>(pos));
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator pos)
{
    return erase(pos, pos + 1);
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(iterator first, iterator last)
{
    return erase(static_cast<const_iterator>(first), static_cast<const_iterator>(last));
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last)
{
    size_t pos_end = last - begin();
    size_t shift = first - begin();
    size_t dist = last - first;
    size_t pos_begin = shift;
    while (pos_begin != pos_end)
    {
        size_t i = pos_begin;
        while (i + dist < size_)
        {
            std::swap(data_[i], data_[i + dist]);
            i += dist;
        }
        ++pos_begin;
    }
    while (dist > 0)
    {
        pop_back();
        --dist;
    }
    return begin() + shift;
}