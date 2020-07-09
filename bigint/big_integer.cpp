#include "big_integer.h"

#include <stdexcept>
#include <string>
#include <cstdint>
#include <utility>
#include <list>
#include <algorithm>
#include <iostream>

// Private Methods

bool big_integer::is_zero() const {
    return val_.size() == 1 && val_.back() == 0;
}

void big_integer::clear_back(big_integer& num) {
    while (num.val_.size() > 1 && num.val_.back() == 0) {
        num.val_.pop_back();
    }
    if (num.val_.size() == 1 && num.val_.back() == 0) {
        num.sign_ = false;
    }
}

void big_integer::summator_pos(uint32_t& val1, uint32_t val2, uint64_t& carry) {
    uint64_t tmp = carry + val1 + val2;
    val1 = static_cast<uint32_t>(tmp % BASE);
    carry = tmp / BASE;
}

void big_integer::summator_neg(uint32_t& val1, uint32_t val2, uint64_t& borrow) {
    uint64_t tmp = BASE - borrow + val1 - val2;
    val1 = static_cast<uint32_t>(tmp % BASE);
    borrow = 1 - tmp / BASE;
}

int big_integer::cmp_no_sign(big_integer const &rhs) const {
    if (val_.size() == rhs.val_.size()) {
        size_t i = val_.size();
        while (i != 0) {
            --i;
            if (val_[i] > rhs.val_[i]) {
                return 1;
            }
            if (val_[i] < rhs.val_[i]) {
                return -1;
            }
        }
        return 0;
    }
    if (val_.size() > rhs.val_.size()) {
        return 1;
    } else {
        return -1;
    }
}

int big_integer::cmp(big_integer const& rhs) const {
    if (sign_) {
        if (rhs.sign_) {
            return -cmp_no_sign(rhs);
        }
        return -1;
    } else {
        if (rhs.sign_)
        {
            return 1;
        }
        return cmp_no_sign(rhs);
    }
}

void big_integer::add_long_short(big_integer& lng, uint32_t shrt) {
    for (size_t i = 0; i != lng.val_.size() && shrt != 0; ++i) {
        uint32_t acc = lng.val_[i];
        lng.val_[i] = acc + shrt;
        shrt = acc > UINT32_MAX - shrt;
    }
    if (shrt > 0) {
        lng.val_.push_back(shrt);
    }
}

uint32_t big_integer::mul_long_short(big_integer const& lng, uint32_t shrt, big_integer& res) {
    uint64_t remainder = 0;
    for (size_t i = 0; i != lng.val_.size(); ++i) {
        uint64_t tmp = static_cast<uint64_t>(lng.val_[i]) * shrt + remainder;
        res.val_[i] = static_cast<uint32_t>(tmp % BASE);
        remainder = tmp / BASE;
    }
    return static_cast<uint32_t>(remainder);
}

uint32_t big_integer::div_long_short(big_integer const& lng, uint32_t shrt, big_integer& res) {
    uint64_t carry = 0;
    for (size_t i = lng.val_.size(); i != 0; ) {
        uint64_t tmp = carry * BASE + lng.val_[--i];
        res.val_[i] = static_cast<uint32_t>(tmp / shrt);
        carry = tmp % shrt;
    }
    return static_cast<uint32_t>(carry);
}

uint32_t big_integer::sum_long_long(iterator it1, iterator it1_, const_iterator it2, const_iterator it2_,
                                    void (*const summator)(uint32_t&, uint32_t, uint64_t&)){
    uint64_t acc = 0;
    for (; it1 != it1_ && it2 != it2_; ++it1, ++it2) {
        summator(*it1, *it2, acc);
    }
    for (; it1 != it1_ && acc != 0; ++it1) {
        summator(*it1, 0, acc);
    }
    return static_cast<uint32_t>(acc);
}

void big_integer::mul_long_long(big_integer& lng1, big_integer const& lng2) {
    if (lng1.is_zero() || lng2.is_zero()) {
        lng1 = big_integer();
        return;
    }
    big_integer res;
    res.val_.resize(lng1.val_.size() + lng2.val_.size());
    for (size_t i = 0; i < lng1.val_.size(); ++i) {
        for (size_t j = 0, carry = 0; j < lng2.val_.size() || carry; ++j) {
            uint64_t cur = res.val_[i + j] + static_cast<uint64_t>(lng1.val_[i]) * (j < lng2.val_.size() ? lng2.val_[j] : 0) + carry;
            res.val_[i + j] = static_cast<uint32_t>(cur % BASE);
            carry = cur / BASE;
        }
    }
    clear_back(res);
    res.swap(lng1);
}

std::pair<big_integer, big_integer> big_integer::div_long_long(big_integer& lng1, big_integer const& lng2) {
    std::pair<big_integer, big_integer> p;
    p.first.val_.resize(lng1.val_.size() - lng2.val_.size() + 1);
    auto f = static_cast<uint32_t>(BASE / (static_cast<uint64_t>(lng2.val_.back()) + 1));

    big_integer r;
    r.val_.resize(lng1.val_.size());
    r.val_.push_back(mul_long_short(lng1, f, r));

    big_integer d;
    d.val_.resize(lng2.val_.size());
    mul_long_short(lng2, f, d);

    big_integer dq;
    dq.val_.resize(lng2.val_.size());

    ptrdiff_t i = lng1.val_.size() - lng2.val_.size();
    while (i >= 0) {
        uint32_t qt = trial(r, d, i);
        dq.val_.push_back(mul_long_short(d, qt, dq));
        if (smaller(r, dq, i)) {
            --qt;
            dq.val_.pop_back();
            dq.val_.push_back(mul_long_short(d, qt, dq));
        }
        p.first.val_[i] = qt;
        difference(r, dq, i);
        dq.val_.pop_back();
        --i;
    }
    div_long_short(r, f, r);
    p.second.swap(r);
    clear_back(p.first);
    clear_back(p.second);
    return p;
}

uint32_t big_integer::trial(big_integer const& lng1, big_integer const& lng2, uint32_t shift) {
    size_t i = shift + lng2.val_.size();
    __uint128_t r0 = lng1.val_[i - 2];
    __uint128_t r1 = lng1.val_[i - 1];
    __uint128_t r2 = lng1.val_[i];
    __uint128_t r3 = (r2 << 64) + (r1 << 32) + r0;

    __uint128_t d0 = lng2.val_[lng2.val_.size() - 2];
    __uint128_t d1 = lng2.val_[lng2.val_.size() - 1];
    __uint128_t d2 = (d1 << 32) + d0;

    r3 /= d2;
    __uint128_t val = UINT32_MAX;
    if (r3 >= val) {
        return UINT32_MAX;
    }
    return static_cast<uint32_t>(r3);
}

bool big_integer::smaller(big_integer const& lng1, big_integer const& lng2, uint32_t shift) {
    size_t i = lng2.val_.size() - 1;
    while (i != 0) {
        if (lng1.val_[i + shift] != lng2.val_[i]) {
            break;
        }
        --i;
    }
    return lng1.val_[i + shift] < lng2.val_[i];
}

void big_integer::difference(big_integer& lng1, big_integer const& lng2, uint32_t shift) {
    uint32_t borrow = 0;
    for (size_t i = 0; i != lng2.val_.size(); ++i) {
        uint64_t tmp = BASE + lng1.val_[i + shift] - lng2.val_[i] - borrow;
        lng1.val_[i + shift] = static_cast<uint32_t>(tmp % BASE);
        borrow = 1 - static_cast<uint32_t>(tmp / BASE);
    }
}

void big_integer::into_two_complement(big_integer & num) {
    if (num.sign_) {
        for (size_t i = 0; i != num.val_.size(); ++i) {
            num.val_[i] = ~num.val_[i];
        }
        add_long_short(num, 1);
        clear_back(num);
    }
}


void big_integer::apply_bit_op(big_integer const & rhs, uint32_t (*const bit_op)(uint32_t, uint32_t)) {
    into_two_complement(*this);
    big_integer cp = rhs;
    into_two_complement(cp);
    size_t end = std::min(val_.size(), rhs.val_.size());

    for (size_t i = 0; i != end; ++i) {
        val_[i] = bit_op(val_[i], cp.val_[i]);
    }
    if (val_.size() < cp.val_.size()) {
        uint32_t bit = sign_ ? UINT32_MAX : 0;
        for (size_t i = end; i != cp.val_.size(); ++i) {
            val_.push_back(bit_op(cp.val_[i], bit));
        }
    }
    if (val_.size() > cp.val_.size()) {
        uint32_t bit = cp.sign_ ? UINT32_MAX : 0;
        for (size_t i = end; i != val_.size(); ++i) {
            val_[i]  = bit_op(val_[i], bit);
        }
    }
    if (bit_op(sign_, cp.sign_) == 1) {
        sign_ = true;
        into_two_complement(*this);
    } else {
        sign_ = false;
        clear_back(*this);
    }
}


// Public Methods

big_integer::big_integer()
        : val_(1, 0), sign_(false)
{}

big_integer::big_integer(big_integer const& other) {
    val_ = other.val_;
    sign_ = other.sign_;
}

big_integer::big_integer(int a) : big_integer() {
    sign_ = a < 0;
    auto val = static_cast<int64_t>(a);
    if (val < 0) {
        val = -val;
    }
    val_[0] = static_cast<uint32_t>(val);
}

big_integer::big_integer(std::string const& str) : big_integer() {
    size_t i = (str.front() == '-') ?  1 : 0;
    for (; i != str.size(); ++i) {
        if (str[i] < '0' || str[i] > '9') {
            std::string message = "Invalid character : ";
            message += str[i];
            throw std::runtime_error(message);
        }
        uint32_t remainder = mul_long_short(*this, 10, *this);
        if (remainder > 0) {
            val_.push_back(remainder);
        }
        add_long_short(*this, str[i] - 48);
    }
    if (str.front() == '-' && !is_zero()) {
        sign_ = true;
    }
}

void big_integer::swap(big_integer& num) {
    std::swap(sign_, num.sign_);
    val_.swap(num.val_);
}

big_integer& big_integer::operator=(big_integer const& other) {
    if (this != & other) {
        val_ = other.val_;
        sign_ = other.sign_;
    }
    return *this;
}

big_integer& big_integer::operator+=(big_integer const& rhs) {
    uint32_t remainder;
    if (cmp_no_sign(rhs) >= 0) {
        remainder = sum_long_long(val_.begin(),val_.end(), rhs.val_.begin(), rhs.val_.end(),
                                  sign_ == rhs.sign_ ? summator_pos : summator_neg);
    } else {
        big_integer num = rhs;
        remainder = sum_long_long(num.val_.begin(), num.val_.end(),
                                  val_.begin(), val_.end(),
                                  sign_ == rhs.sign_ ? summator_pos : summator_neg);
        num.swap(*this);
    }
    if (remainder > 0) {
        val_.push_back(remainder);
    }
    clear_back(*this);
    return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
    uint32_t remainder;
    if (cmp_no_sign(rhs) >= 0) {
        remainder = sum_long_long(val_.begin(),val_.end(), rhs.val_.begin(), rhs.val_.end(),
                                  sign_ != rhs.sign_ ? summator_pos : summator_neg);
    } else {
        big_integer num = rhs;
        remainder = sum_long_long(num.val_.begin(), num.val_.end(),val_.begin(), val_.end(),
                                  sign_ != rhs.sign_ ? summator_pos : summator_neg);
        num.swap(*this);
        sign_ = !sign_;
    }
    if (remainder > 0) {
        val_.push_back(remainder);
    }
    clear_back(*this);
    return *this;
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
    bool sign = sign_ != rhs.sign_;
    mul_long_long(*this, rhs);
    this->sign_ = is_zero() ? false : sign;
    return *this;
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
    bool sign = (!sign_ && rhs.sign_) || (sign_ && !rhs.sign_);
    if (rhs.val_.size() == 1) {
        div_long_short(*this, rhs.val_.back(), *this);
        clear_back(*this);
        sign_ = sign;
        return *this;
    }
    if (val_.size() < rhs.val_.size()) {
        big_integer tmp;
        this->swap(tmp);
        return *this;
    }
    auto p = div_long_long(*this, rhs);
    p.first.swap(*this);
    sign_ = is_zero() ? false : sign;
    return *this;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
    if (rhs.val_.size() == 1) {
        big_integer tmp = div_long_short(*this, rhs.val_.back(), *this);
        val_.swap(tmp.val_);
        sign_ = is_zero() ? false : sign_;
        return *this;
    }
    if (val_.size() < rhs.val_.size()) {
        return *this;
    }
    auto p = div_long_long(*this, rhs);
    val_.swap(p.second.val_);
    return *this;
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
    apply_bit_op(rhs, AND);
    return *this;
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
    apply_bit_op(rhs, OR);
    return *this;
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
    apply_bit_op(rhs, XOR);
    return *this;
}

big_integer& big_integer::operator<<=(int rhs) {
    if (!is_zero()) {
        size_t pos1 = val_.size();
        val_.push_back(0);
        for (size_t i = 0; i < rhs / 32; ++i) {
            val_.push_back(0);
        }
        uint32_t shift = rhs % 32;
        size_t pos2 = val_.size() - 1;
        while (pos1 != 0) {
            uint32_t MASK1 = val_[pos1] << shift;
            uint32_t MASK2 = val_[pos1 - 1] >> (32 - shift);
            val_[pos2] = (MASK1 | MASK2);
            --pos1;
            --pos2;
        }
        val_[pos2] = val_[pos1] << shift;
        while (pos2 != 0) {
            val_[--pos2] = 0;
        }
        clear_back(*this);
    }
    return *this;
}

big_integer& big_integer::operator>>=(int rhs) {
    if (!is_zero()) {
        size_t pos1 = 0;
        uint32_t shift = rhs % 32;
        size_t pos2 = rhs / 32;
        while (pos2 != val_.size() - 1) {
            uint32_t MASK2 = val_[pos2 + 1] << (32 - shift);
            uint32_t MASK1 = val_[pos2] >> shift;
            val_[pos1] = (MASK1 | MASK2);
            ++pos1;
            ++pos2;
        }
        val_[pos1] = val_[pos2] >> shift;
        while (pos1 != val_.size() - 1) {
            val_[++pos1] = 0;
        }
        if (sign_) {
            add_long_short(*this, 1);
        }
        clear_back(*this);
    }
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer r = big_integer(*this);
    r.sign_ = is_zero() ? false : !r.sign_;
    return r;
}

big_integer big_integer::operator~() const {
    big_integer r = big_integer(*this);
    into_two_complement(r);
    for (size_t i = 0; i != r.val_.size(); ++i) {
        r.val_[i] = ~r.val_[i];
    }
    r.sign_ = !r.sign_;
    into_two_complement(r);
    return r;
}

big_integer& big_integer::operator++() {
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer& big_integer::operator--() {
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const& b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const& b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const& a, big_integer const& b) {
    return a.cmp(b) == 0;
}

bool operator!=(big_integer const& a, big_integer const& b) {
    return a.cmp(b) != 0;
}

bool operator<(big_integer const& a, big_integer const& b) {
    return a.cmp(b) < 0;
}

bool operator>(big_integer const& a, big_integer const& b)
{
    return a.cmp(b) > 0;
}

bool operator<=(big_integer const& a, big_integer const& b)
{
    return a.cmp(b) <= 0;
}

bool operator>=(big_integer const& a, big_integer const& b)
{
    return a.cmp(b) >= 0;
}

std::string to_string(big_integer const& a) {
    if (a.is_zero()) {
        return "0";
    }
    std::list<char> l;
    big_integer tmp = a;
    while (!tmp.is_zero()) {
        uint32_t reminder = big_integer::div_long_short(tmp, 10, tmp);
        big_integer::clear_back(tmp);
        l.push_front(static_cast<char>(reminder + 48));
    }
    if (a.sign_) {
        l.push_front('-');
    }
    return std::string(l.begin(), l.end());
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
    return s << to_string(a);
}