#include "game.h"

using namespace chess;

Game::Game(Client *cln, QQuickItem *parent) : QQuickItem(parent), client(cln)
{
	for (int i = 0; i < 64; ++i)
        spots[i] = new Spot(i, this);
		
	setWidth(SQ_SIZE*8);
	setHeight(SQ_SIZE*8);
	setEnabled(false);
	setAcceptedMouseButtons(Qt::LeftButton);
	
	timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]()
    {
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
    connect(&watcher, &QFutureWatcher<Move>::finished, [this](){
        disableSelection();
        if (gameGoing) 
			attemptMove(watcher.result(), aiPlayer.getColor());
    });

    connect(client, &Client::updateGame, [this](int cmd, int mv, int ms) {
        disableSelection();
        switch (cmd) {
			case 0x02:
				if (gameGoing) {
                    attemptMove(Move(mv), onlinePlayer.getColor());
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

    chess::init_magic();
}

void Game::startGame(GameType type, bool color, int sec)
{
	gameGoing = true;
	draw = false;
	rotated = color;
	
    engine.reset();
	
    destroyPieces();
    createPieces();
	
	switch (type) {
		case OFFLINE :
			players[color] = &offlinePlayer;
			players[!color] = &offlinePlayer;
			break;
		case AI :
            info.time_set = false;
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
	
    moves = engine.legal_moves();
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
		
        info.time_set = true;
		auto stopTime =	times[turn].minute()*60000+
						times[turn].second()*1000+
						times[turn].msec();
					
        if (engine.size() >= 80)
            info.stop_time = engine.get_time() + stopTime;
		else
            info.stop_time = engine.get_time() + stopTime / (40 - engine.size()/2);
		
		timer->start(100);
	}
	
	if (type == AI && side == BLACK) {
		info.depth = QQmlProperty::read(depthSlider, "value").toInt();
		info.stopped = false;
		aiPlayer.passTurn();
    }
}

void Game::endGame()
{
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

void Game::promote(int type)
{
    move.set_flags((uint16_t)type);
	disableSelection();
	attemptMove(move, side);
}

void Game::claimDraw()
{
	status = DRAW;
	endGame();
	emit statusChanged(status, turn);
}

void Game::mousePressEvent(QMouseEvent *e)
{

	int y = e->y()/SQ_SIZE;
	int x = e->x()/SQ_SIZE;
	int sq;
	
	if (rotated)
        sq = chess::rf2sq(y, 7-x);
	else
        sq = chess::rf2sq(7-y, x);
		
	Piece *p = pieces[y][x];
	
	if (moveSelection) {
			
		to = sq;
		
        if (isSpotActive(sq)) {
		
            move = findMove();
		
            if (move & Move::N_PROM) {
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

Move Game::findMove() const
{
    for (const auto move : moves) {
        if (move.from() == from && move.to() == to)
            return move;
    }
    return NO_MOVE;
}

void Game::attemptMove(Move m, int color)
{
	if (turn != color) 
		return;
	
    engine.act(m);
	graphicMove(m);
	turn = !turn;
    status = REGULAR;

    moves = engine.legal_moves();

    if (!moves.size()) {
        if (engine.check())
            status = CHECKMATE;
        else
            status = STALEMATE;
        endGame();
    } else if (engine.check())
        status = CHECK;

    if (engine.draw_available())
        draw = true;
    else
        draw = false;
		
	emit statusChanged(status, turn);
	emit drawAvailable(draw);
	
	info.depth = QQmlProperty::read(depthSlider, "value").toInt();
	
	auto stopTime =	times[turn].minute()*60000+
					times[turn].second()*1000+
					times[turn].msec();
					
    if (engine.size() >= 80)
        info.stop_time = engine.get_time() + stopTime;
	else
        info.stop_time = engine.get_time() + stopTime / (40 - engine.size()/2);
	
    players[turn]->passTurn();
}

bool Game::isSpotActive(int sq) const
{
	if (rotated)
		sq = 63 - sq;
	return spots[sq]->isVisible();
}

void Game::sendToClient()
{
    client->sendMove(move, times[!turn].minute()*60000+
						   times[!turn].second()*1000+
						   times[!turn].msec());
}

void Game::createPieces()
{
	int sq;
	int y;
	int x;

    for (auto [type, color, rank, file] : engine.pieces_with_positions()) {
        sq = chess::rf2sq(rank, file);
        y = rotated ? rank : Board::RANK_8-rank;
        x = rotated ? Board::FILE_H-file : file;
        pieces[y][x] = new Piece(y, x, type, color, this);
    }
}

void Game::destroyPieces()
{
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

void Game::showSpots()
{
	int sq;
	
    for (const auto move : engine.legal_moves()) {
        if (move.from() == from) {
            sq = move.to();
			if (rotated) 
				sq = 63 - sq;
			spots[sq]->setVisible(true);
		}
	}
}

void Game::hideSpots()
{
	for (int i = 0; i < 64; ++i)
		spots[i]->setVisible(false);
}

void Game::disableSelection()
{
	if (moveSelection) {
		moveSelection = false;
		hideSpots();
		selected->setOpacity(1);
	}
}

void Game::graphicMove(Move m)
{
    int f = m.from();
    int t = m.to();
    int flags = m.flags();
	
	if (rotated) {
		f = 63 - f;
		t = 63 - t;
	}
	
	int fromY = 7 - (f / 8);
	int fromX = f & 7;
	
	int toY = 7 - (t / 8);
	int toX = t & 7;
	
    if (flags == Move::KCASTLE || flags == Move::QCASTLE) {
		int rookX = fromX > toX ? 0 : 7;
        int x = (fromX > toX ? 3 : 5)-rotated;
        pieces[toY][x] = pieces[toY][rookX];
        pieces[toY][rookX] = nullptr;
        pieces[toY][x]->setX(x * SQ_SIZE);
		animate(pieces[toY][x], toY, rookX, toY, x);
	}
	
    if (flags & Move::N_PROM) {
        pieces[fromY][fromX]->type = flags;
		pieces[fromY][fromX]->update();
	}

    Piece *trg = pieces[toY][toX];

    if (flags == Move::ENPASS) {
        int y = toY > fromY ? toY-1 : toY+1;
        trg = pieces[y][toX];
        pieces[y][toX] = nullptr;
    }
	
    if (trg) {
		int c = trg->color;
		corpses[c].insert(trg);
		int i = 0;
        int idx = 0;
        int y = trg->y() / SQ_SIZE;
        int x = trg->x() / SQ_SIZE;
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

void Game::animate(Piece *p, int fromY, int fromX, int toY, int toX)
{
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
