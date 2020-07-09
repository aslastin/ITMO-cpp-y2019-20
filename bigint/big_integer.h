#pragma once

#include <cstddef>
#include <iosfwd>
#include <cstdint>
#include <vector>

class big_integer
{
    typedef std::vector<uint32_t>::iterator iterator;
    typedef std::vector<u_int32_t>::const_iterator const_iterator;

    std::vector<uint32_t> val_;
    bool sign_;
    static uint64_t const BASE = static_cast<uint64_t>(UINT32_MAX) + 1;

    // Helpful Methods

    bool is_zero() const;
    static void clear_back(big_integer& num);

    static void summator_pos(uint32_t& val1, uint32_t val2, uint64_t& carry);
    static void summator_neg(uint32_t& val1, uint32_t val2, uint64_t& borrow);

    // Methods for compare :

    int cmp_no_sign(big_integer const& rhs) const;
    int cmp(big_integer const& rhs) const;

    // Methods for arithmetic operations :

    static void add_long_short(big_integer& lng, uint32_t shrt);

    static uint32_t mul_long_short(big_integer const& lng, uint32_t shrt, big_integer& res);

    static uint32_t div_long_short(big_integer const& lng, uint32_t shrt, big_integer& res);

    static uint32_t sum_long_long(iterator it1_begin, iterator it1_end, const_iterator it2_begin, const_iterator it2_end,
                                  void (*const summator)(uint32_t&, uint32_t, uint64_t&));

    static void mul_long_long(big_integer& lng1, big_integer const& lng2);

    // Methods for long division

    static std::pair<big_integer, big_integer> div_long_long(big_integer& lng1, big_integer const& lng2);

    static uint32_t trial(big_integer const& lng1, big_integer const& lng2, uint32_t shift);

    static bool smaller(big_integer const& lng1, big_integer const& lng2, uint32_t shift);

    static void difference(big_integer& lng1, big_integer const& lng2, uint32_t shift);





    // Methods for bit operations :

    static void into_two_complement(big_integer& rhs);

    static uint32_t AND(uint32_t arg1, uint32_t arg2) { return arg1 & arg2; }
    static uint32_t OR(uint32_t arg1, uint32_t arg2) { return arg1 | arg2; }
    static uint32_t XOR(uint32_t arg1, uint32_t arg2) { return arg1 ^ arg2; }

    void apply_bit_op(big_integer const & rhs, uint32_t (*const bit_op)(uint32_t, uint32_t));

public:
    big_integer();
    big_integer(big_integer const& other);
    big_integer(int a);
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

