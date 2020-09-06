#include <cstdint>
#include <cassert>
#include <string>
#include <vector>

#include "misc.h"

#pragma once

namespace chess {

struct Move {

	enum Masks : uint16_t {	TO = 0b0000000000111111, FROM = 0b0000111111000000, FLAGS = 0b1111000000000000   };
	enum Flags : uint16_t { 
		QUIET   = 0b0000'0000'0000'0000, 
		PUSH    = 0b0001'0000'0000'0000,
		KCASTLE = 0b0010'0000'0000'0000,
		QCASTLE = 0b0011'0000'0000'0000,
		CAPTURE = 0b0100'0000'0000'0000,
		ENPASS  = 0b0101'0000'0000'0000,
		N_PROM  = 0b1000'0000'0000'0000,
		B_PROM  = 0b1001'0000'0000'0000,
		R_PROM  = 0b1010'0000'0000'0000,
		Q_PROM  = 0b1011'0000'0000'0000	};

    Move() = default;
    Move(uint16_t move_) : move(move_) {}
	Move(uint16_t from, uint16_t to) : move(to + (from << 6)) {}
	Move(uint16_t from, uint16_t to, uint16_t flags) : move(to + (from << 6) + flags) {}
	Move(const char *str) : Move(std::string(str)) {}
	Move(const std::string &str) 
	{
		if (str.size() < 4) 
			throw "Bad move: " + str;

		set_from(str[0]-'a' + (str[1]-'1') * 8);
		set_to(str[2]-'a' + (str[3]-'1') * 8);

		if (str.size() != 4) {
			if (str.size() != 5) 
				throw "Bad move: " + str;
			switch (str[4]) {
				case 'q':
				case 'Q': set_flags(Q_PROM); break;
				case 'r':
				case 'R': set_flags(R_PROM); break;
				case 'b':
				case 'B': set_flags(B_PROM); break;
				case 'n':
				case 'N': set_flags(N_PROM); break;
				default: throw "Bad move: " + str;
			}
		}
	}

	bool operator==(Move &rhs) const { return move == rhs.move; }
	operator uint16_t() const { return move; }
	std::string str() const 
	{
		std::string str = sq2str(from()) + sq2str(to()); 
		switch (flags()) {
			case Q_PROM: return str + 'q';
			case R_PROM: return str + 'r';
			case B_PROM: return str + 'b';
			case N_PROM: return str + 'n';
			default: return str;
		}
	};

	uint8_t to() const { return move & TO; }
	uint8_t from() const { return (move & FROM) >> 6; }
	uint16_t flags() const { return move & FLAGS; }
	void set_to(uint8_t to) { move = (move & ~TO) | to; }
	void set_from(uint8_t from) { move = (move & ~FROM) | (from << 6); }
	void set_flags(uint16_t flags) { move = (move & ~FLAGS) | flags; }

	void mirror() { move ^= 0b111000111000; }

private:
	uint16_t move = 0;
};

using MoveList = std::vector<Move>;

}
