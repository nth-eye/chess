#include "spot.h"

Spot::Spot(int sq, QQuickItem *parent)
	: QQuickItem(parent)
{
	setWidth(SQ_SIZE);
	setHeight(SQ_SIZE);
	setY((7 - (sq / 8))*SQ_SIZE);
	setX((sq % 8)*SQ_SIZE);
	setOpacity(0.4);
	
	setEnabled(false);
	setVisible(false);
	setFlag(ItemHasContents, true);
}

QSGNode *Spot::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) {
	QSGRectangleNode *node = static_cast<QSGRectangleNode *>(oldNode);
	if (!node) {
		node = window()->createRectangleNode();
		node->setColor("#aa0000");
		node->setRect(boundingRect());
	}
	return node;
}
