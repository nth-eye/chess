#include <string>
#include <bit>

#pragma once

namespace chess {

template <typename T>
struct BitIterator {

	constexpr BitIterator(uint64_t value_) : value(value_) {};
	constexpr bool operator!=(const BitIterator &other) { return value != other.value; }
	constexpr void operator++() { value &= (value - 1); }
	constexpr T operator*() const { return std::countr_zero(value); }

private:
	uint64_t value;
};

// A1 = 0; H1 = 7; A8 = 56; H8 = 63
struct Bitboard {

	constexpr Bitboard() = default;
	constexpr Bitboard(uint64_t bb_) : bb(bb_) {}

	constexpr operator uint64_t() const { return bb; }
	std::string str() const 
	{
		std::string str;
		for (int rank = 7; rank >= 0; --rank) {
			str += '|';
			for (int file = 0; file < 8; ++file) {
				if (get(rank, file))
					str += 'X';
				else
					str += '-';
				str += '|';
			}
			str += '\n';
		}
		return str;
	}
	constexpr bool operator==(const Bitboard &other) const { return other.bb == bb; }
	constexpr bool operator!=(const Bitboard &other) const { return other.bb != bb; }
	friend Bitboard operator|(const Bitboard &lhs, const Bitboard &rhs) { return lhs.bb | rhs.bb; }

	friend Bitboard operator&(const Bitboard &lhs, const Bitboard &rhs) { return lhs.bb & rhs.bb; }
	friend Bitboard operator-(const Bitboard &lhs, const Bitboard &rhs) { return lhs.bb & ~rhs.bb; }
    friend Bitboard operator-(const Bitboard &lhs, const uint8_t &rhs) { return lhs.bb & ~(1ULL << rhs);}
	Bitboard operator~() const { return ~bb; }
	Bitboard& operator&=(const Bitboard &other) 
	{
		bb &= other.bb; 
		return *this;
	}
	Bitboard& operator|=(const Bitboard &other) 
	{
		bb |= other.bb; 
		return *this;
	}
	constexpr BitIterator<uint64_t> begin() const { return bb; }
	constexpr BitIterator<uint64_t> end() const { return 0; }

	constexpr int count() const { return std::popcount(bb); }
	constexpr void clear() { bb = 0; }
	constexpr void set(const uint8_t pos) { bb |= (1UL << pos); }
	constexpr void clr(const uint8_t pos) { bb &= ~(1UL << pos); }
	constexpr bool get(const uint8_t pos) const { return bb & (1UL << pos); }
	constexpr void set(const int r, const int f) { set(r*8 + f); }
	constexpr void clr(const int r, const int f) { clr(r*8 + f); }
	constexpr bool get(const int r, const int f) const { return get(r*8 + f); }
	constexpr bool empty() const { return !bb; }

private:
	uint64_t bb = 0;
};

}
