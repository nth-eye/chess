#include "player.h"
#include "game.h"

Game* Player::game = nullptr;

void OfflinePlayer::passTurn()
{
	game->changeSide();
}

void AIPlayer::passTurn()
{
    if (game->isGoing()) {
        QFuture<chess::Move> future =
                QtConcurrent::run(game->engine, &Engine::search_position, &game->info);
        watcher->setFuture(future);
    }
}

void OnlinePlayer::passTurn()
{
	game->sendToClient();
}

