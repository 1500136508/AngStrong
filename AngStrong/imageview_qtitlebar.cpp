#include "imageview_qtitlebar.h"
#include <qt_windows.h>
#include <QMouseEvent>

ImageViewQTitleBar::ImageViewQTitleBar(QWidget *parent) :
    QWidget(parent)
{
}

ImageViewQTitleBar::~ImageViewQTitleBar()
{
}

void ImageViewQTitleBar::SetQssSheetStyle(QString sheet_style)
{
	setStyleSheet(sheet_style);
}

void ImageViewQTitleBar::enterEvent(QEvent * event)
{
	setCursor(Qt::SizeAllCursor);
}

void ImageViewQTitleBar::mousePressEvent(QMouseEvent * event)
{
	if (ReleaseCapture())
	{
		QWidget *pWindow = this->window();
		if (pWindow->isWindow())
		{
			SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
		}
	}
	event->ignore();
}
