#include "engine.h"

void Engine::storePVMove(const int move) {

	size_t i = posKey % pvTable.size();
	assert(i >= 0 && i < pvTable.size());
	
	pvTable[i].move = move;
	pvTable[i].posKey = posKey;
}

int Engine::probePVTable() {
	
	size_t i = posKey % pvTable.size();
	assert(i >= 0 && i < pvTable.size());
	
	if (pvTable[i].posKey == posKey)
		return pvTable[i].move;
	return NOMOVE;
}

int Engine::getPVLine(const int depth) {
	
	assert(depth < MAX_DEPTH);
	
	int move = probePVTable();
	int count = 0;
	
	while (move != NOMOVE && count < depth) {
	
		assert(count < MAX_DEPTH);
		
		if (moveExists(move)) {
			makeMove(move);
			PVArray[count++] = move;
		} else 
			break;
			
		move = probePVTable();
	}
	
	while (ply > 0)
		undoMove();
		
	return count;
}
