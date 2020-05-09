#include "engine.h"

void Engine::addQuietMove(int move, MoveList &list) {

	assert(squareValid(FROM(move)));
	assert(squareValid(TO(move)));
	
	Move newMove;
	newMove.move = move;
	
	if (searchKillers[0][ply] == move)
		newMove.score = 900'000;
	else if (searchKillers[1][ply] == move)
		newMove.score = 800'000;
	else
		newMove.score = searchHistory[pieces[FROM(move)]][TO(move)];

	list.emplace_back(newMove);
}

void Engine::addCaptureMove(int move, MoveList &list) {

	assert(squareValid(FROM(move)));
	assert(squareValid(TO(move)));
	assert(pieceOrEmpty(CAPTURED(move)));
	
	list.emplace_back(Move(move, SCORE_MVV_LVA[CAPTURED(move)][pieces[FROM(move)]] + 1'000'000));
}

void Engine::addEnpassMove(int move, MoveList &list) {
	list.emplace_back(Move(move, 105 + 1'000'000));
}

void Engine::addWhitePawnMove(int from, int to, MoveList &list) {

	assert(squareValid(from));
	assert(squareValid(to));
	
	if (RANKS[from] == RANK_7) {
		addCaptureMove(MOVE(from, to, EMPTY, wN, 0), list);
		addCaptureMove(MOVE(from, to, EMPTY, wB, 0), list);
		addCaptureMove(MOVE(from, to, EMPTY, wR, 0), list);
		addCaptureMove(MOVE(from, to, EMPTY, wQ, 0), list);
	} else 
		addCaptureMove(MOVE(from, to, EMPTY, EMPTY, 0), list);
}

void Engine::addWhitePawnCapMove(int from, int to, int capt, MoveList &list) {

	assert(squareValid(from));
	assert(squareValid(to));
	assert(pieceOrEmpty(capt));
	
	if (RANKS[from] == RANK_7) {
		addCaptureMove(MOVE(from, to, capt, wN, 0), list);
		addCaptureMove(MOVE(from, to, capt, wB, 0), list);
		addCaptureMove(MOVE(from, to, capt, wR, 0), list);
		addCaptureMove(MOVE(from, to, capt, wQ, 0), list);
	} else 
		addCaptureMove(MOVE(from, to, capt, EMPTY, 0), list);
}

void Engine::addBlackPawnMove(int from, int to, MoveList &list) {

	assert(squareValid(from));
	assert(squareValid(to));
	
	if (RANKS[from] == RANK_2) {
		addCaptureMove(MOVE(from, to, EMPTY, bN, 0), list);
		addCaptureMove(MOVE(from, to, EMPTY, bB, 0), list);
		addCaptureMove(MOVE(from, to, EMPTY, bR, 0), list);
		addCaptureMove(MOVE(from, to, EMPTY, bQ, 0), list);
	} else 
		addCaptureMove(MOVE(from, to, EMPTY, EMPTY, 0), list);
}

void Engine::addBlackPawnCapMove(int from, int to, int capt, MoveList &list) {
	
	assert(squareValid(from));
	assert(squareValid(to));
	assert(pieceOrEmpty(capt));
	
	if (RANKS[from] == RANK_2) {
		addCaptureMove(MOVE(from, to, capt, bN, 0), list);
		addCaptureMove(MOVE(from, to, capt, bB, 0), list);
		addCaptureMove(MOVE(from, to, capt, bR, 0), list);
		addCaptureMove(MOVE(from, to, capt, bQ, 0), list);
	} else 
		addCaptureMove(MOVE(from, to, capt, EMPTY, 0), list);
}

MoveList Engine::generateAllMoves() {

	assert(checkBoard());
	
	MoveList list;
	list.reserve(200);
	
	int p = EMPTY;
	int sq = 0;
	int t_sq = 0;
	int pNum = 0;
	
	int dir = 0;
	int i = 0;
	int pIdx = 0;
	
	if (side == WHITE) {
		for (pNum = 0; pNum < pieceNum[wP]; ++pNum) {
		
			sq = pieceList[wP][pNum];
			assert(squareValid(sq));
			
			if (pieces[sq+10] == EMPTY) {
				addWhitePawnMove(sq, sq+10, list);
				if (RANKS[sq] == RANK_2 && pieces[sq+20] == EMPTY)
					addQuietMove(MOVE(sq, (sq+20), EMPTY, EMPTY, FLAG_PS), list);
			}
				
			if (squareValid(sq+9) && PIECE_COL[pieces[sq+9]] == BLACK)
				addWhitePawnCapMove(sq, (sq+9), pieces[sq+9], list);
			
			if (squareValid(sq+11) && PIECE_COL[pieces[sq+11]] == BLACK)
				addWhitePawnCapMove(sq, (sq+11), pieces[sq+11], list);
				
			if (enPass != NO_SQ) {
				if (sq+9 == enPass)
					addEnpassMove(MOVE(sq, (sq+9), EMPTY, EMPTY, FLAG_EP), list);
					
				if (sq+11 == enPass)
					addEnpassMove(MOVE(sq, (sq+11), EMPTY, EMPTY, FLAG_EP), list);
			}
		}
		
		if (castlePerm & WKCA) {
			if (pieces[F1] == EMPTY && pieces[G1] == EMPTY) {
				if (!isAttacked(E1, BLACK) && !isAttacked(F1, BLACK))
					addQuietMove(MOVE(E1, G1, EMPTY, EMPTY, FLAG_CA), list);
			}
		}
		
		if (castlePerm & WQCA) {
			if (pieces[D1] == EMPTY && pieces[C1] == EMPTY && pieces[B1] == EMPTY) {
				if (!isAttacked(E1, BLACK) && !isAttacked(D1, BLACK))
					addQuietMove(MOVE(E1, C1, EMPTY, EMPTY, FLAG_CA), list);
			}
		}
	} else {
		for (pNum = 0; pNum < pieceNum[bP]; ++pNum) {
		
			sq = pieceList[bP][pNum];
			assert(squareValid(sq));
			
			if (pieces[sq-10] == EMPTY) {
				addBlackPawnMove(sq, sq-10, list);
				if (RANKS[sq] == RANK_7 && pieces[sq-20] == EMPTY)
					addQuietMove(MOVE(sq, (sq-20), EMPTY, EMPTY, FLAG_PS), list);
			}
				
			if (squareValid(sq-9) && PIECE_COL[pieces[sq-9]] == WHITE)
				addBlackPawnCapMove(sq, (sq-9), pieces[sq-9], list);
			
			if (squareValid(sq-11) && PIECE_COL[pieces[sq-11]] == WHITE)
				addBlackPawnCapMove(sq, (sq-11), pieces[sq-11], list);
				
			if (enPass != NO_SQ) {
				if (sq-9 == enPass)
					addEnpassMove(MOVE(sq, (sq-9), EMPTY, EMPTY, FLAG_EP), list);
					
				if (sq-11 == enPass)
					addEnpassMove(MOVE(sq, (sq-11), EMPTY, EMPTY, FLAG_EP), list);
			}	
		}
		
		if (castlePerm & BKCA) {
			if (pieces[F8] == EMPTY && pieces[G8] == EMPTY) {
				if (!isAttacked(E8, WHITE) && !isAttacked(F8, WHITE))
					addQuietMove(MOVE(E8, G8, EMPTY, EMPTY, FLAG_CA), list);
			}
		}
		
		if (castlePerm & BQCA) {
			if (pieces[D8] == EMPTY && pieces[C8] == EMPTY && pieces[B8] == EMPTY) {
				if (!isAttacked(E8, WHITE) && !isAttacked(D8, WHITE))
					addQuietMove(MOVE(E8, C8, EMPTY, EMPTY, FLAG_CA), list);
			}
		}
	}
	
	pIdx = LOOP_SLIDE_IDX[side];
	p = LOOP_SLIDE[pIdx++];
	while (p) {
		assert(pieceValid(p));
		for (pNum = 0; pNum < pieceNum[p]; ++pNum){
			sq = pieceList[p][pNum];
			assert(squareValid(sq));
			for (i = 0; i < NUM_DIR[p]; ++i) {
			
				dir = PIECE_DIR[p][i];
				t_sq = sq + dir;
				
				while (squareValid(t_sq)) {
					if (pieces[t_sq] != EMPTY) {
						if (PIECE_COL[pieces[t_sq]] != side)
							addCaptureMove(MOVE(sq, t_sq, pieces[t_sq], EMPTY, 0), list);
						break;
					}
					addQuietMove(MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
					t_sq += dir;
				}
			}
		}
		p = LOOP_SLIDE[pIdx++];
	}
	
	pIdx = LOOP_NONSLIDE_IDX[side];
	p = LOOP_NONSLIDE[pIdx++];
	while (p) {
		assert(pieceValid(p));
		for (pNum = 0; pNum < pieceNum[p]; ++pNum){
			sq = pieceList[p][pNum];
			assert(squareValid(sq));
			for (i = 0; i < NUM_DIR[p]; ++i) {
			
				dir = PIECE_DIR[p][i];
				t_sq = sq + dir;
				
				if (!squareValid(t_sq))
					continue;
					
				if (pieces[t_sq] != EMPTY) {
					if (PIECE_COL[pieces[t_sq]] != side)
						addCaptureMove(MOVE(sq, t_sq, pieces[t_sq], EMPTY, 0), list);
					continue;
				}
				addQuietMove(MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
			}
		}
		p = LOOP_NONSLIDE[pIdx++];
	}
	return list;
}
