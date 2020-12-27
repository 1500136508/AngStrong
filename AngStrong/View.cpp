#pragma execution_character_set("utf-8")
#include "View.h"
#include <QMenu>
#include <QFileDialog>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTextStream>
#include <QPushButton>
#include <QDebug>

View::View(QWidget *pParent /* = nullptr */)
	:QGraphicsView(pParent)
	,image_scene_(std::make_shared<ImageScene>(new ImageScene()))
	,image_pix_(std::make_shared<ImagePix>(new ImagePix()))
	,image_rect_item_(std::make_shared<GraphicsRectItem>(new GraphicsRectItem()))
{
	//初始化
	image_scene_->addItem(image_pix_.get());//添加像元元素绑定到Scene
	setScene(image_scene_.get());//将QGraphicsView添加Scene;
	setMouseTracking(true);
}

View::View(QGraphicsScene *scene, QWidget *parent /* = nullptr */)
	:QGraphicsView(scene,parent)
{

}

View::~View()
{
}

void View::mouseMoveEvent(QMouseEvent * event)
{
	QGraphicsView::mouseMoveEvent(event);
}

void View::mousePressEvent(QMouseEvent * event)
{
	QGraphicsView::mousePressEvent(event);
}

void View::mouseReleaseEvent(QMouseEvent * event)
{
	QGraphicsView::mouseReleaseEvent(event);
}

void View::wheelEvent(QWheelEvent * event)
{
	return QGraphicsView::wheelEvent(event);
}

void View::paintEvent(QPaintEvent * event)
{
	return QGraphicsView::paintEvent(event);
}
