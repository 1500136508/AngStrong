#include "angstrong.h"
#include <QFile>
#include <QTextStream>
#include <QGridLayout>
#include "widgetui.h"
#include "titlebar.h"

AngStrong::AngStrong(QWidget *parent)
    : QMainWindow(parent),
	main_widgets_ui_(new WidgetUI()),
	titlebar_ui_(new TitleBar())
{
    ui.setupUi(this);
	ModifyUI();
}

AngStrong::~AngStrong()
{
	if (main_widgets_ui_)
	{
		main_widgets_ui_->deleteLater();
		main_widgets_ui_ = nullptr;
	}
	if (titlebar_ui_)
	{
		titlebar_ui_->deleteLater();
		titlebar_ui_ = nullptr;
	}
}

void AngStrong::ModifyUI()
{
	CreateCustomMainWindow();
	SetDefaultQssStyle();
}

void AngStrong::CreateCustomMainWindow()
{
	this->setParent(main_widgets_ui_);//重新设置这个UI界面的父对象为QWidget
	this->installEventFilter(titlebar_ui_);//安装事件过滤器
	QGridLayout *pLayout = new QGridLayout(main_widgets_ui_);//创建一个整体布局器
	pLayout->addWidget(titlebar_ui_);  //添加标题栏
	pLayout->addWidget(this);       //添加UI界面
	pLayout->setSpacing(0);         //布局之间的距离
	pLayout->setContentsMargins(5, 5, 5, 5); //布局器的四周边距
	main_widgets_ui_->setLayout(pLayout);  //将这个布局器设置在QWidget上
	main_widgets_ui_->show();//显示QWidge 最后添加
}

void AngStrong::SetQssStyle(QString style)
{
	this->setStyleSheet(style);
	main_widgets_ui_->setStyleSheet(style);
	titlebar_ui_->setStyleSheet(style);
}

void AngStrong::SetDefaultQssStyle()
{
	QString stylesheet;
	QFile file("qss/qss_black.qss");
	file.open(QFile::ReadOnly);
	QTextStream filetext(&file);
	stylesheet = filetext.readAll();
	file.close();
	SetQssStyle(stylesheet);
}
