#include "engine.h"

void Engine::checkUp(SearchInfo *info) {

	using namespace std::chrono;
	
	if (info->timeSet && getTime() > info->stopTime)
		info->stopped = true;
}

int Engine::searchPosition(SearchInfo *info) {

	int pvMoves = 0;
	int bestMove = NOMOVE;
	int bestScore = std::numeric_limits<int>::min()+1;
	
	int alpha = std::numeric_limits<int>::min()+1;
    int beta = std::numeric_limits<int>::max()-1;
    
	clearSearch(info);
	
	for (int depth = 1; depth <= info->depth; ++depth) {
	
		bestScore = alphaBeta(alpha, beta, depth, info, true);
		
		if (info->stopped) {
			printf("SEARCH ENDED\n");
			fflush(stdout);
			break;
		}
			
		pvMoves = getPVLine(depth);
		bestMove = PVArray[0];
		
		printf("Depth: %d\tscore: %d\tmove: %s\tnodes: %d\t", depth, bestScore, printMove(bestMove), info->nodes);
		printf("ordering: %.2f\n", info->fhf/info->fh);
		printf("pv");
		for (int i = 0; i < pvMoves; ++i)
			printf(" %s", printMove(PVArray[i]));
		printf("\n");
	}
	return bestMove;
}

void Engine::clearSearch(SearchInfo *info) {
	
	for (int i = 0; i < 13; ++i) {
		for (int j = 0; j < BRD_SQ_NUM; ++j)
			searchHistory[i][j] = 0;
	}
	
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < MAX_DEPTH; ++j)
			searchKillers[i][j] = 0;
	}
	
	std::fill(pvTable.begin(), pvTable.end(), PVEntry());
	ply = 0;
	
	info->startTime = getTime();
	info->stopped = false;
	info->nodes = 0;
	info->fh = 0;
	info->fhf = 0;
}

int Engine::alphaBeta(int alpha, int beta, int depth, SearchInfo *info, bool doNull) {
	
	assert(checkBoard());
	
	auto list = generateAllMoves();
	
	info->nodes++;
	
	if (!depth || ply > MAX_DEPTH-1)
		return evaluate();
		
	if (info->nodes & 65535)
		checkUp(info);
	
	if (info->stopped)
		return 0;
		
	if ((isRepetition() || fiftyMove >= 100) && ply > 0)
		return 0;
		
	int legal = 0;
	int bestMove = NOMOVE;
	int oldAlpha = alpha;
	int score = std::numeric_limits<int>::min()+1;
	int pvMove = probePVTable();
	
	if (pvMove) {
		for (auto &it : list) {
			if (it.move == pvMove) {
				it.score = 2'000'000;
				break;
			}
		}
	}

	for (int i = 0; i < list.size(); ++i) {
		
		pickNextMove(i, list);
	
		if (!makeMove(list[i].move))
			continue;

		++legal;
		score = -alphaBeta(-beta, -alpha, depth-1, info, true);
		undoMove();
		
		if (score > alpha) {
			if (score >= beta) {
			
				if (legal == 1)
					info->fhf++;
				info->fh++;
				
				if (!(list[i].move & FLAG_CP)) {
					searchKillers[1][ply] = searchKillers[0][ply];
					searchKillers[0][ply] = list[i].move;
				}
				return beta;
			}
			
			alpha = score;
			bestMove = list[i].move;
			
			if (!(list[i].move & FLAG_CP))
				searchHistory[pieces[FROM(bestMove)]][TO(bestMove)] += depth;
		}
	}
	
	if (!legal) {
		if (isAttacked(kingSq[side], !side))
			return -MATE + ply;
		else
			return 0;
	}
	
	if (alpha != oldAlpha)
		storePVMove(bestMove);
		
	return alpha;
}

int Engine::quiescence(int alpha, int beta, SearchInfo &info) {
	//	TODO
	return 0;
}

int Engine::evaluate() {

	int sq;
	int score = material[WHITE] - material[BLACK];
	
	for (int p = wP, j = 0; p < wQ; ++p, ++j) {
		for (int i = 0; i < pieceNum[p]; ++i) {
			sq = pieceList[p][i];
			assert(squareValid(sq));
			score += SCORE_POS[j][SQ64(sq)];
		}
	}
	
	for (int p = bP, j = 0; p < bQ; ++p, ++j) {
		for (int i = 0; i < pieceNum[p]; ++i) {
			sq = pieceList[p][i];
			assert(squareValid(sq));
			score -= SCORE_POS[j][MIRROR[SQ64(sq)]];
		}
	}
	
	if (side == WHITE)
		return score;
	return -score;	
}

void Engine::pickNextMove(int moveNum, MoveList &list) {

	Move temp;
	int bestScore = 0;
	int bestNum = 0;
	
	for (int i = moveNum; i < list.size(); ++i) {
		if (list[i].score > bestScore) {
			bestScore = list[i].score;
			bestNum = i;
		}
	}
	temp = list[moveNum];
	list[moveNum] = list[bestNum];
	list[bestNum] = temp;
}
