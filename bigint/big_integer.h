#pragma once

#include <cstddef>
#include <iosfwd>
#include <cstdint>
#include <vector>
#include <functional>

class big_integer
{
    typedef std::vector<uint32_t>::iterator iterator;
    typedef std::vector<uint32_t>::const_iterator const_iterator;

    std::vector<uint32_t> val_;
    bool sign_;
    static uint64_t const BASE = static_cast<uint64_t>(UINT32_MAX) + 1;

    bool is_zero() const;

    // Delete useless zeros in the end of the number
    void clear_back();

    // Functions given to sum_long_long :

    // val1' = val1 + val2 + carry
    static void summator_pos(uint32_t& val1, uint32_t val2, uint64_t& carry);
    // val1' = val1 - val2 - carry
    static void summator_neg(uint32_t& val1, uint32_t val2, uint64_t& borrow);

    // Compare :

    int cmp_no_sign(big_integer const& rhs) const;
    int cmp(big_integer const& rhs) const;

    // Arithmetic operations (don't pay attention to the sign of the arguments):

    // lng' = lng + shrt
    static void add_long_short(big_integer& lng, uint32_t shrt);

    // res = lng * shrt
    static uint32_t mul_long_short(big_integer const& lng, uint32_t shrt, big_integer& res);

    // lng' = lng / shrt
    static uint32_t div_long_short(big_integer& lng, uint32_t shrt);

    // Apply the summator to the two transmitted sequences.
    // Precondition : it1_ - it1 >= it_2 - it2
    static uint32_t sum_long_long(iterator it1, iterator it1_, const_iterator it2, const_iterator it2_,
                                  void (*const summator)(uint32_t&, uint32_t, uint64_t&));

    // lng1' = lng1 * lng2
    static void mul_long_long(big_integer& lng1, big_integer const& lng2);

    // Methods for long division

    // lng1' = lng1 / lng2
    // Precondition : lng1 >= lng2
    static std::pair<big_integer, big_integer> div_long_long(big_integer& lng1, big_integer const& lng2);

    static uint32_t trial(big_integer const& lng1, big_integer const& lng2, uint32_t shift);

    static bool smaller(big_integer const& lng1, big_integer const& lng2, uint32_t shift);

    // Methods for bit operations :

    void into_two_complement();

    // (*this)' = bit_op(*this, rhs)
    void apply_bit_op(big_integer const & rhs, std::function<uint32_t(uint32_t, uint32_t)> const& bit_op);

public:
    big_integer();
    big_integer(big_integer const& other);
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

