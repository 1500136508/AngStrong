#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>

namespace Ui {
class ImageView;
}

class ImageViewQTitleBar;
class ImageViewQToolBar;
class ImageViewQView;
class ImageViewQStatusBar;
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
	void InitializeUI();
	void SetQssSheetStyle(QString sheet_style);
	void SetDefaultQssSheetStyle();

	void ReleasePointer();

    Ui::ImageView *ui;
	ImageViewQTitleBar *titlebar_;
	ImageViewQToolBar *toolbar_;
	ImageViewQView *view_;
	ImageViewQStatusBar *statusbar_;
};

#endif // IMAGEVIEW_H
