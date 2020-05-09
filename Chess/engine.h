#ifndef BITBOARD_H
#define BITBOARD_H

#include <vector>
#include <chrono>
#include <random>

#include "defines.h"

struct Move {
	Move() {};
	Move(int m, int s) : move(m), score(s) {}
	int move;
	int score;
};

using MoveList = std::vector<Move>;
	  
struct State {
	State() {}
	State(int m, int c, int e, int f, U64 p) :	move(m), 
												castlePerm(c), 
												enPass(e), 
												fiftyMove(f), 
												posKey(p) {}
	int move;
	int castlePerm;
	int enPass;
	int fiftyMove;
	U64 posKey;
};

struct PVEntry {
	U64 posKey = 0ULL;
	int move = 0;
};

struct SearchInfo {
	U64 startTime = 0;
	U64 stopTime = 0; 
	int depth = 0;
	int nodes = 0;
	float fh = 0;
	float fhf = 0;
	bool timeSet = false;
	bool stopped = false;
};

struct Engine {
	Engine();
	
	static int FILES[BRD_SQ_NUM];
	static int RANKS[BRD_SQ_NUM];
	
	static int SQ_120_TO_64[BRD_SQ_NUM];
	static int SQ_64_TO_120[64];
	
	static U64 SET_MASK[64];
	static U64 CLEAR_MASK[64];
	
	static U64 PIECE_KEYS[13][120];
	static U64 CASTLE_KEYS[16];
	
	static int SCORE_MVV_LVA[13][13];
	
	int fr2sq(const int f, const int r) const;
	U64 getTime() const;
	int countBits(U64 bb) const;
	int popBit(U64 *bb) const;
	U64 generatePosKey() const;
	
	bool squareValid(const int sq) const {return FILES[sq] != OFFBOARD && RANKS[sq] != OFFBOARD;}
	bool sideValid(const int s) const {return s == WHITE || s == BLACK;}
	bool fileRankValid(const int fr) const {return fr >= 0 && fr <= 7;}
	bool pieceValid(const int p) const {return p > EMPTY && p <= bK;}
	bool pieceOrEmpty(const int p) const {return p >= EMPTY && p < bK;}
	
	void initSq120To64();
	void initBitmask();
	void initHashKeys();
	void initFilesRanks();
	void initMvvLva();
	
	char *printSquare(const int sq);
	char *printMove(const int move);
	void printAttacked(const int side);
	void printMoveList(MoveList &list);
	void printBitboard(U64 bb);
	void printFenboard();
	
	void storePVMove(const int move);
	int probePVTable();
	int getPVLine(const int depth);
	bool moveExists(const int move);
	
	bool checkBoard();
	void perft(int depth);
	void perftTest(int depth);
	
	void addQuietMove(int move, MoveList &list);
	void addCaptureMove(int move, MoveList &list);
	void addEnpassMove(int move, MoveList &list);
	void addWhitePawnMove(int from, int to, MoveList &list);
	void addWhitePawnCapMove(int from, int to, int capt, MoveList &list);
	void addBlackPawnMove(int from, int to, MoveList &list);
	void addBlackPawnCapMove(int from, int to, int capt, MoveList &list);
	MoveList generateAllMoves();
	
	void checkUp(SearchInfo *info);
	void clearSearch(SearchInfo *info);
	int searchPosition(SearchInfo *info);
	void pickNextMove(int moveNum, MoveList &list);
	int alphaBeta(int alpha, int beta, int depth, SearchInfo *info, bool doNull);
	int quiescence(int alpha, int beta, SearchInfo &info);
	int evaluate();
	
	int parseMove(char *c);
	void parseFen(const char *fen);
	void resetBoard();
	void clearPiece(const int sq);
	void addPiece(const int sq, const int piece);
	void movePiece(const int from, const int to);
	void updateListsMaterial();
	
	bool isAttacked(const int sq, const int side);
	bool isRepetition() const;
	bool isNEmptyMove(const int num) const;
	bool isNFoldRepetitiom(const int num) const;
	
	bool makeMove(int move);
	void undoMove();
	int findMove(int from, int to, MoveList &l);
	
	std::vector<State> history;

	int pieces[BRD_SQ_NUM];
	int kingSq[2];
private:
	int side;
	int enPass;
	int castlePerm;
	int fiftyMove;

	int ply;
	
	U64 sideKey;
	U64 posKey;
	
	U64 pawns[3];
	
	int pieceNum[13];
	int bigPieces[3];
	int majPieces[3];
	int minPieces[3];
	int material[3];
	
	int pieceList[13][10];
	
	U64 leafNodes = 0;
	
	std::vector<PVEntry> pvTable;
	int PVArray[MAX_DEPTH];
	int searchHistory[13][BRD_SQ_NUM];
	int searchKillers[2][MAX_DEPTH];
};

#endif // BITBOARD_H
