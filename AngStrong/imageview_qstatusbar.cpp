#include "imageview_qstatusbar.h"
#include "ui_imageview_qstatusbar.h"

ImageViewQStatusBar::ImageViewQStatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageViewQStatusBar)
{
    ui->setupUi(this);
}

ImageViewQStatusBar::~ImageViewQStatusBar()
{
    delete ui;
}

void ImageViewQStatusBar::SetQssSheetStyle(QString sheet_style)
{
	setStyleSheet(sheet_style);
}
