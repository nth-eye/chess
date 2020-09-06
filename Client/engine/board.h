#include <iostream>
#include <cstring>
#include <chrono>
#include <sstream>

#include "move.h"
#include "magic.h"

#pragma once

namespace chess {

constexpr auto START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr auto SIDE_FEN = "wb-";
constexpr auto RANK_FEN = "12345678";
constexpr auto FILE_FEN = "abcdefgh";
constexpr Bitboard ENPS_MASK = 0x00FFFFFFFFFFFF00ULL;
constexpr Move NO_MOVE = Move();

struct Board {

	enum {	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE	};
	enum {	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE	};
	enum {	WHITE, BLACK, BOTH	};
	enum {	WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8	};
	enum {  A1, B1, C1, D1, E1, F1, G1, H1, 
			A2, B2, C2, D2, E2, F2, G2, H2, 
			A3, B3, C3, D3, E3, F3, G3, H3, 
			A4, B4, C4, D4, E4, F4, G4, H4, 
			A5, B5, C5, D5, E5, F5, G5, H5, 
			A6, B6, C6, D6, E6, F6, G6, H6,
			A7, B7, C7, D7, E7, F7, G7, H7,
			A8, B8, C8, D8, E8, F8, G8, H8	};

	Board() = default;
	Board(const Board &parent, Move move);
	Board(const std::string &fen) { set_pos(fen); }

	std::string str() const;
	bool operator==(const Board &other) const;
	bool operator!=(const Board &other) const { return !operator==(other); }

	void set_pos(const std::string &fen);
	void reset();
	void make_move(const Move move);
	MoveList pseudolegal_actions() const;
	MoveList legal_actions() const;

	bool attacked(const uint8_t sq, const bool clr) const;
	bool check() const { return attacked(kings[side], !side); }
	bool legal(Move move) const;
	bool enough_material() const;

    Bitboard all() const { return pieces[0] | pieces[1]; }
    Bitboard get_whites() const { return pieces[WHITE] | Bitboard(kings[WHITE]); }
    Bitboard get_blacks() const { return pieces[BLACK] | Bitboard(kings[BLACK]); }
	Bitboard get_pawns() const { return pawns & ENPS_MASK; }
	Bitboard get_rooks() const { return rooks - bishops; }
	Bitboard get_bishops() const { return bishops - rooks; }
	Bitboard get_queens() const { return rooks & bishops; }
    Bitboard get_knights() const { return (all() - rooks - bishops - pawns - kings[0]) - kings[1]; }
    Bitboard get_kings() const { return (1ULL << kings[0]) | (1ULL << kings[1]); }
	uint16_t get_ply() const { return ply; }
	uint8_t get_ply50() const { return ply50; }
	uint8_t get_enpass() const { return pawns - ENPS_MASK; }
	uint8_t get_castle() const { return castle; }
	bool turn() const { return side; }
	bool free(const int sq) const { return !all().get(sq); }
	bool free(const int r, const int f) const { return free(rf2sq(r, f)); }

private:
	Bitboard pieces[2]; // Pieces separated by color according WHITE / BLACK enum.
	Bitboard rooks; 	// Rooks and queens.
	Bitboard bishops;	// Bishops and queens.
	Bitboard pawns;

	uint8_t kings[2];   // Squares of kings according to WHITE / BLACK enum.

	uint16_t ply = 0;
	uint8_t ply50 = 0;
	uint8_t castle = 0;
	bool side = 0;
};

uint64_t perft(const int depth, Board &b);
void perft_test(const int depth, Board &board);
std::string moves2str(const MoveList &list);
void init_magic();

}
