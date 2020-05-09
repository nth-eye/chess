#include "engine.h"

int Engine::fr2sq(const int f, const int r) const {
	return (21+f) + (r*10);
}

U64 Engine::getTime() const {
	using namespace std::chrono; 
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

int Engine::countBits(U64 bb) const {
	int r;
	for (r = 0; bb; r++, bb &= bb-1);
	return r;
}

int Engine::popBit(U64 *bb) const {
	U64 b = *bb ^ (*bb-1);
	unsigned int fold = static_cast<unsigned>((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb-1);
	return BIT_TABLE[(fold * 0x783a9b23) >> 26];
}

U64 Engine::generatePosKey() const {

	U64 finalKey = 0;
	int piece = EMPTY;
	
	for (int sq = 0; sq < BRD_SQ_NUM; ++sq) {
		piece = pieces[sq];
		if (piece != NO_SQ && piece != EMPTY && piece != OFFBOARD) {
			assert(piece >= wP && piece <= bK);
			finalKey ^= PIECE_KEYS[piece][sq];
		}
	}
	
	if (side == WHITE)
		finalKey ^= sideKey;
	
	if (enPass != NO_SQ) {
		assert(enPass >= 0 && enPass < BRD_SQ_NUM);
		finalKey ^= PIECE_KEYS[EMPTY][enPass];
	}
	
	assert(castlePerm >= 0 && castlePerm <= 15);
	
	finalKey ^= CASTLE_KEYS[castlePerm];
	
	return finalKey;
}
