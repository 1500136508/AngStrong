#pragma once
#include <QGraphicsView>
#include "imageview_qscene.h"
#include "imageview_qpixmap.h"
#include "graphicsrectitem.h"

class ImageViewQView : public QGraphicsView
{
	Q_OBJECT
public:
	ImageViewQView(QWidget *pParent = nullptr);
	ImageViewQView(QGraphicsScene *scene, QWidget *parent = nullptr);
	virtual ~ImageViewQView();

	void SetQssSheetStyle(QString sheet_style); 
protected:
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent*event) override;
	void paintEvent(QPaintEvent *event) override;
private:
	void InitializeUI();

	std::shared_ptr<ImageViewQScene>			imageview_qscene_;
	std::shared_ptr<ImageViewQPixmap>			imageview_qpix_;
	std::shared_ptr<GraphicsRectItem>			imageview_rect_item_;
};

