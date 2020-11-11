#ifndef PLAYER_H
#define PLAYER_H

#include <QtConcurrent>
#include <QFutureWatcher>

#include "engine.h"

class Game;

class Player {
public:
	Player() {}
	virtual ~Player() = default;
	
    int getColor() const { return color; }
    void setColor(int color_) { color = color_; }
    virtual void passTurn() { return; }
	
	static Game *game;
protected:
	int color;
};


class OfflinePlayer : public Player {
public:
	OfflinePlayer() {}
	
    virtual void passTurn() override;
};


class AIPlayer : public Player{
public:
    AIPlayer(QFutureWatcher<chess::Move> *watcher_ = nullptr,
             QEventLoop *loop_ = nullptr)
        : watcher(watcher_), loop(loop_)
    {}
	
    virtual void passTurn() override;
private:
    QFutureWatcher<chess::Move> *watcher;
    QEventLoop *loop;
};


class OnlinePlayer : public Player {
public:
	OnlinePlayer() {}
	
    virtual void passTurn() override;
};

#endif // PLAYER_H
