#pragma once
#include <QGraphicsScene>

class ImageViewQScene : public QGraphicsScene
{
	Q_OBJECT
public:
	ImageViewQScene(QObject *parent = nullptr);
	ImageViewQScene(const QRectF &sceneRect, QObject *parent = nullptr);
	ImageViewQScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = nullptr);
	virtual ~ImageViewQScene();

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);
};

