#ifndef MODEL_H
#define MODEL_H

#include <QAbstractListModel>
#include <QHostAddress>
#include <QDataStream>

struct GameItem {
	uint8_t ip[16];			// up to IPv6 128 bits, IPv4 will be mapped to IPv6
	uint16_t port;
	uint16_t colorAndTime;	// 1 bit is color, others for time in seconds
};

class Model : public QAbstractListModel {
	Q_OBJECT
public:
	explicit Model(QObject *parent = nullptr);
	
	enum { IpRole = Qt::UserRole, PortRole, ColorRole, TimeRole };
	
	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	
	// Editable:
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	QHash<int, QByteArray> roleNames() const override;
	
	// Adds:
	void clearList();
	void setList(QByteArray &newList);
	bool setItemAt(int index, const GameItem &item);
	GameItem* getItemAt(int index);
private:
	QVector<GameItem> list;
};

#endif // MODEL_H
