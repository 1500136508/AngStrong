#include "imageview_qview.h"

ImageViewQView::ImageViewQView(QWidget *pParent /* = nullptr */)
	:QGraphicsView(pParent)
	,imageview_qscene_(std::make_shared<ImageViewQScene>(new ImageViewQScene()))
	,imageview_qpix_(std::make_shared<ImageViewQPixmap>(new ImageViewQPixmap()))
	,imageview_rect_item_(std::make_shared<GraphicsRectItem>(new GraphicsRectItem()))
{
	InitializeUI();
}

ImageViewQView::ImageViewQView(QGraphicsScene *scene, QWidget *parent /* = nullptr */)
	: QGraphicsView(scene, parent)
	, imageview_qscene_(std::make_shared<ImageViewQScene>(scene))
	, imageview_qpix_(std::make_shared<ImageViewQPixmap>(new ImageViewQPixmap()))
	, imageview_rect_item_(std::make_shared<GraphicsRectItem>(new GraphicsRectItem()))
{
	InitializeUI();
}

ImageViewQView::~ImageViewQView()
{
}

void ImageViewQView::mouseMoveEvent(QMouseEvent * event)
{
	QGraphicsView::mouseMoveEvent(event);
}

void ImageViewQView::mousePressEvent(QMouseEvent * event)
{
	QGraphicsView::mousePressEvent(event);
}

void ImageViewQView::mouseReleaseEvent(QMouseEvent * event)
{
	QGraphicsView::mouseReleaseEvent(event);
}

void ImageViewQView::wheelEvent(QWheelEvent * event)
{
	return QGraphicsView::wheelEvent(event);
}

void ImageViewQView::paintEvent(QPaintEvent * event)
{
	return QGraphicsView::paintEvent(event);
}

void ImageViewQView::InitializeUI()
{
	imageview_qscene_->addItem(imageview_qpix_.get());//添加像元元素绑定到Scene
	setScene(imageview_qscene_.get());//将QGraphicsView添加Scene;
	setMouseTracking(true);
}

void ImageViewQView::SetQssSheetStyle(QString sheet_style)
{
	setStyleSheet(sheet_style);
}
