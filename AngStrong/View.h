#pragma once
#include <QGraphicsView>
#include <vector>
#include "imagescene.h"
#include "imagepix.h"
#include "graphicsrectitem.h"

class View : public QGraphicsView
{
	Q_OBJECT
public:
	View(QWidget *pParent = nullptr);
	View(QGraphicsScene *scene, QWidget *parent = nullptr);
	virtual ~View();

protected:
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent*event) override;
	void paintEvent(QPaintEvent *event) override;
private:
	std::shared_ptr<ImageScene>			image_scene_;
	std::shared_ptr<ImagePix>			image_pix_;
	std::shared_ptr<GraphicsRectItem>   image_rect_item_;
};

