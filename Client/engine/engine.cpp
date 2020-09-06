#include "engine.h"

namespace chess {

Move Engine::search_position(SearchInfo *info)
{
    Move best_move = NO_MOVE;
    auto moves = legal_moves();
    int best_score = std::numeric_limits<int>::min()+1;

    int alpha = std::numeric_limits<int>::min()+1;
    int beta = std::numeric_limits<int>::max()-1;

    clear_search(info);

    for (auto move : moves) {

        int score = alpha_beta(Board(history.back().first, move), alpha, beta, info->depth, info, true);

        if (score > best_score) {
            best_move = move;
            best_score = score;
        }

        if (info->stopped)
            break;
    }
    return best_move;
}

int Engine::alpha_beta(Board board, int alpha, int beta, int depth, SearchInfo *info, bool doNull)
{
    auto list = legal_moves();

    info->nodes++;

    if (!depth)
        return evaluate(board);

    if (info->nodes & 65535)
        check_up(info);

    if (info->stopped)
        return 0;

    if ((is_n_repetition(1) || board.get_ply50() >= 100) && board.get_ply() > 0)
        return 0;

    int score = std::numeric_limits<int>::min()+1;

    for (size_t i = 0; i < list.size(); ++i) {

        Board tmp_board(board, list[i]);
        score = -alpha_beta(tmp_board, -beta, -alpha, depth-1, info, true);

        if (score >= beta)
            return beta;

        if (score > alpha)
            alpha = score;
    }
    return alpha;
}

void Engine::clear_search(SearchInfo *info)
{
    info->start_time = get_time();
    info->stopped = false;
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;
}

int Engine::evaluate(Board board)
{
    int score = board.get_whites().count() - board.get_blacks().count();

    if (board.turn() == WHITE)
        return score;
    return -score;
}

std::vector<std::tuple<int, int, int, int>> Engine::pieces_with_positions() const
{
    auto board = history.back().first;

    auto kings = board.get_kings();
    auto whites = board.get_whites();
    auto pawns = board.get_pawns();
    auto rooks = board.get_rooks();
    auto bishops = board.get_bishops();
    auto queens = board.get_queens();
    auto knigts = board.get_knights();

    int type = EMPTY;
    int color = BOTH;

    std::vector<std::tuple<int, int, int, int>> pieces;

    for (int r = Board::RANK_1; r < Board::RANK_NONE; ++r) {

        for (int f = Board::FILE_A; f < Board::FILE_NONE; ++f) {

            type = EMPTY;
            color = BOTH;

            if (whites.get(r, f))
                color = WHITE;
            else
                color = BLACK;

            if (pawns.get(r, f)) {
                type = P;
            } else if (bishops.get(r, f)) {
                type = B;
            } else if (rooks.get(r, f)) {
                type = R;
            } else if (queens.get(r, f)) {
                type = Q;
            } else if (knigts.get(r, f)) {
                type = N;
            } else if (kings.get(r, f))
                type = K;

            if (type != EMPTY)
                pieces.emplace_back(type, color, r, f);
        }
    }
    return pieces;
}

GameResult Engine::result() const  
{
	const auto board = history.back().first;

	if (board.legal_actions().empty()) {
		// Checkmate.
		if (board.check())
			return board.turn() == Board::BLACK ? GameResult::WHITE_WON : GameResult::BLACK_WON;
		// Stalemate.
		return GameResult::DRAW;
	}
	if (!board.enough_material() || board.get_ply50() >= 100 || history.back().second >= 2 /*|| board.getPly() >= 450*/) 
		return GameResult::DRAW;

	return GameResult::UNDECIDED;
}

void Engine::reset(Board board) 
{
	history.clear();
	history.emplace_back(std::move(board), 0);
}

void Engine::act(Move m) 
{
	Board board(history.back().first, m);
	history.emplace_back(board, count_repetitions(board));
}

int Engine::count_repetitions(const Board &pos) const 
{
	for (auto it = history.rbegin(); it != history.rbegin()+pos.get_ply50(); ++it) {
		if (it->first == pos)
			return 1 + it->second;
	}
	return 0;
}

bool Engine::is_n_repetition(const unsigned n) const
{
	return history.back().second >= n-1;
}
}
