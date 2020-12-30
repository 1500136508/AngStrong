#pragma once
#include <vector>
#include <QGraphicsView>
#include <opencv.hpp>
#include "imageview_qscene.h"
#include "imageview_qpixmap.h"
#include "graphicsrectitem.h"
#include "imageview_qtoolbar.h"

class CameraDS;
class ISampleGrabberCB;
class ImageViewQView : public QGraphicsView
{
	Q_OBJECT
public:
	ImageViewQView(QWidget *pParent = nullptr);
	ImageViewQView(QGraphicsScene *scene, QWidget *parent = nullptr);
	virtual ~ImageViewQView();

	bool OpenCamera(const int camera_id, const int widht, const int height, bool is_YUV2 = false);
	bool CloseCamera();
	void SetGrabImageCallBack(ISampleGrabberCB *callback);
	void DisplayImage(cv::Mat image_rgb, cv::Mat image_depth, cv::Mat image_ir);

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
	void resizeEvent(QResizeEvent *)override;
	void contextMenuEvent(QContextMenuEvent *event)override;
	void enterEvent(QEvent *event)override;
	void leaveEvent(QEvent *event)override;
	bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
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

	void SetImage(cv::Mat mat);//接收图像专用槽函数

	void ReceiveOpenCameraTriggered();
	void ReceiveCloseCameraTriggered();
	void ReceiveLiveTriggered();
	void ReceivePauseTriggered();
	void ReceiveStopTriggered();
	void ReceiveCaptureFilterTriggered();
	void ReceiveCapturePinTriggered();
private:
	void InitializeUI();
	void BuildConnect();
	void CreateCustomRightButtonMenu();
	void ReleasePointer();
	void RegisterDevice();//注册nativeEvent;

	void UpdataCameraList();

	QImage cvMat2QImage(const cv::Mat& mat);
	cv::Mat QImage2cvMat(QImage image);

	std::shared_ptr<ImageViewQScene>			imageview_qscene_;
	std::shared_ptr<ImageViewQPixmap>			imageview_qpix_;
	std::shared_ptr<GraphicsRectItem>			imageview_rect_item_;
	std::shared_ptr<ImageViewQToolBar>			imageview_toolbar_;
	CameraDS									*camera_;
	ISampleGrabberCB							*grabimage_callback_function_ = nullptr;

	cv::Mat m_Image;
	QImage qImage;
	int m_ImageWidth;
	int m_ImageHeight;
	std::vector<cv::Mat> container_;
	cv::Mat combine_image_;
	volatile bool first_time_to_live_ = true;
};

