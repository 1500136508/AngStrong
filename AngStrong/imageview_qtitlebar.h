#ifndef IMAGEVIEW_QTITLEBAR_H
#define IMAGEVIEW_QTITLEBAR_H

#include <QWidget>

namespace Ui {
class ImageViewQTitleBar;
}

class ImageViewQTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewQTitleBar(QWidget *parent = nullptr);
    ~ImageViewQTitleBar();

	void SetQssSheetStyle(QString sheet_style);
private:
    Ui::ImageViewQTitleBar *ui;
};

#endif // IMAGEVIEW_QTITLEBAR_H
