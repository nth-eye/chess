#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>
#include "model.h"

class Client : public QTcpServer {
	Q_OBJECT
	Q_PROPERTY(Model *model READ getModel NOTIFY modelChanged)
public:
	explicit Client(QObject *parent = nullptr);
	
	Q_INVOKABLE void connectToServer(QString serverIP);
	Q_INVOKABLE void disconnectFromServer();
	Q_INVOKABLE void connectToGame(int index);
	Q_INVOKABLE void disconnectFromGame();
	
	Q_INVOKABLE void updateGames();
	Q_INVOKABLE void createGame(bool color, qint16 time);
	Q_INVOKABLE void deleteGame();
	
	Q_INVOKABLE QString getOpponentIp() const {return playerSocket->peerAddress().toString();}
	Q_INVOKABLE bool getSide() const {return playerSide;}
	Q_INVOKABLE int getTime() const {return time;}
	
	Q_INVOKABLE void sendAction(QString msg);
	Q_INVOKABLE void sendMessage(QString msg);
				void sendMove(int move, int ms);
	
	Model *getModel() const {return model;}
private:
	static const int PORT = 22869;	// server port
	QTcpSocket *serverSocket;
	QTcpSocket *playerSocket;
	Model *model;
	bool playerSide;
	int time;
protected:
	virtual void incomingConnection(qintptr descriptor) override;	// when someone connects to our game
signals:
	void modelChanged(Model *model);	
	
	void connectedToServer();
	void disconnectedFromServer();
	void connectedToPlayer();		// when we are connecting to existing game
	void disconnectedFromPlayer();
	
	void updateDialogue(QString message);
	void updateGame(int cmd, int move, int sec);
	void importantMsg(QString message);
	void drawOffer();
	
	void serverError(QString errorString);
	void playerError(QString errorString);
public slots:
	void handleServerReply();
	void handlePlayerReply();
};

#endif // CLIENT_H
