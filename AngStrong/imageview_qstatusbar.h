#ifndef IMAGEVIEW_QSTATUSBAR_H
#define IMAGEVIEW_QSTATUSBAR_H

#include <QWidget>

namespace Ui {
class ImageViewQStatusBar;
}

class ImageViewQStatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewQStatusBar(QWidget *parent = nullptr);
    ~ImageViewQStatusBar();

	void SetQssSheetStyle(QString sheet_style);
private:

    Ui::ImageViewQStatusBar *ui;
};

#endif // IMAGEVIEW_QSTATUSBAR_H
