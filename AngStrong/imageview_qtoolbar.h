#ifndef IMAGEVIEW_QTOOLBAR_H
#define IMAGEVIEW_QTOOLBAR_H

#include <QToolBar>

namespace Ui {
class ImageViewQToolBar;
}

class ImageViewQToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit ImageViewQToolBar(QWidget *parent = nullptr);
    ~ImageViewQToolBar();

	void SetQssSheetStyle(QString sheet_style);
private:
	void InitializeUI();

    Ui::ImageViewQToolBar *ui;
};

#endif // IMAGEVIEW_QTOOLBAR_H
