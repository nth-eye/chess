#include "engine.h"

int Engine::FILES[BRD_SQ_NUM] = {};	
int Engine::RANKS[BRD_SQ_NUM] = {};

int Engine::SQ_120_TO_64[BRD_SQ_NUM] = {};
int Engine::SQ_64_TO_120[64] = {};

U64 Engine::SET_MASK[64] = {};
U64 Engine::CLEAR_MASK[64] = {};

U64 Engine::PIECE_KEYS[13][120] = {};
U64 Engine::CASTLE_KEYS[16] = {};

int Engine::SCORE_MVV_LVA[13][13] = {};

Engine::Engine() : pvTable(PVTABLE_SIZE/sizeof(PVEntry))
{
	initFilesRanks();
	initSq120To64();
	initBitmask();
	initHashKeys();
	initMvvLva();
	
	history.reserve(256);
	
	parseFen(START_FEN);
	perftTest(5);
}

void Engine::initFilesRanks() {

	int sq = 0;
	
	for(int index = 0; index < BRD_SQ_NUM; ++index) {
		FILES[index] = OFFBOARD;
		RANKS[index] = OFFBOARD;
	}
	
	for (int rank = RANK_1; rank < RANK_NONE; ++rank) {
		for (int file = FILE_A; file < FILE_NONE; ++file) {
			sq = fr2sq(file, rank);
			FILES[sq] = file;
			RANKS[sq] = rank;
		}
	}
}

void Engine::initSq120To64() {
	
	int sq = A1;
	int sq64 = 0;
	
	for (int i = 0; i < BRD_SQ_NUM; ++i)
		SQ_120_TO_64[i] = 65;
	
	for(int i = 0; i < 64; ++i)
		SQ_64_TO_120[i] = 120;
		
	for (int rank = RANK_1; rank < RANK_NONE; ++rank) {
		for (int file = FILE_A; file < FILE_NONE; ++file) {
			sq = fr2sq(file, rank);
			SQ_64_TO_120[sq64] = sq;
			SQ_120_TO_64[sq] = sq64;
			sq64++;
		}
	}
}

void Engine::initBitmask() {

	for (int i = 0; i < 64; ++i) {
		SET_MASK[i] |= (1ULL << i);
		CLEAR_MASK[i] = ~SET_MASK[i];
	}
}

void Engine::initHashKeys() {
	
	std::mt19937_64 rand64;
		
	for (int i = 0; i < 13; ++i) {
		for (int j = 0; j < BRD_SQ_NUM; ++j)
			PIECE_KEYS[i][j] = rand64();
	}
	sideKey = rand64();
	for (int i = 0; i < 16; ++i)
		CASTLE_KEYS[i] = rand64();
}

void Engine::initMvvLva() {
	
	for (int att = wP; att <= bK; ++att) {
		for (int vic = wP; vic <= bK; ++vic)
			SCORE_MVV_LVA[vic][att] = SCORE_VICTIM[vic] + 6 - SCORE_VICTIM[att]/100;
	}
}

void Engine::resetBoard() {
	
	for (int sq = 0; sq < BRD_SQ_NUM; ++sq) 
		pieces[sq] = OFFBOARD;
	
	for (int i = 0; i < 64; ++i)
		pieces[SQ120(i)] = EMPTY;
		
	for (int i = 0; i < 3; ++i) {
		bigPieces[i] = 0;
		majPieces[i] = 0;
		minPieces[i] = 0;
		material[i] = 0;
		pawns[i] = 0ULL;
	}
	
	for (int i = 0; i < 13; ++i)
		pieceNum[i] = 0;
		
	kingSq[WHITE] = kingSq[BLACK] = NO_SQ;
	
	side = BOTH;
	ply = 0;
	enPass = NO_SQ;
	castlePerm = 0;
	fiftyMove = 0;
	posKey = 0ULL;
	history.clear();
}

void Engine::parseFen(const char *fen) {

	resetBoard();
	
	int rank = RANK_8;
	int file = FILE_A;
	int piece = 0;
	int count = 0;
	int sq64 = 0;
	int sq120 = 0;
	
	while ((rank >= RANK_1) && *fen) {
		count = 1;
		switch (*fen) {
			case 'p': piece = bP; break;
			case 'n': piece = bN; break;
			case 'b': piece = bB; break;
			case 'r': piece = bR; break;
			case 'q': piece = bQ; break;
			case 'k': piece = bK; break;
			case 'P': piece = wP; break;
			case 'N': piece = wN; break;
			case 'B': piece = wB; break;
			case 'R': piece = wR; break;
			case 'Q': piece = wQ; break;
			case 'K': piece = wK; break;
			
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				piece = EMPTY;
				count = *fen - '0';
				break;
			
			case ' ':
			case '/':
				--rank;
				file = FILE_A;
				++fen;
				continue;
			
			default:
				printf("FEN error \n");
				fflush(stdout);
		}
		
		for (int i = 0; i < count; ++i, ++file) {
			sq64 = rank*8 + file;
			sq120 = SQ120(sq64);
			if (piece != EMPTY)
				pieces[sq120] = piece;
		}
		++fen;
	}
	side = (*fen == 'w') ? WHITE : BLACK;
	
	fen += 2;
	
	for (int i = 0; i < 4; ++i) {
		if (*fen == ' ')
			break;
		switch (*fen) {
			case 'K': castlePerm |= WKCA; break;
			case 'Q': castlePerm |= WQCA; break;
			case 'k': castlePerm |= BKCA; break;
			case 'q': castlePerm |= BQCA; break;
			default: break;
		}
		++fen;
	}
	++fen;
	
	if (*fen != '-') {
		file = fen[0] - 'a';
		rank = fen[1] - '1';
		enPass = fr2sq(file, rank);
	}
	posKey = generatePosKey();
	updateListsMaterial();
}

int Engine::parseMove(char *c) {
	
	if (c[0] > 'h' || c[0] < 'a') return false;
	if (c[1] > '8' || c[1] < '1') return false;
	if (c[2] > 'h' || c[2] < 'a') return false;
	if (c[3] > '8' || c[3] < '1') return false;
	
	int from = fr2sq(c[0]-'a', c[1]-'1');
	int to = fr2sq(c[2]-'a', c[3]-'1');
	
	printf("c: %sfrom: %d to: %d\n", c, from, to);
	fflush(stdout);
	
	assert(squareValid(from));
	assert(squareValid(to));
	
	const auto list = generateAllMoves();
	
	int move = 0;
	int promP = EMPTY;
	for (const auto &it : list) {
		move = it.move;
		if (FROM(move) == from && TO(move) == to) {
			promP = PROMOTED(move);
			switch (promP) {
				case EMPTY: return move;
				case wN:
				case bN: c[4] = 'n'; return move;
				case wB:
				case bB: c[4] = 'b'; return move;
				case wR:
				case bR: c[4] = 'r'; return move;
				case wQ:
				case bQ: c[4] = 'q'; return move;
				default: continue;
			}
		}
	}
	return NOMOVE;
}

void Engine::updateListsMaterial() {

	int piece, color;
	
	for (int sq = 0; sq < BRD_SQ_NUM; ++sq) {
	
		piece = pieces[sq];
		
		if (piece != OFFBOARD && piece != EMPTY) {
		
			color = PIECE_COL[piece];
			
			if (PIECE_BIG[piece]) bigPieces[color]++;
			if (PIECE_MAJ[piece]) majPieces[color]++;
			if (PIECE_MIN[piece]) minPieces[color]++;
			
			material[color] += PIECE_VAL[piece];
			
			pieceList[piece][pieceNum[piece]] = sq;
			pieceNum[piece]++;
			
			if (piece == wK || piece == bK) 
				kingSq[color] = sq;	
				
			if (piece == wP || piece == bP) {
				SETBIT(pawns[color], SQ64(sq));
				SETBIT(pawns[BOTH], SQ64(sq));
			}
		}
	}
}

void Engine::clearPiece(const int sq) {
	
	int p = pieces[sq];
	int c = PIECE_COL[p];
	int pNum = -1;
	
	assert(squareValid(sq));
	assert(pieceValid(p));
	
	pieces[sq] = EMPTY;
	material[c] -= PIECE_VAL[p];
	
	if (PIECE_BIG[p]) {
		bigPieces[c]--;
		if (PIECE_MAJ[p])
			majPieces[c]--;
		else 
			minPieces[c]--;
		
	} else {
		CLRBIT(pawns[c], SQ64(sq));
		CLRBIT(pawns[BOTH], SQ64(sq));
	}
	
	for (int i = 0; i < pieceNum[p]; ++i) {
		if (pieceList[p][i] == sq) {
			pNum = i;
			break;
		}
	}
	
	assert(pNum != -1);
	pieceNum[p]--;
	pieceList[p][pNum] = pieceList[p][pieceNum[p]];
}

void Engine::addPiece(const int sq, const int p) {

	assert(pieceValid(p));
	assert(squareValid(sq));
	
	int c = PIECE_COL[p];
	
	pieces[sq] = p;
	
	if (PIECE_BIG[p]) {
		bigPieces[c]++;
		if (PIECE_MAJ[p])
			majPieces[c]++;
		else 
			minPieces[c]++;
		
	} else {
		SETBIT(pawns[c], SQ64(sq));
		SETBIT(pawns[BOTH], SQ64(sq));
	}
	
	material[c] += PIECE_VAL[p];
	pieceList[p][pieceNum[p]++] = sq;
}

void Engine::movePiece(const int from, const int to) {
	
	assert(squareValid(from));
	assert(squareValid(to));
	
	int p = pieces[from];
	int c = PIECE_COL[p];
	
	pieces[from] = EMPTY;
	pieces[to] = p;
	
	if (!PIECE_BIG[p]) {
		CLRBIT(pawns[c], SQ64(from));
		CLRBIT(pawns[BOTH], SQ64(from));
		SETBIT(pawns[c], SQ64(to));
		SETBIT(pawns[BOTH], SQ64(to));
	}
	
	for (int i = 0; i < pieceNum[p]; ++i) {
		if (pieceList[p][i] == from) {
			pieceList[p][i] = to;
			break;
		}
	}
}

bool Engine::makeMove(int move) {

	int from = FROM(move);
	int to = TO(move);
	
	assert(checkBoard());
	assert(squareValid(from));
	assert(squareValid(to));
	assert(sideValid(side));
	assert(pieceValid(pieces[from]));
	
	history.emplace_back(State(move, castlePerm, enPass, fiftyMove, posKey));
	
	if (move & FLAG_EP) {
		if (side == WHITE) {
			posKey ^= PIECE_KEYS[pieces[to-10]][to-10];
			clearPiece(to-10);
		} else {
			posKey ^= PIECE_KEYS[pieces[to+10]][to+10];
			clearPiece(to+10);
		}
	} else if (move & FLAG_CA) {
		switch (to) {
			case C1: 
				posKey ^= PIECE_KEYS[pieces[A1]][A1];
				posKey ^= PIECE_KEYS[pieces[A1]][D1];
				movePiece(A1, D1); 
				break;
			case C8: 
				posKey ^= PIECE_KEYS[pieces[A8]][A8];
				posKey ^= PIECE_KEYS[pieces[A8]][D8];
				movePiece(A8, D8); 
				break;
			case G1: 
				posKey ^= PIECE_KEYS[pieces[H1]][H1];
				posKey ^= PIECE_KEYS[pieces[H1]][F1];
				movePiece(H1, F1); 
				break;
			case G8: 
				posKey ^= PIECE_KEYS[pieces[H8]][H8];
				posKey ^= PIECE_KEYS[pieces[H8]][F8];
				movePiece(H8, F8); 
					break;
			default: 
				assert(false); 
				break;
		}
	}
	
	if (enPass != NO_SQ)
		posKey ^= PIECE_KEYS[EMPTY][enPass];
	posKey ^= CASTLE_KEYS[castlePerm];
	
	castlePerm &= CASTLE_PERM[from];
	enPass = NO_SQ;
	
	posKey ^= CASTLE_KEYS[castlePerm];
	
	++ply;
	++fiftyMove;
	
	int capt = CAPTURED(move);
	if (capt != EMPTY) {
		assert(pieceValid(capt));
		posKey ^= PIECE_KEYS[pieces[to]][to];
		clearPiece(to);
		fiftyMove = 0;
	}
	
	if (PIECE_P[pieces[from]]) {
		fiftyMove = 0;
		if (move & FLAG_PS) {
			if (side == WHITE) {
				enPass = from+10;
				assert(RANKS[enPass] == RANK_3);
			} else {
				enPass = from-10;
				assert(RANKS[enPass] == RANK_6);
			}
			posKey ^= PIECE_KEYS[EMPTY][enPass];
		}
	}
	
	posKey ^= PIECE_KEYS[pieces[from]][from];
	posKey ^= PIECE_KEYS[pieces[from]][to];
	movePiece(from, to);
	
	int promP = PROMOTED(move);
	if (promP != EMPTY) {
		assert(pieceValid(promP) && !PIECE_P[promP]);
		posKey ^= PIECE_KEYS[pieces[to]][to];
		clearPiece(to);
		posKey ^= PIECE_KEYS[promP][to];
		addPiece(to, promP);
	}
	
	if (PIECE_K[pieces[to]])
		kingSq[side] = to;
		
	side ^= 1;
	posKey ^= sideKey;
	
	assert(checkBoard());
	
	if (isAttacked(kingSq[!side], side)) {
		undoMove();
		return false;
	}
	return true;
}

void Engine::undoMove() {
	
	assert(checkBoard());
	
	const State state = history.back();

	castlePerm = state.castlePerm;
	fiftyMove = state.fiftyMove;
	enPass = state.enPass;
	posKey = state.posKey;
	
	int move = state.move;
	int from = FROM(move);
	int to = TO(move);
	
	assert(squareValid(from));
	assert(squareValid(to));

	--ply;
	side ^= 1;
	
	if (move & FLAG_EP) {
		if (side == WHITE)
			addPiece(to-10, bP);
		else 
			addPiece(to+10, wP);
	} else if (move & FLAG_CA) {
		switch (to) {
			case C1: movePiece(D1, A1); break;
			case C8: movePiece(D8, A8); break;
			case G1: movePiece(F1, H1); break;
			case G8: movePiece(F8, H8); break;
			default: assert(false); break;
		}
	}
	
	movePiece(to, from);
	
	if (PIECE_K[pieces[from]])
		kingSq[side] = from;
		
	int capt = CAPTURED(move);
	if (capt != EMPTY) {
		assert(pieceValid(capt));
		addPiece(to, capt);
	}
	
	int promP = PROMOTED(move);
	if (promP != EMPTY) {
		assert(pieceValid(promP) && !PIECE_P[promP]);
		clearPiece(from);
		addPiece(from, PIECE_COL[promP] == WHITE ? wP : bP);
	}
	history.pop_back();
	
	assert(checkBoard());
}


bool Engine::moveExists(const int move) {
	
	const auto list = generateAllMoves();
	
	for (const auto &it : list) {
		if (!makeMove(it.move))
			continue;
		undoMove();
		if (it.move == move)
			return true;
	}
	return false;
}

int Engine::findMove(int from, int to, MoveList &l) {
	
	for (const auto &it : l) {
		if (FROM(it.move) == from && TO(it.move) == to)
			return it.move;
	}
	return NOMOVE;
}

bool Engine::isAttacked(const int sq, const int s) {
	
	assert(squareValid(sq));
	assert(sideValid(side));
	assert(checkBoard());

	int p, dir, t_sq;

	// for pawns
	if (s == WHITE) {
		if (pieces[sq-11] == wP || pieces[sq-9] == wP)
			return true;
	} else {
		if (pieces[sq+11] == bP || pieces[sq+9] == bP)
			return true;
	}
	// for knights
	for (int i = 0; i < 8; ++i) {
		p = pieces[sq + N_DIR[i]];
		if (isN(p) && PIECE_COL[p] == s)
			return true;
	}
	// for kings
	for (int i = 0; i < 8; ++i) {		
		p = pieces[sq + K_DIR[i]];
		if (isK(p) && PIECE_COL[p] == s)
			return true;
	}
	// for bishop and queen
	for(int i = 0; i < 4; ++i) {		
		dir = B_DIR[i];
		t_sq = sq + dir;
		p = pieces[t_sq];
		while (p != OFFBOARD) {
			if (p != EMPTY) {
				if(isBQ(p) && PIECE_COL[p] == s)
					return true;
				break;
			}
			t_sq += dir;
			p = pieces[t_sq];
		}
	}
	// for rook and queen
	for (int i = 0; i < 4; ++i) {
		dir = R_DIR[i];
		t_sq = sq + dir;
		p = pieces[t_sq];
		while (p != OFFBOARD) {
			if (p != EMPTY) {
				if (isRQ(p) && PIECE_COL[p] == s)
					return true;
				break;
			}
			t_sq += dir;
			p = pieces[t_sq];	
		}
	}
	return false;
}

bool Engine::isRepetition() const {
	for (size_t i = history.size()-fiftyMove; i < history.size(); ++i) {
		assert(i >= 0);
		if (posKey == history[i].posKey)
			return true;
	}
	return false;
}

bool Engine::isNEmptyMove(const int n) const {
	return fiftyMove >= n;
}

bool Engine::isNFoldRepetitiom(const int n) const {
	int count = 1;
	for (size_t i = history.size()-fiftyMove; i < history.size(); ++i) {
		assert(i >= 0);
		if (posKey == history[i].posKey)
			++count;
	}
	return count >= n;
}
