#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QNetworkInterface>
#include <QTcpServer>
#include <QTcpSocket>
#include <QCloseEvent>

#include <set>
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QMainWindow {
    Q_OBJECT
public:
    Server(QWidget *parent = nullptr);
    ~Server();
private:
    static const int PORT = 22869;
    Ui::Server *ui;
    QTcpServer *server;
    std::set<QTcpSocket *> peers;
    std::map<QTcpSocket *, QByteArray> games;
    const QByteArray greetings = "Hello from chess server!\r\n\r\n";
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    void newPeer();
    void newGame(QTcpSocket *client);
    void peerLeft();
    // Wnen some peer connected to existing game.
    void gameLeft(QTcpSocket *client);
    void sendAvailableGames(QTcpSocket *client);
    
    void handleRequest();
    
    void dropAllConnections();
    void stopServer();
};
#endif // SERVER_H
