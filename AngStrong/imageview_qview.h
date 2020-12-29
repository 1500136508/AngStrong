#pragma once
#include <QGraphicsView>
#include <opencv.hpp>
#include "imageview_qscene.h"
#include "imageview_qpixmap.h"
#include "graphicsrectitem.h"
#include "imageview_qtoolbar.h"

class ImageViewQView : public QGraphicsView
{
	Q_OBJECT
public:
	ImageViewQView(QWidget *pParent = nullptr);
	ImageViewQView(QGraphicsScene *scene, QWidget *parent = nullptr);
	virtual ~ImageViewQView();

	void SetQssSheetStyle(QString sheet_style); 

	void Zoom(QPointF pointF, double fScale = 1.0f);
	void ZoomIn(QPointF poinF, double fScale = 1.2f);
	void ZoomOut(QPointF pointF, double fScale = 0.8f);
	void ZoomFit();
protected:
	bool Open();//Only Framework
	bool Save();//Only Framework

	void mouseMoveEvent(QMouseEvent *event)override;
	void mousePressEvent(QMouseEvent *event)override;
	void mouseReleaseEvent(QMouseEvent *event)override;
	void wheelEvent(QWheelEvent*event)override;
	void paintEvent(QPaintEvent *event)override;
	void contextMenuEvent(QContextMenuEvent *event)override;
	void enterEvent(QEvent *event)override;
	void leaveEvent(QEvent *event)override;
public slots:
	void on_open_clicked();
	void on_save_clicked();
	void on_close_clicked();
	void on_zoomIn_clicked();
	void on_zoomOut_clicked();
	void on_zoomFit_clicked();
	void on_measure_clicked();
	void on_measureRect_clicked();
	void on_measureCircle_clicked();
private:
	void InitializeUI();
	void CreateCustomRightButtonMenu();
	void ReleasePointer();
	int threadtest();

	QImage cvMat2QImage(const cv::Mat& mat);
	cv::Mat QImage2cvMat(QImage image);

	std::shared_ptr<ImageViewQScene>			imageview_qscene_;
	std::shared_ptr<ImageViewQPixmap>			imageview_qpix_;
	std::shared_ptr<GraphicsRectItem>			imageview_rect_item_;
	std::shared_ptr<ImageViewQToolBar>			imageview_toolbar_;

	cv::Mat m_Image;
	QImage qImage;
	int m_ImageWidth;
	int m_ImageHeight;
};

