#include <QHostAddress>
#include <QDataStream>
#include <QTime>

#include "model.h"

Model::Model(QObject *parent) : QAbstractListModel(parent) {}

int Model::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || list.empty())
        return 0;
    
    return list.size();
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || list.empty())
        return QVariant();
    
    const GameItem item = list.at(index.row());
    
    switch (role) {
        case IpRole:	return QVariant(QHostAddress(item.ip).toString());
        case PortRole:	return QVariant(QString::number(item.port));
        case ColorRole:	return QVariant(item.colorAndTime & 0x8000 ? "black" : "white");
        case TimeRole:	return QVariant(item.colorAndTime & 0x0FFF ?
                                        QDateTime::fromTime_t(item.colorAndTime & 0x0FFF ).toString("mm:ss") :
                                        "No limit");
    }

    return QVariant();
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    return Qt::ItemIsEditable; // FIXME: Implement me!
}

QHash<int, QByteArray> Model::roleNames() const
{
    QHash<int, QByteArray> names;
    names[IpRole] = "IP";
    names[PortRole] = "port";
    names[ColorRole] = "colorCode";
    names[TimeRole] = "time";
    return names;
}

void Model::clearList()
{
    beginResetModel();
    list.clear();
    endResetModel();
}

void Model::setList(QByteArray &newList)
{
    beginResetModel();
    
    int size = newList.size()/sizeof(GameItem);
    
    QDataStream out(newList);
    out.setByteOrder(QDataStream::BigEndian);

    list.clear();
    list.resize(size);
    
    for (int i = 0; i < size; ++i) {	
        for (int j = 0; j < 16; j++)
            out >> list[i].ip[j];
        out >> list[i].port;
        out >> list[i].colorAndTime;
    }

    endResetModel();
}

bool Model::setItemAt(int index, const GameItem &item)
{
    if (index < 0 || index >= list.size())
        return false;
    list[index] = item;
    return true;
}

GameItem* Model::getItemAt(int index)
{
    return &list[index];
}
