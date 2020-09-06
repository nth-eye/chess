#include "client.h"

Client::Client(QObject *parent) 
	: QTcpServer(parent)
{
	model = new Model(this);
	
	connect(this, &QTcpServer::acceptError, 
		[this]() {emit playerError(errorString());}
	);
	
	serverSocket = new QTcpSocket(this);
	connect(serverSocket, &QAbstractSocket::connected, this, &Client::connectedToServer);
	connect(serverSocket, &QAbstractSocket::disconnected, this, &Client::disconnectedFromServer);
	connect(serverSocket, &QAbstractSocket::disconnected, [this]() {close();});
	connect(serverSocket, &QAbstractSocket::readyRead, this, &Client::handleServerReply);
	connect(serverSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), 
		[this]() {emit serverError(serverSocket->errorString());}
	);
	
	playerSocket = new QTcpSocket(this);
	connect(playerSocket, &QAbstractSocket::connected, this, &Client::connectedToPlayer);
	connect(playerSocket, &QAbstractSocket::disconnected, this, &Client::disconnectedFromPlayer);
    connect(playerSocket, &QAbstractSocket::readyRead, this, &Client::handlePlayerReply);
    connect(playerSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), 
		[this]() {emit playerError(playerSocket->errorString());}
	);
}

void Client::connectToServer(QString serverIP)
{
	serverSocket->connectToHost(serverIP, PORT);
}

void Client::disconnectFromServer()
{
	serverSocket->disconnectFromHost();
}

void Client::updateGames()
{
	model->clearList();
	serverSocket->write("\x01");
}

void Client::createGame(bool color, qint16 sec)
{
	if (!listen(QHostAddress::Any)) {
        emit playerError(errorString());
	} else {
		playerSide = color;
		time = sec;
		
		QByteArray block = "\x02";
		QDataStream out(&block, QIODevice::Append);
		
		uint16_t port = serverPort();
		uint16_t colorAndTime = !color;
		
		colorAndTime <<= 15;
		colorAndTime += sec;
		
		out << port << colorAndTime;
		serverSocket->write(block);
	}
}

void Client::deleteGame()
{
	close();
	serverSocket->write("\x04");
}

void Client::handleServerReply()
{
	QByteArray block = serverSocket->readAll();
	model->setList(block);
}

void Client::incomingConnection(qintptr descriptor)
{
    playerSocket->setSocketDescriptor(descriptor);
    deleteGame();
    emit connectedToPlayer();
}

void Client::connectToGame(int index)
{
    GameItem *item = model->getItemAt(index);
	playerSide = item->colorAndTime & 0x8000;
	time = item->colorAndTime & 0x0FFF;
    playerSocket->connectToHost(QHostAddress(item->ip), item->port);
}

void Client::disconnectFromGame()
{
	playerSocket->disconnectFromHost();
}

void Client::handlePlayerReply()
{
	char request;
	playerSocket->read(&request, 1);
	QByteArray block = playerSocket->readAll();
	
	int move = 0;
	int ms = 0;
	QDataStream out(block);
	out.setByteOrder(QDataStream::BigEndian);
	
	switch (request) {
		case 0x01:	//new chat message
			emit updateDialogue(QString(block));
			break;
		case 0x02:	//make move
			out >> move;
			out >> ms;
			emit updateGame(0x02, move, ms);
			break;
		case 0x04:	//resigned
			emit updateGame(0x04, 0, 0);
			emit importantMsg("OPPONENT RESIGNED - YOU WON");
			break;
		case 0x08:	//claims draw
			emit updateGame(0x08, 0, 0);
			emit importantMsg("OPPONENT CHOSE A DRAW");
			break;
		case 0x0f:	//offers draw
			emit drawOffer();
			break;
		case 0x10:	//confirm game end
			emit updateGame(0x10, 0, 0);
			break;
	}
}

void Client::sendMessage(QString msg)
{
	QByteArray block = "\x01";
	block.append(msg);
	playerSocket->write(block);
}

void Client::sendMove(int move, int ms)
{
	QByteArray block = "\x02";
	QDataStream out(&block, QIODevice::Append);
	out << move << ms;
	playerSocket->write(block);
}

void Client::sendAction(QString msg)
{
	playerSocket->write(msg.toUtf8());
}
