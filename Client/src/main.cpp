#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "game.h"

int main(int argc, char *argv[]) {

	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);
	QQmlApplicationEngine engine;

	QScopedPointer<Client> client(new Client);
	QScopedPointer<Game> game(new Game(client.get()));
	qmlRegisterSingletonInstance("chess.client", 1, 0, "Client", client.get());
	qmlRegisterSingletonInstance("chess.game", 1, 0, "Game", game.get());

    const QUrl url(QStringLiteral("qrc:/QML/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
					 &app, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);
	engine.load(url);
	
	QObject *root = engine.rootObjects().at(0);
	QQuickItem *boardImg = root->findChild<QQuickItem*>("boardImg");
	QQuickItem *slider = root->findChild<QQuickItem*>("depthSlider");

	game->setParentItem(boardImg);	
	game->depthSlider = slider;
	
	return app.exec();
}
