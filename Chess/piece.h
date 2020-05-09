#ifndef PIECE_H
#define PIECE_H

#include <QQuickItem>
#include <QQuickWindow>
#include <QSGImageNode>
#include <QImage>

#include "defines.h"

const int SQ_SIZE = 60;

class Piece : public QQuickItem {
	Q_OBJECT
public:
	Piece(int y, int x, int t, int c, QQuickItem *parent = nullptr);
	
	int type;
	int color;
protected:
	QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
};

struct Cmp { // Piece type comparator for displaying dead pieces in sorted order
	bool operator()(const Piece *lhs, const Piece *rhs) const { 
		return lhs->type < rhs->type; 
	} 
};

#endif // PIECE_H
