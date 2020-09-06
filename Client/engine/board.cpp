#include "board.h"

namespace chess {

// Attack directions for rooks/bishops.
static constexpr std::array<std::pair<int, int>, 4> R_DIR = {{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
static constexpr std::array<std::pair<int, int>, 4> B_DIR = {{{1, 1}, {-1, 1}, {1, -1}, {-1, -1}}};
static constexpr int K_DIR[] = { 7, 8, 9, 1, -7, -8, -9, -1 };
static constexpr int PROMOTIONS[] = { Move::Q_PROM, Move::R_PROM, Move::B_PROM, Move::N_PROM };

static Magic R_MAGIC_PARAMS[64];
static Magic B_MAGIC_PARAMS[64];
static Bitboard R_ATTACKS[102400];
static Bitboard B_ATTACKS[5248];

void init_magic() 
{
	Magic::fill_attacks_table(R_MAGIC_PARAMS, R_ATTACKS, R_MAGIC_NUM, R_DIR);
	Magic::fill_attacks_table(B_MAGIC_PARAMS, B_ATTACKS, B_MAGIC_NUM, B_DIR);
}

// Relevant occupancy squares for king.
static constexpr Bitboard K_ATTACKS[] = {
	0x0000000000000302ULL, 0x0000000000000705ULL, 0x0000000000000E0AULL, 0x0000000000001C14ULL, 
	0x0000000000003828ULL, 0x0000000000007050ULL, 0x000000000000E0A0ULL, 0x000000000000C040ULL, 
	0x0000000000030203ULL, 0x0000000000070507ULL, 0x00000000000E0A0EULL, 0x00000000001C141CULL,
	0x0000000000382838ULL, 0x0000000000705070ULL, 0x0000000000E0A0E0ULL, 0x0000000000C040C0ULL, 
	0x0000000003020300ULL, 0x0000000007050700ULL, 0x000000000E0A0E00ULL, 0x000000001C141C00ULL, 
	0x0000000038283800ULL, 0x0000000070507000ULL, 0x00000000E0A0E000ULL, 0x00000000C040C000ULL,
	0x0000000302030000ULL, 0x0000000705070000ULL, 0x0000000E0A0E0000ULL, 0x0000001C141C0000ULL, 
	0x0000003828380000ULL, 0x0000007050700000ULL, 0x000000E0A0E00000ULL, 0x000000C040C00000ULL, 
	0x0000030203000000ULL, 0x0000070507000000ULL, 0x00000E0A0E000000ULL, 0x00001C141C000000ULL,
	0x0000382838000000ULL, 0x0000705070000000ULL, 0x0000E0A0E0000000ULL, 0x0000C040C0000000ULL, 
	0x0003020300000000ULL, 0x0007050700000000ULL, 0x000E0A0E00000000ULL, 0x001C141C00000000ULL, 
	0x0038283800000000ULL, 0x0070507000000000ULL, 0x00E0A0E000000000ULL, 0x00C040C000000000ULL,
	0x0302030000000000ULL, 0x0705070000000000ULL, 0x0E0A0E0000000000ULL, 0x1C141C0000000000ULL, 
	0x3828380000000000ULL, 0x7050700000000000ULL, 0xE0A0E00000000000ULL, 0xC040C00000000000ULL, 
	0x0203000000000000ULL, 0x0507000000000000ULL, 0x0A0E000000000000ULL, 0x141C000000000000ULL,
	0x2838000000000000ULL, 0x5070000000000000ULL, 0xA0E0000000000000ULL, 0x40C0000000000000ULL };

// Relevant occupancy squares for knight.
static constexpr Bitboard N_ATTACKS[] = {
	0x0000000000020400ULL, 0x0000000000050800ULL, 0x00000000000A1100ULL, 0x0000000000142200ULL, 
	0x0000000000284400ULL, 0x0000000000508800ULL, 0x0000000000A01000ULL, 0x0000000000402000ULL, 
	0x0000000002040004ULL, 0x0000000005080008ULL, 0x000000000A110011ULL, 0x0000000014220022ULL,
	0x0000000028440044ULL, 0x0000000050880088ULL, 0x00000000A0100010ULL, 0x0000000040200020ULL, 
	0x0000000204000402ULL, 0x0000000508000805ULL, 0x0000000A1100110AULL, 0x0000001422002214ULL, 
	0x0000002844004428ULL, 0x0000005088008850ULL, 0x000000A0100010A0ULL, 0x0000004020002040ULL,
	0x0000020400040200ULL, 0x0000050800080500ULL, 0x00000A1100110A00ULL, 0x0000142200221400ULL, 
	0x0000284400442800ULL, 0x0000508800885000ULL, 0x0000A0100010A000ULL, 0x0000402000204000ULL, 
	0x0002040004020000ULL, 0x0005080008050000ULL, 0x000A1100110A0000ULL, 0x0014220022140000ULL,
	0x0028440044280000ULL, 0x0050880088500000ULL, 0x00A0100010A00000ULL, 0x0040200020400000ULL, 
	0x0204000402000000ULL, 0x0508000805000000ULL, 0x0A1100110A000000ULL, 0x1422002214000000ULL, 
	0x2844004428000000ULL, 0x5088008850000000ULL, 0xA0100010A0000000ULL, 0x4020002040000000ULL,
	0x0400040200000000ULL, 0x0800080500000000ULL, 0x1100110A00000000ULL, 0x2200221400000000ULL, 
	0x4400442800000000ULL, 0x8800885000000000ULL, 0x100010A000000000ULL, 0x2000204000000000ULL, 
	0x0004020000000000ULL, 0x0008050000000000ULL, 0x00110A0000000000ULL, 0x0022140000000000ULL,
	0x0044280000000000ULL, 0x0088500000000000ULL, 0x0010A00000000000ULL, 0x0020400000000000ULL };

// Relevant occupancy squares for pawns from both sides. 
static constexpr Bitboard P_ATTACKS[2][64] = { {
	0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000002ULL, 0x0000000000000005ULL, 0x000000000000000AULL, 0x0000000000000014ULL, 
    0x0000000000000028ULL, 0x0000000000000050ULL, 0x00000000000000A0ULL, 0x0000000000000040ULL,
    0x0000000000000200ULL, 0x0000000000000500ULL, 0x0000000000000A00ULL, 0x0000000000001400ULL, 
    0x0000000000002800ULL, 0x0000000000005000ULL, 0x000000000000A000ULL, 0x0000000000004000ULL, 
    0x0000000000020000ULL, 0x0000000000050000ULL, 0x00000000000A0000ULL, 0x0000000000140000ULL, 
    0x0000000000280000ULL, 0x0000000000500000ULL, 0x0000000000A00000ULL, 0x0000000000400000ULL, 
    0x0000000002000000ULL, 0x0000000005000000ULL, 0x000000000A000000ULL, 0x0000000014000000ULL, 
    0x0000000028000000ULL, 0x0000000050000000ULL, 0x00000000A0000000ULL, 0x0000000040000000ULL, 
    0x0000000200000000ULL, 0x0000000500000000ULL, 0x0000000A00000000ULL, 0x0000001400000000ULL, 
    0x0000002800000000ULL, 0x0000005000000000ULL, 0x000000A000000000ULL, 0x0000004000000000ULL, 
    0x0000020000000000ULL, 0x0000050000000000ULL, 0x00000A0000000000ULL, 0x0000140000000000ULL, 
    0x0000280000000000ULL, 0x0000500000000000ULL, 0x0000A00000000000ULL, 0x0000400000000000ULL,
    0x0002000000000000ULL, 0x0005000000000000ULL, 0x000A000000000000ULL, 0x0014000000000000ULL,
    0x0028000000000000ULL, 0x0050000000000000ULL, 0x00A0000000000000ULL, 0x0040000000000000ULL	},
{	
	0x0000000000000200ULL, 0x0000000000000500ULL, 0x0000000000000A00ULL, 0x0000000000001400ULL, 
	0x0000000000002800ULL, 0x0000000000005000ULL, 0x000000000000A000ULL, 0x0000000000004000ULL, 
	0x0000000000020000ULL, 0x0000000000050000ULL, 0x00000000000A0000ULL, 0x0000000000140000ULL,
	0x0000000000280000ULL, 0x0000000000500000ULL, 0x0000000000A00000ULL, 0x0000000000400000ULL, 
	0x0000000002000000ULL, 0x0000000005000000ULL, 0x000000000A000000ULL, 0x0000000014000000ULL, 
	0x0000000028000000ULL, 0x0000000050000000ULL, 0x00000000A0000000ULL, 0x0000000040000000ULL,
	0x0000000200000000ULL, 0x0000000500000000ULL, 0x0000000A00000000ULL, 0x0000001400000000ULL, 
	0x0000002800000000ULL, 0x0000005000000000ULL, 0x000000A000000000ULL, 0x0000004000000000ULL, 
	0x0000020000000000ULL, 0x0000050000000000ULL, 0x00000A0000000000ULL, 0x0000140000000000ULL,
	0x0000280000000000ULL, 0x0000500000000000ULL, 0x0000A00000000000ULL, 0x0000400000000000ULL, 
	0x0002000000000000ULL, 0x0005000000000000ULL, 0x000A000000000000ULL, 0x0014000000000000ULL, 
	0x0028000000000000ULL, 0x0050000000000000ULL, 0x00A0000000000000ULL, 0x0040000000000000ULL,
	0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 
	0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 
	0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
	0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL	}};

static constexpr Bitboard LIGHT_SQUARES = 0x55AA55AA55AA55AAULL;
static constexpr Bitboard DARK_SQUARES = 0xAA55AA55AA55AA55ULL;

static constexpr int CASTLE_PERM_FROM[64] = {
	0xD, 0xF, 0xF, 0xF, 0xC, 0xF, 0xF, 0xE,
	0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
	0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
	0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
	0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
	0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
	0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
	0x7, 0xF, 0xF, 0xF, 0x3, 0xF, 0xF, 0xB  };

static constexpr int CASTLE_PERM_TO[64] = {
	0xD, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xE,
	0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
	0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
	0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
	0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
	0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
	0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
	0x7, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xB  };

Board::Board(const Board &parent, Move move) : Board(parent) { make_move(move); }

std::string moves2str(const MoveList &list) 
{
	std::stringstream ss;
	ss << "MoveList:";
	for (auto move : list)
		ss << "\nmove: " << std::to_string(move) << "\t-\t" << move.str();
	ss << "\nMove List Total: " << std::to_string(list.size()) << '\n';
	return ss.str();
}

uint64_t perft(const int depth, Board &b) 
{
	if (!depth)
		return 1;

	uint64_t nodes = 0;
	const auto list = b.legal_actions();
	Board temp(b);

	for (const auto &move : list) {
		b.make_move(move);
		nodes += perft(depth-1, b);
		b = temp;
	}
	return nodes;
}

void perft_test(const int depth, Board &board) 
{
	Board temp(board);
	
	std::cout << board.str() << '\n';
	std::cout << "\nStarting test to depth: " << depth << '\n';
	
	uint64_t all_nodes = 0;
	uint64_t move_nodes = 0;
	
	auto start = std::chrono::steady_clock::now();
	const auto list = board.legal_actions();

	for (const auto &move : list) {
		board.make_move(move);
		move_nodes = perft(depth-1, board);
		all_nodes += move_nodes;
		board = temp;
		std::cout << move.str() << '\t' << move_nodes << '\n';
	}
	auto time = std::chrono::steady_clock::now() - start;
	std::cout << "\nTest completed: " << all_nodes <<  " nodes visited in " <<  time.count() / 1'000'000.0 << "ms" << std::endl;
}

bool Board::operator==(const Board &other) const 
{
	return  (pieces[WHITE] == other.pieces[WHITE]) &&
			(pieces[BLACK] == other.pieces[BLACK]) && 
			(rooks == other.rooks) &&
			(bishops == other.bishops) && 
			(pawns == other.pawns) &&
			(kings[0] == other.kings[0]) &&
			(kings[1] == other.kings[1]) &&
			(castle == other.castle) &&
			(side == other.side);
}

std::string Board::str() const 
{
	std::stringstream ss;
	ss << "\nGAME BOARD:\n\n";

	for (int r = RANK_8; r >= RANK_1; --r) {
		ss << RANK_FEN[r] << "   ";
		for (int f = FILE_A; f < FILE_NONE; ++f) {
			if (!pieces[WHITE].get(r, f) && !pieces[BLACK].get(r, f)) {
				ss << ". ";
				continue;
			}
			if (kings[WHITE] == r*8 + f) {
				ss << "K ";
				continue;
			}
			if (kings[BLACK] == r*8 + f) {
				ss << "k ";
				continue;
			}
			char c = ' ';
			if ((pawns & ENPS_MASK).get(r, f)) {
				c = 'p';
			} else if (bishops.get(r, f)) {
				if (rooks.get(r, f))
					c = 'q';
				else
					c = 'b';
			} else if (rooks.get(r, f)) {
				c = 'r';
			} else {
				c = 'n';
			}
			if (pieces[WHITE].get(r, f)) 
				c = std::toupper(c);
			ss << c << ' ';
		}
		ss << '\n';
	}
	ss << "\n   ";
	for (int file = FILE_A; file < FILE_NONE; ++file)
		ss << ' ' << FILE_FEN[file];

	ss	<< "\n\nside:\t" << SIDE_FEN[side] << "\nenPass:\t" << std::to_string((pawns & ~ENPS_MASK) % 8) + " file"
		<< "\ncastle:\t" << (castle & WKCA ? 'K' : '-') << (castle & WQCA ? 'Q' : '-') << (castle & BKCA ? 'k' : '-')
		<< (castle & BQCA ? 'q' : '-') << "\n50move:\t" << std::to_string(ply50) << "\nply:\t" << std::to_string(ply)
		<< '\n';
	return ss.str();
}

void Board::set_pos(const std::string &fen) 
{
	reset();
	
	int rank = RANK_8;
	int file = FILE_A;

	// Parsing fen string.
	std::vector<std::string> parsed;
	std::istringstream iss(fen);
	std::string str; 
	while (iss >> str)
		parsed.push_back(str);

	if (parsed.size() < 4)
		throw "Bad fen string: not enough info fields";

	// Parsing pieces.
	for (const char c : parsed[0]) {

		if (c == '/') {
			--rank;
			if (rank < 0)
				throw "Bad fen string: too few /";
			file = FILE_A;
			continue;
		}

		if (std::isdigit(c)) {
			file += c - '0';
			continue;
		}

		if (std::isupper(c))
			pieces[WHITE].set(rank, file);
		else
			pieces[BLACK].set(rank, file);

		switch (c) {
			case 'p':
			case 'P':
				pawns.set(rank, file);
				break;
			case 'n': 
			case 'N': 
				break;
			case 'b': 
			case 'B': 
				bishops.set(rank, file);
				break;
			case 'r':
			case 'R': 
				rooks.set(rank, file);
				break;
			case 'q':
			case 'Q': 
				bishops.set(rank, file);
				rooks.set(rank, file);
				break;
			case 'k': 
				kings[BLACK] = rank*8 + file;
				break;
			case 'K': 
				kings[WHITE] = rank*8 + file;
				break;
			default:
				throw "Bad fen string: unknown character";
				break;
		}
		++file;
	}
	// Side
	if (parsed[1][0] != 'w'  || parsed[1][0] == 'b')
		throw "Bad fen string: only 'w' and 'b' as turn";
	else 
		side = (parsed[1][0] == 'w') ? WHITE : BLACK;
	// Castling rights
	for (const char c : parsed[2]) {
		switch (c) {
			case 'K': castle |= WKCA; break;
			case 'Q': castle |= WQCA; break;
			case 'k': castle |= BKCA; break;
			case 'q': castle |= BQCA; break;
			case '-': break;
			default: throw "Bad fen string: unknown character in castling rights";
		}
	}
	// enPassant
	if (parsed[3] != "-") {
		if (parsed[3].size() != 2)
			throw "Bad fen string: unknown character in enPassant";
		file = parsed[3][0] - 'a';
		rank = parsed[3][1] - '1';
		if (file < FILE_A || file > FILE_H || (side == WHITE && rank != RANK_6) || (side == BLACK && rank != RANK_3))
			throw "Bad fen string: wrong enPassant rank";
		pawns.set(RANK_8, file);//enPass = rank*8 + file; // (rank == RANK_3) ? RANK_1 : 
	}
	// If ply info not provided.
	if (parsed.size() < 6) {
		ply = ply50 = 0;
		return;
	}
	// 50 moves rule
	int num = std::stoi(parsed[4]);
	if (num < 0 || num >= 50)
		throw "Bad fen string: inadequate number for 50 moves rule";
	else
		ply50 = num*2;
	// Ply from fullmoves
	num = std::stoi(parsed[5]);
	if (num < 1)
		throw "Bad fen string: moves cannot < 1";
	else
		ply = (num-1)*2 + side;
}

void Board::reset() 
{
	std::memset(reinterpret_cast<void*>(this), 0, sizeof(Board));
}

MoveList Board::pseudolegal_actions() const 
{
	MoveList list;
	list.reserve(100);

	// attacking side king square and pieces
	uint8_t king = kings[side];
	Bitboard attacker = pieces[side];

	// Iterating through attacking side pieces.
	for (auto src : attacker) {
		// King
		if (src == king) {
			for (const auto dst : K_ATTACKS[src] - attacker) 
				list.emplace_back(src, dst);
			// Castlings
			auto is_path_free = [this](int from, int to) 
			{
				while (from != to) {
					if (all().get(from))
						return false;
					++from;
				}
				return true;
			};
			auto is_path_not_attacked = [this](int from, int to) 
			{
				while (from != to) {
					if (attacked(from, !side))
						return false;
					++from;
				}
				return true;
			};
			const auto castle_rights = (side == WHITE) ? castle : castle >> 2; 
			// Checking king and queen sides if permission flag set.
			if (castle_rights & WKCA) {
				const auto k_rook = (side == WHITE) ? H1 : H8;
				if (is_path_free(king+1, k_rook) && is_path_not_attacked(king, king+2))
					list.emplace_back(king, king+2, Move::KCASTLE);
			}
			if (castle_rights & WQCA) {
				const auto q_rook = (side == WHITE) ? A1 : A8;
				if (is_path_free(q_rook+1, king) && is_path_not_attacked(king-1, king+1))
					list.emplace_back(king, king-2, Move::QCASTLE);
			}
			continue;
		}

		bool processed = false;
		// Rook / queen
		if (rooks.get(src)) {
			processed = true;
			Bitboard attacked = R_MAGIC_PARAMS[src].get_attacks(all()) - attacker;
			for (const auto dst : attacked)
				list.emplace_back(src, dst);
		}
		// Bishop / queen
		if (bishops.get(src)) {
			processed = true;
			Bitboard attacked = B_MAGIC_PARAMS[src].get_attacks(all()) - attacker;
			for (const auto dst : attacked)
				list.emplace_back(src, dst);
		}
		if (processed) 
			continue;

		// Pawns
		if ((pawns & ENPS_MASK).get(src)) {
			// Moves forward.
			int prom_r, push_r, d;
			if (side == WHITE) {
				prom_r = RANK_8;
				push_r = RANK_3;
				d = 1;
			} else {	
				prom_r = RANK_1;
				push_r = RANK_6;
				d = -1;
			}
			const auto dst_r = src / 8 + d;
			const auto dst_f = src % 8;
			const auto dst = rf2sq(dst_r, dst_f);
			if (free(dst)) {
				if (dst_r != prom_r) {
					list.emplace_back(src, dst);
					// Maybe it'll be possible to move two squares.
					if (dst_r == push_r && free(push_r + d, dst_f))
						list.emplace_back(src, rf2sq(push_r + d, dst_f), Move::PUSH);
				} else {
					// Promotions
					for (auto prom : PROMOTIONS)
						list.emplace_back(src, dst, prom);
				}
			}
			// Captures
			for (auto dir : {-1, 1}) {
				const int dst_f = src % 8 + dir;
				if (dst_f < 0 || dst_f >= 8) 
					continue;
				const auto dst = rf2sq(dst_r, dst_f);
				if (pieces[!side].get(dst)) {
					if (dst_r == prom_r) {
						// Promotions
						for (auto prom : PROMOTIONS)
							list.emplace_back(src, dst, prom);
					} else {
						// Ordinary capture
						list.emplace_back(src, dst);
					}
				} else if (dst_r == (7-push_r) && pawns.get(RANK_8, dst_f)) {
					// enPassant
					list.emplace_back(src, dst, Move::ENPASS);
				}
			}
			continue;
		}
		// Knight
		for (const auto dst : N_ATTACKS[src] - attacker)
			list.emplace_back(src, dst);
	}
	return list;
}

MoveList Board::legal_actions() const 
{
	MoveList list = pseudolegal_actions();
	list.erase(std::remove_if(	list.begin(), 
								list.end(), 
								[&](Move m) { return !legal(m); }), list.end());
	return list;
}

void Board::make_move(const Move move) 
{
	const auto from = move.from();
	const auto to = move.to();
	const auto flag = move.flags();

	// Remove en passant square, update castling rules and plies.
	++ply50;
	++ply;
	pawns &= ENPS_MASK;
	castle &= CASTLE_PERM_FROM[from];
	castle &= CASTLE_PERM_TO[to];

	auto do_castling = [this](int rook_src, int rook_dst) 
	{
		pieces[side].clr(rook_src);
		rooks.clr(rook_src);
		pieces[side].set(rook_dst);
		rooks.set(rook_dst);
	};
	// Special moves already discovered during generation.
	switch (flag) {
		default:
			break;
		case Move::KCASTLE:
			if (side == WHITE)
				do_castling(H1, F1);
			else
				do_castling(H8, F8);
			break;
		case Move::QCASTLE:
			if (side == WHITE)
				do_castling(A1, D1);
			else
				do_castling(A8, D8);
			break;
		case Move::PUSH:
			pawns.set(RANK_8, to % 8);
			break;
		case Move::ENPASS:
			const auto trg = (side == WHITE) ? to-8 : to+8;
			pawns.clr(trg);
			pieces[!side].clr(trg);
			break;
	}
	// Remove captured piece.
	if (pieces[!side].get(to)) {
		ply50 = 0;
		pieces[!side].clr(to);
		rooks.clr(to);
		bishops.clr(to);
		pawns.clr(to);
	}
	// Move in our pieces.
	pieces[side].set(to);
	pieces[side].clr(from);

	side = !side;

	// If king moves. !side because we just changed
	if (from == kings[!side]) {
		kings[!side] = to;
		return;
	}
	// If pawn was moved, reset 50 move draw counter.
	if (pawns.get(from))
		ply50 = 0;

	// Promotion
	if (flag & Move::N_PROM) {
		switch (flag) {
			case Move::B_PROM:
				bishops.set(to);
				break;
			case Move::R_PROM:
				rooks.set(to);
				break;
			case Move::Q_PROM:
				rooks.set(to);
				bishops.set(to);
				break;
			default:;
		}
		pawns.clr(from);
		return;
	}
	// Update Bitboards during regular move.
	rooks |= static_cast<uint64_t>(rooks.get(from)) << to;
	bishops |= static_cast<uint64_t>(bishops.get(from)) << to;
	pawns |= static_cast<uint64_t>(pawns.get(from)) << to;

	// Move from other Bitboards.
	rooks.clr(from);
	bishops.clr(from);
	pawns.clr(from);
}

bool Board::attacked(const uint8_t sq, const bool att_clr) const 
{
	Bitboard they = pieces[att_clr];
	Bitboard king = (1ULL << kings[att_clr]);

	// Check rooks and queens.
	if (R_MAGIC_PARAMS[sq].get_attacks(all()) & they & rooks)
		return true;
	// Check bishops and queens.
	if (B_MAGIC_PARAMS[sq].get_attacks(all()) & they & bishops)
		return true;
	// Check pawns.
	if (P_ATTACKS[att_clr][sq] & they  & pawns)
		return true;
	// Check knights.
	if (N_ATTACKS[sq] & (they - king - rooks - bishops - (pawns & ENPS_MASK)))
		return true;
	// Check king.
	if (K_ATTACKS[sq] & king)
		return true;

	return false;
}

bool Board::legal(Move move) const 
{
	Board board = *this;
	board.make_move(move);
	return !board.attacked(board.kings[side], !side);
}

bool Board::enough_material() const 
{
	if (!rooks.empty() || !pawns.empty())
		return true;

	if (all().count() < 4)
		return false;

	if (!(get_knights().empty()))
		return true;

	const bool light_bishop = bishops & LIGHT_SQUARES;
	const bool dark_bishop = bishops & DARK_SQUARES;

	return light_bishop && dark_bishop;
}

}