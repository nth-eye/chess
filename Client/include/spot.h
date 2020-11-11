#ifndef SPOT_H
#define SPOT_H

#include <QSGRectangleNode>
#include "piece.h"

class Spot : public QQuickItem {
    Q_OBJECT
public:
    Spot(int sq, QQuickItem *parent = nullptr);
protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
};

#endif // SPOT_H
