#include "angstrong.h"
#include <QFile>
#include <QTextStream>
#include <QGridLayout>
#include <QDockWidget>
#include <QKeyEvent>
#include "widgetui.h"
#include "titlebar.h"
#include "event_handler_mai.h"
#include "logmanager.h"
#include "ui_imageview.h"

AngStrong::AngStrong(QWidget *parent)
    : QMainWindow(parent),
	main_widgets_ui_(new WidgetUI()),
	titlebar_ui_(new TitleBar()),
	eventhandler_main_(new EventHandlerMain())
{
    ui.setupUi(this);
	InitializeUI();
	BuildConnect();

	sn_thread_ = new std::thread(&AngStrong::CheckSNThread, this);
	sn_thread_->detach();
}

AngStrong::~AngStrong()
{
	is_quite_program_ = true;
	ReleasePointer();
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
	//	delete p;//ȥ��������м䴰�ڣ�ʵ����������ͣ��
	//}
	setDockNestingEnabled(true);//��DockǶ�׹���
	//imageview_.setParent(this);
	imageview_.AddEventHandler(eventhandler_main_);
	imageview_.SetTitle("ImageView00");
	imageview_.show();
}

void AngStrong::CreateCustomMainWindow()
{
	this->setParent(main_widgets_ui_);//�����������UI����ĸ�����ΪQWidget
	this->installEventFilter(titlebar_ui_);//��װ�¼�������
	QGridLayout *pLayout = new QGridLayout(main_widgets_ui_);//����һ�����岼����
	pLayout->addWidget(titlebar_ui_);  //��ӱ�����
	pLayout->addWidget(this);       //���UI����
	pLayout->setSpacing(0);         //����֮��ľ���
	pLayout->setContentsMargins(5, 5, 5, 5); //�����������ܱ߾�
	main_widgets_ui_->setLayout(pLayout);  //�����������������QWidget��
	main_widgets_ui_->show();//��ʾQWidge ������
}

void AngStrong::SetQssSheetStyle(QString style)
{
	this->setStyleSheet(style);
	main_widgets_ui_->setStyleSheet(style);
	titlebar_ui_->setStyleSheet(style);
}

void AngStrong::keyPressEvent(QKeyEvent * keyValue)
{
	sn_ += keyValue->text();
	key_press_time_ = clock();
	is_key_press_ = true;
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
	//����DisplayViewͣ������
	QDockWidget *m_dock_display = new QDockWidget(tr("Display"));
	m_dock_display->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); //���ڿ��ƶ�
	//m_dock_output->setAllowedAreas(/*Qt::LeftDockWidgetArea | */Qt::BottomDockWidgetArea);
	m_dock_display->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(Qt::RightDockWidgetArea, m_dock_display);
	m_dock_display->setWidget(&dispview_);
	m_dock_display->setMinimumWidth(500);
}

void AngStrong::BuildConnect()
{
	qRegisterMetaType<cv::Mat>("cv::Mat");
	connect(this, SIGNAL(SendSN(QString)), eventhandler_main_, SLOT(ReceiveSN(QString)));
	connect(eventhandler_main_, SIGNAL(SendPSensorData(QString)), &dispview_, SLOT(ReadPSensorData(QString)));
	connect(eventhandler_main_, SIGNAL(SendWriteSN(QString)), &dispview_, SLOT(ReceiveWriteSN(QString)));
	connect(eventhandler_main_, SIGNAL(SendReadSN(QString)), &dispview_, SLOT(ReceiveSN(QString)));
}

void AngStrong::ReleasePointer()
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
	if (sn_thread_)
	{
		delete sn_thread_;
		sn_thread_ = nullptr;
	}
	if (eventhandler_main_)
	{
		delete eventhandler_main_;
		eventhandler_main_ = nullptr;
	}
}

void AngStrong::CheckSNThread()
{
	while (!is_quite_program_)
	{
		if (is_key_press_)
		{
			if (clock() - key_press_time_ > 100)
			{
				if (sn_.isEmpty())
				{
					Sleep(3);
					continue;
				}
				emit SendSN(sn_);
				LogManager::Write("sn=" + sn_.toStdString());
				sn_.clear();
				is_key_press_ = false;
			}
		}
		else
		{
			Sleep(3);
			continue;
		}
	}
}
