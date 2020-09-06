#include <tuple>

#include "board.h"

#pragma once

namespace chess {

enum GameResult : uint8_t { UNDECIDED, WHITE_WON, BLACK_WON, DRAW };

enum {	EMPTY,
        P, N, B, R, Q, K };

enum {	WHITE, BLACK, BOTH	};


struct SearchInfo {
    uint64_t start_time = 0;
    uint64_t stop_time = 0;
    int depth = 0;
    int nodes = 0;
    float fh = 0;
    float fhf = 0;
    bool time_set = false;
    bool stopped = false;
};

struct Engine {

	GameResult result() const;

    MoveList legal_moves() { return history.back().first.legal_actions(); }

    Move search_position(SearchInfo *info);
    int alpha_beta(Board board, int alpha, int beta, int depth, SearchInfo *info, bool doNull);
    int evaluate(Board board);

    void clear_search(SearchInfo *info);
    void check_up(SearchInfo *info)
    {
        using namespace std::chrono;

        if (info->time_set && get_time() > info->stop_time)
            info->stopped = true;
    }
    uint64_t get_time() const
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
    std::vector<std::tuple<int, int, int, int>> pieces_with_positions() const;

	void reset(Board board);
	void reset() { reset(Board(START_FEN)); }
	void act(Move m);

    size_t size() const { return history.size(); }

    bool is_n_repetition(const unsigned n) const;

    bool check() const { return history.back().first.check(); }
    bool draw_available() const { return is_n_repetition(3) || history.back().first.get_ply50() > 100; }

private:
    int count_repetitions(const Board &pos) const;

	std::vector<std::pair<Board, unsigned>> history;
};

}
