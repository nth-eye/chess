#include "game.h"

Game::Game(Client *cln, QQuickItem *parent) : QQuickItem(parent), client(cln)
{
	for (int i = 0; i < 64; ++i)
		spots[i] = new Spot(i, this);
		
	setWidth(SQ_SIZE*8);
	setHeight(SQ_SIZE*8);
	setEnabled(false);
	setAcceptedMouseButtons(Qt::LeftButton);
	
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, [=](){
		times[turn] = times[turn].addMSecs(-100);
		emit timeChanged(times[turn], turn);
		if (times[turn] == QTime(0,0,0)) {
			timer->stop();
			if (players[!side] == &onlinePlayer) {
				if (players[turn] == &onlinePlayer)
					return;
				else
					client->sendAction("\x10");
			}
			endGame();
			emit statusChanged(CHECKMATE, turn);
		}
	});
	
	Player::game = this;
    aiPlayer = AIPlayer(&watcher, &loop);
    connect(&watcher, &QFutureWatcher<int>::finished, [this](){
        disableSelection();
        if (gameGoing) 
			attemptMove(watcher.result(), aiPlayer.getColor());
    });

	connect(client, &Client::updateGame, [this](int cmd, int mv, int ms) {
        disableSelection();
        switch (cmd) {
			case 0x02:
				if (gameGoing) {
					attemptMove(mv, onlinePlayer.getColor());
					times[!turn].setHMS(0, 0, 0);
					times[!turn] = times[!turn].addMSecs(ms);
					emit timeChanged(times[!turn], !turn);
				}
				timer->start(100);
				break;
			case 0x04:
				emit statusChanged(RESIGN, turn);
				endGame();
				break;
			case 0x08:
				emit statusChanged(DRAW, turn);
				endGame();
				break;
			case 0x10:
				emit statusChanged(CHECKMATE, !side);
				endGame();
				break;
        }
	});

    // QueuedConnection is necessary in case the signal finished is emitted before the loop starts
    // (if the task is already finished when setFuture is called)
    connect(&watcher, &QFutureWatcher<int>::finished, &loop, &QEventLoop::quit,  Qt::QueuedConnection);
}

void Game::startGame(GameType type, bool color, int sec) {

	gameGoing = true;
	draw = false;
	rotated = color;
	
	engine.parseFen(START_FEN);
	
    destroyPieces();
    createPieces();
	
	switch (type) {
		case OFFLINE :
			players[color] = &offlinePlayer;
			players[!color] = &offlinePlayer;
			break;
		case AI :
			info.timeSet = false;
			players[color] = &localPlayer;
			players[!color] = &aiPlayer;
			break;
		case ONLINE :
			players[color] = &localPlayer;
			players[!color] = &onlinePlayer;
			break;
	}
	
	players[color]->setColor(color);
	players[!color]->setColor(!color);

	side = color;
	turn = WHITE;
	status = REGULAR;
	
	generateLegalMoves();
	setEnabled(true);
	
	emit statusChanged(status, turn);
	emit drawAvailable(draw);
	
	times[WHITE].setHMS(0, 0, 0);
	times[BLACK].setHMS(0, 0, 0);
		
	if (sec) {
		
		times[WHITE] = times[WHITE].addSecs(sec);
		times[BLACK] = times[BLACK].addSecs(sec);
		
		emit timeChanged(times[BLACK], BLACK);
		emit timeChanged(times[WHITE], WHITE);
		
		info.timeSet = true;
		auto stopTime =	times[turn].minute()*60000+
						times[turn].second()*1000+
						times[turn].msec();
					
		if (engine.history.size() >= 80)
			info.stopTime = engine.getTime() + stopTime;
		else
			info.stopTime = engine.getTime() + stopTime / (40 - engine.history.size()/2);
		
		timer->start(100);
	}
	
	if (type == AI && side == BLACK) {
		info.depth = QQmlProperty::read(depthSlider, "value").toInt();
		info.stopped = false;
		aiPlayer.passTurn();
	}
}

void Game::endGame() {
	
	timer->stop();
	
    gameGoing = false;
	setEnabled(false);
	disableSelection();
	
	if (players[turn] == &aiPlayer) {
		QMutex mutex;
		mutex.lock();
		info.stopped = true;
		mutex.unlock();
	}
	
	emit gameEnded();
}

void Game::promote(int type) {
	move &= ~(0xf << 20);
	move |= (type<< 20);
	disableSelection();
	attemptMove(move, side);
}

void Game::claimDraw() {
	status = DRAW;
	endGame();
	emit statusChanged(status, turn);
}

void Game::mousePressEvent(QMouseEvent *e) {

	int y = e->y()/SQ_SIZE;
	int x = e->x()/SQ_SIZE;
	int sq;
	
	if (rotated)
		sq = engine.fr2sq(7-x, y);
	else
		sq = engine.fr2sq(x, 7-y);
		
	Piece *p = pieces[y][x];
	
	if (moveSelection) {
			
		to = sq;
		
		if (isSpotActive(Engine::SQ64(sq))) {
		
			move = engine.findMove(from, to, moves);
		
			if (move & FLAG_PR) {
				setEnabled(false);
				emit getPromotion();
				return;
			}
			attemptMove(move, side);
		}
		
		disableSelection();
		
    } else if (turn == side && p && p->color == side) {
	
		from = sq;
		selected = p;
		
		moveSelection = true;
		showSpots();
		selected->setOpacity(0.4);
	}
}

void Game::attemptMove(int m, int color) {

	if (turn != color) 
		return;
	
	engine.makeMove(m);
	graphicMove(m);
	turn = !turn;
    
    generateLegalMoves();
	
	status = REGULAR;
	bool check = engine.isAttacked(engine.kingSq[turn], !turn);
	
	if (!moves.size()) {
		if (check)
			status = CHECKMATE;
		else 
			status = STALEMATE;
		endGame();
	} else if (engine.isNEmptyMove(150) || engine.isNFoldRepetitiom(5)) {
		status = DRAW;
		endGame();
	} else if (check)
		status = CHECK;
		
	if (engine.isNEmptyMove(100) || engine.isNFoldRepetitiom(3))
		draw = true;
	else
		draw = false;
		
	emit statusChanged(status, turn);
	emit drawAvailable(draw);
	
	info.depth = QQmlProperty::read(depthSlider, "value").toInt();
	
	auto stopTime =	times[turn].minute()*60000+
					times[turn].second()*1000+
					times[turn].msec();
					
	if (engine.history.size() >= 80)
		info.stopTime = engine.getTime() + stopTime;
	else
		info.stopTime = engine.getTime() + stopTime / (40 - engine.history.size()/2);
	
    players[turn]->passTurn();
}

void Game::generateLegalMoves() {

	moves.clear();
	for (const auto &it : engine.generateAllMoves()) {
		if (!engine.makeMove(it.move))
			continue;
		engine.undoMove();
		moves.emplace_back(it);
	}
}

bool Game::isSpotActive(int sq) const {
	if (rotated)
		sq = 63 - sq;
	return spots[sq]->isVisible();
}

bool Game::isPromotion() const {
	if ((selected->type == wP && Engine::RANKS[to] == RANK_8) || 
		(selected->type == bP && Engine::RANKS[to] == RANK_1)) {
		return true;
	}
	return false;
}

void Game::sendToClient() {
    client->sendMove(move, times[!turn].minute()*60000+
						   times[!turn].second()*1000+
						   times[!turn].msec());
}

void Game::createPieces() {

	int sq;
	int piece;
	int y;
	int x;
    
    for (int rank = RANK_1; rank < RANK_NONE; ++rank) {
		for (int file = FILE_A; file < FILE_NONE; ++file) {
			sq = engine.fr2sq(file, rank);
			y = rotated ? rank : RANK_8-rank;
			x = rotated ? FILE_H-file : file;
			if ((piece = engine.pieces[sq]))
				pieces[y][x] = new Piece(y, x, piece, PIECE_COL[piece], this);
		}
	}
}

void Game::destroyPieces() {
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (pieces[i][j]) {
				delete pieces[i][j];
				pieces[i][j] = nullptr;
			}
		}
	}
	for (int i = 0; i < 2; ++i) {
		for (auto it : corpses[i])
			delete it;
		corpses[i].clear();
	}
}

void Game::showSpots() {

	int sq;
	int m;
	
	for (const auto &it : moves) {
		m = it.move;
		if (FROM(m) == from) {
			sq = Engine::SQ64(TO(m));
			if (rotated) 
				sq = 63 - sq;
			spots[sq]->setVisible(true);
		}
	}
}

void Game::hideSpots() {
	for (int i = 0; i < 64; ++i)
		spots[i]->setVisible(false);
}

void Game::disableSelection() {
	if (moveSelection) {
		moveSelection = false;
		hideSpots();
		selected->setOpacity(1);
	}
}

void Game::graphicMove(int m) {

	int f = Engine::SQ64(FROM(m));	// from
	int t = Engine::SQ64(TO(m));	// to
	
	if (rotated) {
		f = 63 - f;
		t = 63 - t;
	}
	
	int fromY = 7 - (f / 8);
	int fromX = f % 8;
	
	int toY = 7 - (t / 8);
	int toX = t % 8;
	
    if (m & FLAG_CA) {
		int rookX = fromX > toX ? 0 : 7;
        int x = (fromX > toX ? 3 : 5)-rotated;
        pieces[toY][x] = pieces[toY][rookX];
        pieces[toY][rookX] = nullptr;
		pieces[toY][x]->setX(x*SQ_SIZE);
		animate(pieces[toY][x], toY, rookX, toY, x);
	}
	
	if (m & FLAG_PR) {
		pieces[fromY][fromX]->type = PROMOTED(m);
		pieces[fromY][fromX]->update();
	}
	
	if (m & FLAG_CP) {
		Piece *trg = pieces[toY][toX];
		if (m & FLAG_EP) {
			int y = toY > fromY ? toY-1 : toY+1;
			trg = pieces[y][toX];
			pieces[y][toX] = nullptr;
		}
		int c = trg->color;
		corpses[c].insert(trg);
		int i = 0;
        int idx = 0;
        int y = trg->y()/SQ_SIZE;
        int x = trg->x()/SQ_SIZE;
		for (auto it : corpses[c]) {
            if (it == trg)
                idx = i;
            else {
                it->setY(i%8 * SQ_SIZE);
                it->setX(540 + 120*c + i/8*SQ_SIZE);
            }
			++i;
		}
		trg->setOpacity(0.5);
        animate(trg, y, x, idx%8, 9 + 2*c + idx/8);
	}
	
	pieces[toY][toX] = pieces[fromY][fromX];
	pieces[fromY][fromX] = nullptr;

    animate(pieces[toY][toX], fromY, fromX, toY, toX);
}

void Game::animate(Piece *p, int fromY, int fromX, int toY, int toX) {

	p->setZ(1);

    auto animX = new QPropertyAnimation(p, "x", this);
    animX->setEasingCurve(QEasingCurve::OutBounce);
    animX->setDuration(500);
    animX->setStartValue(fromX*SQ_SIZE);
    animX->setEndValue(toX*SQ_SIZE);

    auto animY = new QPropertyAnimation(p, "y", this);
    animY->setEasingCurve(QEasingCurve::OutBounce);
    animY->setDuration(500);
    animY->setStartValue(fromY*SQ_SIZE);
    animY->setEndValue(toY*SQ_SIZE);

    animX->start(QAbstractAnimation::DeleteWhenStopped);
    animY->start(QAbstractAnimation::DeleteWhenStopped);

    p->setZ(0);
}
