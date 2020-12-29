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
	if ((event->delta() > 0) && (scale_ >= 80))//���Ŵ�ԭʼͼ���80��
	{
		return QGraphicsPixmapItem::wheelEvent(event);
	}
	else if ((event->delta() < 0) && (scale_ <= 0.5))//ͼ����С������Ӧ��С֮��Ͳ�������С
	{
	}
	else
	{
		qreal qrealOriginScale = scale_;
		if (event->delta() > 0)//��������ǰ����
		{
			scale_ *= 1.1;//ÿ�ηŴ�10%
		}
		else
		{
			scale_ *= 0.9;//ÿ����С10%
		}
		setScale(scale_);
		if (event->delta() > 0)
		{
			moveBy(-event->pos().x()*qrealOriginScale*0.1, -event->pos().y()*qrealOriginScale*0.1);//ʹͼƬ���ŵ�Ч��������������������ڵ�Ϊ���Ľ������ŵ�
		}
		else
		{
			moveBy(event->pos().x()*qrealOriginScale*0.1, event->pos().y()*qrealOriginScale*0.1);//ʹͼƬ���ŵ�Ч��������������������ڵ�Ϊ���Ľ������ŵ�
		}
	}
}
