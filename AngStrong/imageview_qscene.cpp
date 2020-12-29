#include "imageview_qscene.h"

ImageViewQScene::ImageViewQScene(QObject *parent /* = nullptr */)
	:QGraphicsScene(parent)
{

}

ImageViewQScene::ImageViewQScene(const QRectF &sceneRect, QObject *parent /* = nullptr */)
	:QGraphicsScene(sceneRect,parent)
{

}

ImageViewQScene::ImageViewQScene(qreal x, qreal y, qreal width, qreal height, QObject *parent /* = nullptr */)
	: QGraphicsScene(x, y, width, height, parent)
{

}

ImageViewQScene::~ImageViewQScene()
{
}

void ImageViewQScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	update();
	QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void ImageViewQScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	update();
	QGraphicsScene::mousePressEvent(mouseEvent);
}

void ImageViewQScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	update();
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void ImageViewQScene::wheelEvent(QGraphicsSceneWheelEvent * wheelEvent)
{
	QGraphicsScene::wheelEvent(wheelEvent);
}
