#include "player.h"
#include "game.h"

Game* Player::game = nullptr;

void OfflinePlayer::passTurn() {
	game->changeSide();
}

void AIPlayer::passTurn() {
	if (game->isGoing()) {
        QFuture<int> future = QtConcurrent::run(game->engine, &Engine::searchPosition, &game->info);
        watcher->setFuture(future);
	}
}

void OnlinePlayer::passTurn() {
	game->sendToClient();
}

