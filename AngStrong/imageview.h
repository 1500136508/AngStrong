#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>
#include <opencv.hpp>

namespace Ui {
class ImageView;
}

class ISampleGrabberCB;
class EventHandler;
class ImageView : public QWidget
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView();

	void SetQssSheetStyle(QString sheet_style);
	void SetTitle(QString title);
	void SetGrabImageCallBack(ISampleGrabberCB *callback);
	void AddEventHandler(EventHandler *event_handler);
protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
private slots:
	void ReceiveImage(cv::Mat image_rgb,cv::Mat image_depth,cv::Mat image_ir);
private:
	void InitializeUI();
	void ReleasePointer();
	bool CreateBorderEvent(const QByteArray &eventType,void *message,long *result);

    Ui::ImageView *ui;
	ISampleGrabberCB *grabimage_callback_ = nullptr;
};

#endif // IMAGEVIEW_H
