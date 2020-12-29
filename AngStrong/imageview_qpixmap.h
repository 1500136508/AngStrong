#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>

class ImageViewQPixmap : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
public:
	explicit ImageViewQPixmap(QGraphicsPixmapItem *parent = nullptr);
	explicit ImageViewQPixmap(const QPixmap &pixmap, QGraphicsPixmapItem *parent = nullptr);
	virtual ~ImageViewQPixmap();

	void SetFit(double scale = 1.0f);
	float GetScale()const;
protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void wheelEvent(QGraphicsSceneWheelEvent *event);
private:
	void InitializeUI();
	void ImageZoom(QGraphicsSceneWheelEvent * event);
	double scale_;
};

