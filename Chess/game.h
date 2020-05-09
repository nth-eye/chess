#ifndef GAME_H
#define GAME_H

#include <QtWidgets/QApplication>
#include <QQmlProperty>

#include <set>

#include "spot.h"
#include "piece.h"
#include "client.h"
#include "player.h"
#include "engine.h"

class Game : public QQuickItem {
	Q_OBJECT
public:
	explicit Game(Client *client, QQuickItem *parent = nullptr);
	
	enum GameStatus : int {REGULAR, CHECK, CHECKMATE, STALEMATE, DRAW, RESIGN};
	Q_ENUMS(GameStatus)
	enum GameType : int {OFFLINE, AI, ONLINE};
	Q_ENUMS(GameType)
	
	Q_INVOKABLE void startGame(GameType type, bool color, int sec);
	Q_INVOKABLE void endGame();
	Q_INVOKABLE void claimDraw();
	Q_INVOKABLE void promote(int type);
	Q_INVOKABLE int getSide() const {return side;}
	
	void createPieces();
	void destroyPieces();
	
	bool isSpotActive(int sq) const;
	bool isPromotion() const ;
	bool isGoing() const {return gameGoing;}
	
	void showSpots();
	void hideSpots();
	void disableSelection();
	
	void generateLegalMoves();
	void attemptMove(int move, int color);
	void graphicMove(int move);
	void sendToClient();
	void changeSide() {side = !side;}
	void pauseTimer() {timer->stop();}
	void unpauseTimer() {timer->start(100);}
	
	Engine engine;
	SearchInfo info;
	QQuickItem *depthSlider;
private:
	QTime times[2];
	QTimer *timer;
	Client *client;
	Player *players[2] = {};	// for offline both players are the same object
	
	Player localPlayer;
	OfflinePlayer offlinePlayer;
	OnlinePlayer onlinePlayer;
	AIPlayer aiPlayer;
	
	int status;
	int turn;
	int side;
	
	int from;
	int to;
	int move;
	
	MoveList moves;

	bool moveSelection = false;
	bool gameGoing = false;
    bool rotated = false;
    bool draw = false;
    
    std::multiset<Piece *, Cmp> corpses[2];
	
	Spot *spots[64] = {};			// graphical squares for move selection
	Piece *pieces[8][8] = {};		// graphical pieces
	Piece *selected;
    QFutureWatcher<int> watcher;
    QEventLoop loop;
protected:
	void mousePressEvent(QMouseEvent *event) override;
signals:
	void statusChanged(int status, bool turn);
	void timeChanged(QTime time, bool turn);
	void gameEnded();
	void drawAvailable(bool draw);
	void getPromotion();
public slots:
    void animate(Piece *p, int fromY, int fromX, int toY, int toX);
};

#endif // GAME_H
