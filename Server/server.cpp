#include "server.h"
#include "ui_server.h"

Server::Server(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::Server)
{
	ui->setupUi(this);
	
	server = new QTcpServer(this);
	
	if (!server->listen(QHostAddress::Any, PORT)) {
        ui->statusLabel->setText("Server couldn't start: "+server->errorString());
	} else {
		QString ipAddress;
		for (const auto &it : QNetworkInterface::allAddresses()) {
			if (it != QHostAddress::LocalHost) {
				ipAddress = it.toString();
				break;
			}
		}
		if (ipAddress.isEmpty())
			ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
			
		ui->statusLabel->setText(tr("Server is running on\nIP:\t%1\nport:\t%2\n")
									.arg(ipAddress).arg(server->serverPort()));
	}
	
	connect(ui->restartButton, &QPushButton::clicked, this, &Server::dropAllConnections);
	connect(ui->stopButton, &QPushButton::clicked, this, &Server::stopServer);
	connect(server, &QTcpServer::newConnection, this, &Server::newPeer);
}

Server::~Server() {
	delete ui;
}

void Server::newPeer() {
	
	QTcpSocket *client = server->nextPendingConnection();
	
    connect(client, &QAbstractSocket::disconnected, this, &Server::peerLeft);
    connect(client, &QIODevice::readyRead, this, &Server::handleRequest);
    
	peers.insert(client);
	
	QString str = client->peerAddress().toString() + " on port " + QString::number(client->peerPort());
	ui->peerList->addItem(str);
}


void Server::peerLeft() {

	QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
	
	if (games.count(client)) gameLeft(client);
	
	QString str = client->peerAddress().toString() + " on port " + QString::number(client->peerPort());
    if (auto *item = ui->peerList->findItems(str, Qt::MatchExactly).at(0))
        delete item;
        
    peers.erase(client);
    client->deleteLater();
}

void Server::newGame(QTcpSocket *client) {

	QByteArray other = client->read(4);
	games.insert({client, other});
				 
	uint16_t port = static_cast<uint8_t>(games[client][1]) | (games[client][0] << 8);
	uint16_t time = static_cast<uint8_t>(games[client][3]) | ((games[client][2] & 0x7F) << 8);
				   
	QString str = client->peerAddress().toString() + 
				  " : " + QString::number(port) + 
				  " : " + ((games[client][2] & 0x80) ? "white : " : "black : ") + 
				   QString::number(time);
				   
	ui->gameList->addItem(str);
	
	client->readAll();
}

void Server::gameLeft(QTcpSocket *client) {
	
	uint16_t port = static_cast<uint8_t>(games[client][1]) | (games[client][0] << 8);
	uint16_t time = static_cast<uint8_t>(games[client][3]) | ((games[client][2] & 0x7F) << 8);
				   
	QString str = client->peerAddress().toString() + 
				  " : " + QString::number(port) + 
				  " : " + ((games[client][2] & 0x80) ? "white : " : "black : ") + 
				   QString::number(time);

    if (auto *item = ui->gameList->findItems(str, Qt::MatchExactly).at(0))
        delete item;
    
    games.erase(client);
}

void Server::sendAvailableGames(QTcpSocket *client) {

	client->readAll();
	
	QByteArray block;
    
	for (auto const &[key, val] : games) {
		auto addr = key->peerAddress().toIPv6Address();
		for (int i = 0; i < 16; ++i)
			block.append(addr[i]);
		block.append(val);
	}
    client->write(block);
}

void Server::handleRequest() {
	
	QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
	char request;
	client->read(&request, 1);
	
	switch (request) {
		case 0x01:
			sendAvailableGames(client);
			break;
		case 0x02:
			newGame(client);
			break;
		case 0x04:
			gameLeft(client);
			break;
	}
}

void Server::dropAllConnections() {
	const auto allPeers(peers);
	for (const auto it : allPeers)
		it->disconnectFromHost();
}

void Server::stopServer() {
	dropAllConnections();
	close();
}

void Server::closeEvent(QCloseEvent *event) {
	stopServer();
}

