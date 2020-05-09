#include "engine.h"

void Engine::printBitboard(U64 bb) {

	U64 shift = 1ULL;
	
	int sq = 0;
	int sq64 = 0;
	
	printf("\n");
	for (int rank = RANK_8; rank >= RANK_1; --rank) {
		printf("|");
		for (int file = FILE_A; file < FILE_NONE; ++file) {
			sq = fr2sq(file, rank);
			sq64 = SQ64(sq);
			if ((shift << sq64) & bb)
				printf("X");
			else
				printf("-");
			printf("|");
		}
		printf("\n");
	}
	printf("\n\n");
	fflush(stdout);
}

void Engine::printFenboard() {

	int sq, piece;
	
	printf("\nGAME BOARD:\n\n");
	for (int rank = RANK_8; rank >= RANK_1; --rank) {
		printf("%d ", rank+1);
		for (int file = FILE_A; file < FILE_NONE; ++file) {
			sq = fr2sq(file, rank);
			piece = pieces[sq];
			printf("%3c", PIECE_FEN[piece]);
		}
		printf("\n");
	}
	printf("\n  ");
	for (int file = FILE_A; file < FILE_NONE; ++file)
		printf("%3c", 'a'+file);
	printf("\n");
	printf("side: %c\n", SIDE_FEN[side]);
	printf("enPass: %d\n", enPass);
	printf("castle: %c%c%c%c\n", castlePerm & WKCA ? 'K' : '-', 
								 castlePerm & WQCA ? 'Q' : '-', 
							     castlePerm & BKCA ? 'k' : '-',
							     castlePerm & BQCA ? 'q' : '-');
	printf("posKey: %llX\n", posKey);
	fflush(stdout);
}

void Engine::printAttacked(const int s) {
		
	int rank = 0;
	int file = 0;
	int sq = 0;
	
	printf("\n\nSquares attacked by: %c\n", SIDE_FEN[s]);
	for (rank = RANK_8; rank >= RANK_1; --rank) {
		for (file = FILE_A; file < FILE_NONE; ++file) {
			sq = fr2sq(file, rank);
			if (isAttacked(sq, s))
				printf("X");
			else
				printf("-");
		}
		printf("\n");
	}  
    printf("\n\n");
	fflush(stdout);
}

char *Engine::printSquare(const int sq){

	static char str[3];
	
	int file = FILES[sq];
	int rank = RANKS[sq];
	
	sprintf(str, "%c%c", 'a'+file, '1'+rank);
	
	return str;
}

char *Engine::printMove(const int move) {
	
	static char str[6];
	
	int fileFrom = FILES[FROM(move)];
	int rankFrom = RANKS[FROM(move)];
	int fileTo = FILES[TO(move)];
	int rankTo = RANKS[TO(move)];
	
	int prom = PROMOTED(move);
	char c = '\0';
	
	switch (prom) {
		case wQ:
		case bQ: c = 'q'; break;
		case wR:
		case bR: c = 'r'; break;
		case wB:
		case bB: c = 'b'; break;
		case wN:
		case bN: c = 'n'; break;
	}
	sprintf(str, "%c%c%c%c%c", 'a'+fileFrom, '1'+rankFrom, 'a'+fileTo, '1'+rankTo, c);
	return str;
}

void Engine::printMoveList(MoveList &l) {
	
	int move;
	int score;
	
	printf("\nMoveList:\n");
	for (const auto &it : l) {
		move = it.move;
		score = it.score;
		printf("move: %d\t>\t%s score: %d\n", move, printMove(move), score);
	}
	printf("Move List Total: %zu\n", l.size());
	fflush(stdout);
}
