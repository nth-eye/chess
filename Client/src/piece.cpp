#include <QQuickWindow>
#include <QSGImageNode>

#include "piece.h"
#include "engine.h"

Piece::Piece(int y, int x, int t, int c, QQuickItem *parent)
    : QQuickItem(parent), type(t), color(c)
{
    setWidth(SQ_SIZE);
    setHeight(SQ_SIZE);
    setX(x*SQ_SIZE);
    setY(y*SQ_SIZE);
    
    setEnabled(false);
    setFlag(ItemHasContents, true);
}

QSGNode *Piece::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *)
{
    QSGImageNode *node = static_cast<QSGImageNode *>(oldNode);
    if (!node) 
        node = window()->createImageNode();
    QString str(":/images/");

    using namespace chess;

    switch (type) {
        case P:	str += "pawn";      break;
        case N:	str += "knight";	break;
        case B:	str += "bishop";	break;
        case R:	str += "rook";      break;
        case Q:	str += "queen";     break;
        case K:	str += "king";      break;

    }
    if (color == BLACK)
        str += "_2";

    str += ".png";

    QSGTexture *texture = window()->createTextureFromImage(QImage(str));
    node->setTexture(texture);
    node->setOwnsTexture(true);
    node->setRect(boundingRect());
    return node;
}
