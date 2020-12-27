#include "imageview.h"
#include "ui_imageview.h"
#include <QHBoxLayout>
#include <QGraphicsView>
#include "viewinfo.h"
#include "viewtool.h"

ImageView::ImageView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageView)
{
    ui->setupUi(this);
	//setWindowFlag(Qt::FramelessWindowHint);

	view_tool_ = new ViewTool();
	graphics_view_ = new QGraphicsView();
	view_info_ = new ViewInfo();
	QGridLayout *pLayout = new QGridLayout(this);//创建一个整体布局器
	pLayout->addWidget(view_tool_,0,0);
	pLayout->addWidget(graphics_view_,1,0);
	pLayout->addWidget(view_info_,2,0);
	pLayout->setSpacing(0);         //布局之间的距离
	pLayout->setContentsMargins(0, 0, 0, 0); //布局器的四周边距
	setLayout(pLayout);  //将这个布局器设置在QWidget上
	view_tool_->setVisible(false);
}

ImageView::~ImageView()
{
    delete ui;
	if (graphics_view_)
	{
		graphics_view_->deleteLater();
		graphics_view_ = nullptr;
	}
	if (view_info_)
	{
		view_info_->deleteLater();
		view_info_ = nullptr;
	}
	if (view_tool_)
	{
		view_tool_->deleteLater();
		view_tool_ = nullptr;
	}
}

void ImageView::enterEvent(QEvent * event)
{
	view_tool_->setVisible(true);
}

void ImageView::leaveEvent(QEvent * event)
{
	view_tool_->setVisible(false);
}
