#ifndef PLAYER_H
#define PLAYER_H

#include <QtConcurrent>
#include <QFutureWatcher>

class Game;

class Player {
public:
	Player() {}
	virtual ~Player() = default;
	
	int getColor() const {return color;}
	void setColor(int newColor) {color = newColor;}
	virtual void passTurn() {return;}
	
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
    AIPlayer(QFutureWatcher<int> *w = nullptr, QEventLoop *l = nullptr) : watcher(w), loop(l) {}
	
	virtual void passTurn() override;
private:
    QFutureWatcher<int> *watcher;
    QEventLoop *loop;
};


class OnlinePlayer : public Player {
public:
	OnlinePlayer() {}
	
	virtual void passTurn() override;
};

#endif // PLAYER_H
