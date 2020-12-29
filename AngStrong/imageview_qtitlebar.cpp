#include "imageview_qtitlebar.h"
#include "ui_imageview_qtitlebar.h"

ImageViewQTitleBar::ImageViewQTitleBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageViewQTitleBar)
{
    ui->setupUi(this);
}

ImageViewQTitleBar::~ImageViewQTitleBar()
{
    delete ui;
}

void ImageViewQTitleBar::SetQssSheetStyle(QString sheet_style)
{
	setStyleSheet(sheet_style);
}
