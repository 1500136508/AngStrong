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
