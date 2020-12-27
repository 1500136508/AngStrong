#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>

namespace Ui {
class ImageView;
}

class ViewInfo;
class ViewTool;
class QGraphicsView;
class ImageView : public QWidget
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView();
protected:
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
private:
    Ui::ImageView *ui;
	QGraphicsView *graphics_view_ = nullptr;
	ViewInfo *view_info_ = nullptr;
	ViewTool *view_tool_ = nullptr;
};

#endif // IMAGEVIEW_H
