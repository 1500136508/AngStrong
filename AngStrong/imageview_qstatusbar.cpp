#include "imageview_qstatusbar.h"

ImageViewQStatusBar::ImageViewQStatusBar(QWidget *parent) :
    QStatusBar(parent)
{
}

ImageViewQStatusBar::~ImageViewQStatusBar()
{
}

void ImageViewQStatusBar::SetQssSheetStyle(QString sheet_style)
{
	setStyleSheet(sheet_style);
}
