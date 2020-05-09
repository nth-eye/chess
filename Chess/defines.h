#ifndef DEFINES_H
#define DEFINES_H

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>

#define SQ64(sq120) SQ_120_TO_64[sq120]
#define SQ120(sq64) SQ_64_TO_120[sq64]

#define CLRBIT(bb, sq) (bb &= CLEAR_MASK[sq])
#define SETBIT(bb, sq) (bb |= SET_MASK[sq])
					
#define isN(piece) (PIECE_N[(piece)])
#define isBQ(piece) (PIECE_B[(piece)])
#define isRQ(piece) (PIECE_R[(piece)])
#define isK(piece) (PIECE_K[(piece)])

#define FROM(m) (m & 0x7f)
#define TO(m) ((m >> 7) & 0x7f)
#define CAPTURED(m) ((m >> 14) & 0xf)
#define PROMOTED(m) ((m >> 20) & 0xf)
#define MOVE(f, t, c, p, fl) (f | (t << 7) | (c << 14) | (p << 20) | fl) 

using U64 = uint64_t;

constexpr int FLAG_EP = 0x40000;
constexpr int FLAG_PS = 0x80000;
constexpr int FLAG_CA = 0x1000000;
constexpr int FLAG_CP = 0x7c000;
constexpr int FLAG_PR = 0xf00000;

constexpr int NOMOVE = 0;

enum {	EMPTY, 
		wP, wN, wB, wR, wQ, wK, 
		bP, bN, bB, bR, bQ, bK	};
enum {	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE	};
enum {	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE	};
enum {	WHITE, BLACK, BOTH	};
enum {	WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8	};

enum {A1 = 21, B1, C1, D1, E1, F1, G1, H1, 
	  A2 = 31, B2, C2, D2, E2, F2, G2, H2, 
	  A3 = 41, B3, C3, D3, E3, F3, G3, H3, 
	  A4 = 51, B4, C4, D4, E4, F4, G4, H4, 
	  A5 = 61, B5, C5, D5, E5, F5, G5, H5, 
	  A6 = 71, B6, C6, D6, E6, F6, G6, H6,
	  A7 = 81, B7, C7, D7, E7, F7, G7, H7,
	  A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD	};
	  
constexpr char *START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr char *PIECE_FEN = ".PNBRQKpnbrqk";
constexpr char *SIDE_FEN = "wb-";
constexpr char *RANK_FEN = "12345678";
constexpr char *FILE_FEN = "abcdefgh";
	  
constexpr int PIECE_SLIDE[13] = {	false, false, false, true, true, true, false, false, false, true, true, true, false	};
constexpr int PIECE_BIG[13] = {	false, false, true, true, true, true, true, false, true, true, true, true, true	};
constexpr int PIECE_MAJ[13] = {	false, false, false, false, true, true, true, false, false, false, true, true, true	};
constexpr int PIECE_MIN[13] = {	false, false, true, true, false, false, false, false, true, true, false, false, false	};
constexpr int PIECE_VAL[13] = {	0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000	};
constexpr int PIECE_COL[13] = {	BOTH, 
								WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
								BLACK, BLACK, BLACK, BLACK, BLACK, BLACK	};

constexpr int PIECE_P[13] = {	false, true, false, false, false, false, false, true, false, false, false, false, false	};						
constexpr int PIECE_N[13] = {	false, false, true, false, false, false, false, false, true, false, false, false, false	};
constexpr int PIECE_B[13] = {	false, false, false, true, false, true, false, false, false, true, false, true, false	};
constexpr int PIECE_R[13] = {	false, false, false, false, true, true, false, false, false, false, true, true, false	};
constexpr int PIECE_K[13] = {	false, false, false, false, false, false, true, false, false, false, false, false, true	};
									
constexpr int BRD_SQ_NUM = 120;
constexpr int MAX_DEPTH = 64;
constexpr int PVTABLE_SIZE = 2'000'000;
constexpr int MATE = 29000;
constexpr int BIT_TABLE[64] = {	63, 30, 3, 32, 25, 41, 22, 33, 
								15, 50, 42, 13, 11, 53, 19, 34, 
								61, 29, 2, 51, 21, 43, 45, 10, 
								18, 47, 1, 54, 9, 57, 0, 35, 
								62, 31, 40, 4, 49, 5, 52, 26, 
								60, 6, 23, 44, 46, 27, 56, 16, 
								7, 39, 48, 24, 59, 14, 12, 55, 
								38, 28, 58, 20, 37, 17, 36, 8	};
							
constexpr int N_DIR[] = {	-8, -19, -21, -12, 8, 19, 21, 12	};
constexpr int B_DIR[] = {	-9, -11, 11, 9	};
constexpr int R_DIR[] = {	-1, -10, 1, 10	};
constexpr int K_DIR[] = {	-1, -10, 1, 10, -9, -11, 11, 9	};

constexpr int NUM_DIR[13] = {	0, 2, 8, 4, 4, 8, 8, 2, 8, 4, 4, 8, 8	};
constexpr int PIECE_DIR[13][8] = {	{},
									{	10,	20, 9, 11	}, 
									{	-8, -19, -21, -12, 8, 19, 21, 12	}, 
									{	-9, -11, 11, 9, 0, 0, 0, 0	}, 
									{	-1, -10, 1, 10, 0, 0, 0, 0	}, 
									{	-1, -10, 1, 10, -9, -11, 11, 9	}, 
									{	-1, -10, 1, 10, -9, -11, 11, 9	}, 
									{	-10, -20, -9, -11	}, 
									{	-8, -19, -21, -12, 8, 19, 21, 12	}, 
									{	-9, -11, 11, 9, 0, 0, 0, 0	}, 
									{	-1, -10, 1, 10, 0, 0, 0, 0	}, 
									{	-1, -10, 1, 10, -9, -11, 11, 9	}, 
									{	-1, -10, 1, 10, -9, -11, 11, 9	}	};

constexpr int LOOP_SLIDE[] = {	wB, wR, wQ, 0, bB, bR, bQ, 0	};
constexpr int LOOP_NONSLIDE[] = {	wN, wK, 0, bN, bK, 0	};

constexpr int LOOP_SLIDE_IDX[] = {	0, 4	};
constexpr int LOOP_NONSLIDE_IDX[] = {	0, 3	};

constexpr int LOOP_DIR[] = {	2, 8	};

constexpr int CASTLE_PERM[120] = {	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
									15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
									15, 13, 15, 15, 15, 12, 15, 15, 14, 15, 
									15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
									15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
									15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
									15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
									15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
									15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
									15,  7, 15, 15, 15,  3, 15, 15, 11, 15, 
									15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
									15, 15, 15, 15, 15, 15, 15, 15, 15, 15	};
								
constexpr int SCORE_POS[4][64] ={	{	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
										10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	, 
										5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	, 
										0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	, 
										5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	, 
										10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	, 
										20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	, 
										0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		},
									{	0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
										0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	, 
										0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
										0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
										5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
										5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
										0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
										0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		},
									{	0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	, 
										0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	, 
										0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	, 
										0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	, 
										0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	, 
										0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	, 
										0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	, 
										0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		},
									{	0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
										0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	, 
										0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	, 
										0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	, 
										0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	, 
										0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	, 
										25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	, 
										0	,	0	,	5	,	10	,	10	,	5	,	0	,	0		}};

constexpr int MIRROR[64] = {	56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
								48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	, 
								40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	, 
								32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	, 
								24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	, 
								16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	, 
								8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	, 
								0	,	1	,	2	,	3	,	4	,	5	,	6	,	7	};
							
constexpr int SCORE_VICTIM[13] = {	0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600	};

#endif // DEFINES_H
