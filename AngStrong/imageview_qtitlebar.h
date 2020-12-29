#ifndef IMAGEVIEW_QTITLEBAR_H
#define IMAGEVIEW_QTITLEBAR_H

#include <QWidget>

class ImageViewQTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewQTitleBar(QWidget *parent = nullptr);
    ~ImageViewQTitleBar();

	void SetQssSheetStyle(QString sheet_style);
protected:
	void enterEvent(QEvent *event)override;
	virtual void mousePressEvent(QMouseEvent *event)override;
private:
};

#endif // IMAGEVIEW_QTITLEBAR_H
