#include "imageview_qtoolbar.h"
#include "ui_imageview_qtoolbar.h"

ImageViewQToolBar::ImageViewQToolBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageViewQToolBar)
{
    ui->setupUi(this);
	InitializeUI();
}

ImageViewQToolBar::~ImageViewQToolBar()
{
    delete ui;
}

void ImageViewQToolBar::SetQssSheetStyle(QString sheet_style)
{
	setStyleSheet(sheet_style);
}

void ImageViewQToolBar::InitializeUI()
{
	setAttribute(Qt::WA_DeleteOnClose);
	setAttribute(Qt::WA_TranslucentBackground, true);//����͸��2-�����������͸��,����͸��
}
