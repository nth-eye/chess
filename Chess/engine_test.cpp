#include "engine.h"

bool Engine::checkBoard() {

	int t_piece, t_pce_num, sq120, sq64, color, count;
	
	int t_pieceNum[13] = {};
	int t_bigPieces[2] = {};
	int t_majPieces[2] = {};
	int t_minPieces[2] = {};
	int t_material[2] = {};
	
	U64 t_pawns[3] = {};
	
	t_pawns[WHITE] = pawns[WHITE];
	t_pawns[BLACK] = pawns[BLACK];
	t_pawns[BOTH] = pawns[BOTH];
	
	for (t_piece = wP; t_piece <= bK; ++t_piece) {
		for (t_pce_num = 0; t_pce_num < pieceNum[t_piece]; ++t_pce_num) {
			sq120 = pieceList[t_piece][t_pce_num];
			assert(pieces[sq120] == t_piece);
		}
	}
	
	for (sq64 = 0; sq64 < 64; ++sq64) {
		sq120 = SQ120(sq64);
		t_piece = pieces[sq120];
		t_pieceNum[t_piece]++;
		color = PIECE_COL[t_piece];
		
		if (PIECE_BIG[t_piece]) t_bigPieces[color]++;
		if (PIECE_MAJ[t_piece]) t_majPieces[color]++;
		if (PIECE_MIN[t_piece]) t_minPieces[color]++;
		
		t_material[color] += PIECE_VAL[t_piece];
	}
	
	for (t_piece = wP; t_piece <= bK; ++t_piece)
		assert(t_pieceNum[t_piece] == pieceNum[t_piece]);
		
	count = countBits(t_pawns[WHITE]);
	assert(count == pieceNum[wP]);
	count = countBits(t_pawns[BLACK]);
	assert(count == pieceNum[bP]);
	count = countBits(t_pawns[BOTH]);
	assert(count == (pieceNum[wP] + pieceNum[bP]));
	
	while(t_pawns[WHITE]) {
		sq64 = popBit(&t_pawns[WHITE]);
		assert(pieces[SQ120(sq64)] == wP);
	}
	
	while(t_pawns[BLACK]) {
		sq64 = popBit(&t_pawns[BLACK]);
		assert(pieces[SQ120(sq64)] == bP);
	}
	
	while(t_pawns[BOTH]) {
		sq64 = popBit(&t_pawns[BOTH]);
		assert((pieces[SQ120(sq64)] == wP) || (pieces[SQ120(sq64)] == bP));
	}
	
	assert(t_material[WHITE] == material[WHITE] && t_material[BLACK] == material[BLACK]);
	assert(t_minPieces[WHITE] == minPieces[WHITE] && t_minPieces[BLACK] == minPieces[BLACK]);
	assert(t_majPieces[WHITE] == majPieces[WHITE] && t_majPieces[BLACK] == majPieces[BLACK]);
	assert(t_bigPieces[WHITE] == bigPieces[WHITE] && t_bigPieces[BLACK] == bigPieces[BLACK]);	
	
	assert(side == WHITE || side == BLACK);
	assert(generatePosKey() == posKey);
	
	assert(enPass == NO_SQ	|| (RANKS[enPass] == RANK_6 && side == WHITE)
							|| (RANKS[enPass] == RANK_3 && side == BLACK));
	
	assert(pieces[kingSq[WHITE]] == wK);
	assert(pieces[kingSq[BLACK]] == bK);
	
	return true;	 
}

void Engine::perft(int depth) {

	assert(checkBoard());
	
	if (!depth) {
		leafNodes++;
		return;
	}
	
	int move = NOMOVE;
	const auto list = generateAllMoves();
	
	for (const auto &it : list) {
		move = it .move;
		if (!makeMove(move))
			continue;
		perft(depth-1);
		undoMove();
	}
	return;
}

void Engine::perftTest(int depth) {
	
	assert(checkBoard());
	printFenboard();
	printf("\nStarting test to depth: %d\n", depth);
	
	leafNodes = 0;
	int move = NOMOVE;
	
	auto start = std::chrono::steady_clock::now();
	const auto list = generateAllMoves();

	for (const auto &it : list) {
		move = it.move;
		
		if (!makeMove(move))
			continue;
		
		U64 accumNodes = leafNodes;
		perft(depth-1);
		undoMove();
		
		U64 oldNodes = leafNodes - accumNodes;
		printf("%s\t%llu\n", printMove(move), oldNodes);
	}
	std::chrono::duration<double> time = std::chrono::steady_clock::now() - start;
	printf("\nTest completed: %llu nodes visited in %f s\n", leafNodes, time.count());
	fflush(stdout);
	return;
}
