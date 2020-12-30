#include "angstrong.h"
#include <QFile>
#include <QTextStream>
#include <QGridLayout>
#include <QDockWidget>
#include "widgetui.h"
#include "titlebar.h"
#include "eventhandler_grabimage.h"

AngStrong::AngStrong(QWidget *parent)
    : QMainWindow(parent),
	main_widgets_ui_(new WidgetUI()),
	titlebar_ui_(new TitleBar()),
	eventhandler_grabimage_(new EventHandlerGrabImage())
{
    ui.setupUi(this);
	InitializeUI();
	BuildConnect();
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
	if (eventhandler_grabimage_)
	{
		delete eventhandler_grabimage_;
		eventhandler_grabimage_ = nullptr;
	}
}

void AngStrong::InitializeUI()
{
	CreateCustomMainWindow();
	SetDefaultQssStyle();

	setCentralWidget(&imageview_);
	CreateDockWindow();
	//QWidget* p = takeCentralWidget();
	//if (p)
	//{
	//	delete p;//去除界面的中间窗口，实现任意区域停靠
	//}
	setDockNestingEnabled(true);//打开Dock嵌套功能
	imageview_.setParent(this);
	imageview_.SetGrabImageCallBack(eventhandler_grabimage_);
	imageview_.SetTitle("ImageView00");
	imageview_.show();
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

void AngStrong::SetQssSheetStyle(QString style)
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
	SetQssSheetStyle(stylesheet);
	imageview_.SetQssSheetStyle(stylesheet);
}

void AngStrong::CreateDockWindow()
{
	//增加DisplayView停靠窗口
	QDockWidget *m_dock_display = new QDockWidget(tr("Display"));
	m_dock_display->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //窗口可移动
	//m_dock_output->setAllowedAreas(/*Qt::LeftDockWidgetArea | */Qt::BottomDockWidgetArea);
	m_dock_display->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(Qt::RightDockWidgetArea, m_dock_display);
	m_dock_display->setWidget(&dispview_);
	m_dock_display->setMinimumWidth(500);
}

void AngStrong::BuildConnect()
{
	qRegisterMetaType<cv::Mat>("cv::Mat");
	connect(eventhandler_grabimage_, SIGNAL(send_image(cv::Mat,cv::Mat,cv::Mat)), &imageview_, SLOT(ReceiveImage(cv::Mat,cv::Mat,cv::Mat)));
}
