#include "imageview_qpixmap.h"
#include <QGraphicsSceneWheelEvent>

ImageViewQPixmap::ImageViewQPixmap(QGraphicsPixmapItem *parent /* = nullptr */)
	:QGraphicsPixmapItem(parent)
	,scale_(1.0f)
{
	InitializeUI();
}

ImageViewQPixmap::ImageViewQPixmap(const QPixmap &pixmap, QGraphicsPixmapItem *parent /* = nullptr */)
	: QGraphicsPixmapItem(pixmap, parent)
	,scale_(1.0f)
{
	InitializeUI();
}

ImageViewQPixmap::~ImageViewQPixmap()
{
}

void ImageViewQPixmap::SetFit(double scale)
{
	scale_ = scale;
}

float ImageViewQPixmap::GetScale() const
{
	return scale_;
}

void ImageViewQPixmap::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	return QGraphicsPixmapItem::mouseMoveEvent(event);
}

void ImageViewQPixmap::wheelEvent(QGraphicsSceneWheelEvent * event)
{
	ImageZoom(event);
	return QGraphicsPixmapItem::wheelEvent(event);
}

void ImageViewQPixmap::InitializeUI()
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsFocusable);
}

void ImageViewQPixmap::ImageZoom(QGraphicsSceneWheelEvent * event)
{
	if ((event->delta() > 0) && (scale_ >= 80))//最大放大到原始图像的80倍
	{
		return QGraphicsPixmapItem::wheelEvent(event);
	}
	else if ((event->delta() < 0) && (scale_ <= 0.5))//图像缩小到自适应大小之后就不继续缩小
	{
	}
	else
	{
		qreal qrealOriginScale = scale_;
		if (event->delta() > 0)//鼠标滚轮向前滚动
		{
			scale_ *= 1.1;//每次放大10%
		}
		else
		{
			scale_ *= 0.9;//每次缩小10%
		}
		setScale(scale_);
		if (event->delta() > 0)
		{
			moveBy(-event->pos().x()*qrealOriginScale*0.1, -event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
		}
		else
		{
			moveBy(event->pos().x()*qrealOriginScale*0.1, event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
		}
	}
}
