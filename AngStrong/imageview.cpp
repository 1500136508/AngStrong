#include "imageview.h"
#include "ui_imageview.h"
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QFile>
#include <QTextStream>
#include "imageview_qtitlebar.h"
#include "imageview_qtoolbar.h"
#include "imageview_qview.h"
#include "imageview_qstatusbar.h"

ImageView::ImageView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageView)
	,titlebar_(new ImageViewQTitleBar(parent))
	,toolbar_(new ImageViewQToolBar(parent))
	,view_(new ImageViewQView(parent))
	,statusbar_(new ImageViewQStatusBar(parent))
{
    ui->setupUi(this);
	InitializeUI();
}

ImageView::~ImageView()
{
    delete ui;
	ReleasePointer();
}

void ImageView::enterEvent(QEvent * event)
{
	toolbar_->setVisible(true);
}

void ImageView::leaveEvent(QEvent * event)
{
	toolbar_->setVisible(false);
}

void ImageView::InitializeUI()
{
	setWindowFlag(Qt::FramelessWindowHint);
	QGridLayout *pLayout = new QGridLayout(this);//创建一个整体布局器
	pLayout->addWidget(titlebar_, 0, 0);
	pLayout->addWidget(toolbar_, 1, 0);
	pLayout->addWidget(view_, 2, 0);
	pLayout->addWidget(statusbar_, 3, 0);
	pLayout->setSpacing(0);         //布局之间的距离
	pLayout->setContentsMargins(0, 0, 0, 0); //布局器的四周边距
	setLayout(pLayout);  //将这个布局器设置在QWidget上
	toolbar_->setVisible(false);

	SetDefaultQssSheetStyle();
}

void ImageView::SetQssSheetStyle(QString sheet_style)
{
	setStyleSheet(sheet_style);
	titlebar_->SetQssSheetStyle(sheet_style);
	toolbar_->SetQssSheetStyle(sheet_style);
	view_->SetQssSheetStyle(sheet_style);
	statusbar_->SetQssSheetStyle(sheet_style);
}

void ImageView::SetDefaultQssSheetStyle()
{
	QString stylesheet;
	QFile file("black.qss");
	file.open(QFile::ReadOnly);
	QTextStream filetext(&file);
	stylesheet = filetext.readAll();
	file.close();
	SetQssSheetStyle(stylesheet);
}

void ImageView::ReleasePointer()
{
	if (titlebar_)
	{
		titlebar_->deleteLater();
		titlebar_ = nullptr;
	}
	if (toolbar_)
	{
		toolbar_->deleteLater();
		toolbar_ = nullptr;
	}
	if (view_)
	{
		view_->deleteLater();
		view_ = nullptr;
	}
	if (statusbar_)
	{
		statusbar_->deleteLater();
		statusbar_ = nullptr;
	}
}
