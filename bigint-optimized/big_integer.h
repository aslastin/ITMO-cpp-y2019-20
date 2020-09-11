#pragma once

#include <cstddef>
#include <iosfwd>
#include <cstdint>
#include <vector>
#include <functional>
#include "number_storage.h"

class big_integer {
    using iterator = number_storage::iterator;
    using const_iterator = number_storage::const_iterator;
    using number_t = number_storage::number_t;
    using big_number_t = number_storage::big_number_t;

    constexpr static number_t NUMBER_MAX = UINT32_MAX;
    constexpr static big_number_t BASE = static_cast<big_number_t>(NUMBER_MAX) + 1;

    number_storage val_;
    bool sign_;


    bool is_zero() const;
    // Delete useless zeros in the end of the number
    void clear_back();

    // Functions given to sum_long_long :

    // val1' = val1 + val2 + carry
    static void summator_pos(number_t& val1, number_t val2, big_number_t& carry);
    // val1' = val1 - val2 - carry
    static void summator_neg(number_t& val1, number_t val2, big_number_t& borrow);

    // Compare :

    int cmp_no_sign(big_integer const& rhs) const;
    int cmp(big_integer const& rhs) const;

    // Arithmetic operations (don't pay attention to the sign of the arguments):

    // lng' = lng + shrt
    static void add_long_short(big_integer& lng, number_t shrt);

    // res = lng * shrt
    static number_t mul_long_short(big_integer const& lng, number_t shrt, big_integer& res);

    // lng' = lng / shrt
    static number_t div_long_short(big_integer& lng, number_t shrt);

    // Apply the summator to the two transmitted sequences.
    // Precondition : it1_ - it1 >= it_2 - it2
    static number_t sum_long_long(iterator it1, iterator it1_, const_iterator it2, const_iterator it2_,
                                  void (*const summator)(number_t&, number_t, uint64_t&));

    // lng1' = lng1 * lng2
    static void mul_long_long(big_integer& lng1, big_integer const& lng2);

    // Methods for long division

    // lng1' = lng1 / lng2
    // Precondition : lng1 >= lng2
    static std::pair<big_integer, big_integer> div_long_long(big_integer& lng1, big_integer const& lng2);

    static uint32_t trial(big_integer const& lng1, big_integer const& lng2, number_t shift);

    static bool smaller(big_integer const& lng1, big_integer const& lng2, number_t shift);

    // Methods for bit operations :

    void into_two_complement();

    // (*this)' = bit_op(*this, rhs)
    void apply_bit_op(big_integer const & rhs, std::function<number_t(number_t, number_t)> const& bit_op);

public:
    big_integer();
    big_integer(big_integer const& other) = default;
    big_integer(int a);
    big_integer(int a, size_t size);
    explicit big_integer(std::string const& str);

    void swap(big_integer& num);

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

std::ostream& operator<<(std::ostream& s, big_integer const& a);


