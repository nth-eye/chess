#include "piece.h"

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

QSGNode *Piece::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *) {
	QSGImageNode *node = static_cast<QSGImageNode *>(oldNode);
	if (!node) 
		node = window()->createImageNode();
	QString str(":/images/");
	switch (type) {
		case wP:	str += "white_pawn";	break;
		case wN:	str += "white_knight";	break;
		case wB:	str += "white_bishop";	break;
		case wR:	str += "white_rook";	break;
		case wQ:	str += "white_queen";	break;
		case wK:	str += "white_king";	break;
		case bP:	str += "black_pawn";	break;
		case bN:	str += "black_knight";	break;
		case bB:	str += "black_bishop";	break;
		case bR:	str += "black_rook";	break;
		case bQ:	str += "black_queen";	break;
		case bK:	str += "black_king";	break;
	}
	QSGTexture *texture = window()->createTextureFromImage(QImage(str));
	node->setTexture(texture);
	node->setOwnsTexture(true);
	node->setRect(boundingRect());
	return node;
}
