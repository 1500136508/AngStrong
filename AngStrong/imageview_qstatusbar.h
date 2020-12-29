#ifndef IMAGEVIEW_QSTATUSBAR_H
#define IMAGEVIEW_QSTATUSBAR_H

#include <QStatusBar>

class ImageViewQStatusBar : public QStatusBar
{
    Q_OBJECT

public:
    explicit ImageViewQStatusBar(QWidget *parent = nullptr);
    ~ImageViewQStatusBar();

	void SetQssSheetStyle(QString sheet_style);
private:
};

#endif // IMAGEVIEW_QSTATUSBAR_H
