#include "imageview_qtoolbar.h"
#include "ui_imageview_qtoolbar.h"
#include <windows.h>
#include <dbt.h>
#include "modelparameter.h"
#include "definition_camera.h"
#include "EnumSerial.h"

ImageViewQToolBar::ImageViewQToolBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageViewQToolBar)
{
    ui->setupUi(this);
	InitializeUI();
	BuildConnect();
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
	setAttribute(Qt::WA_TranslucentBackground, true);//设置透明2-窗体标题栏不透明,背景透明
	InitializeCombo();
	FindAllPort();
}

void ImageViewQToolBar::InitializeCombo()
{
	ui->m_combo_display_mode->addItem("IR_Depth_RGB");
	ui->m_combo_display_mode->addItem("IR");
	ui->m_combo_display_mode->addItem("Depth");
	ui->m_combo_display_mode->addItem("RGB");
	ui->m_combo_display_mode->addItem("IR_Depth_RGBAddDepth");
	if (!ModelParameter::IsOpen())
	{
		if (!ModelParameter::Open("model/parameter.ini"))
		{
			ui->m_combo_display_mode->setCurrentIndex(EDisplayMode_IR_Depth_RGB);
		}
	}
	ui->m_combo_display_mode->setCurrentIndex(ModelParameter::get_int_value("Angstrong", "image_display_mode"));

	ui->m_combo_grab_mode->addItem("Preview");
	ui->m_combo_grab_mode->addItem("Grabber");
	ui->m_combo_grab_mode->setCurrentIndex(1);
}

void ImageViewQToolBar::BuildConnect()
{
	connect(ui->m_action_open_camera, SIGNAL(triggered()), this, SLOT(on_open_camera_triggered()));
	connect(ui->m_action_close_camera, SIGNAL(triggered()), this, SLOT(on_close_camera_triggered()));
	connect(ui->m_action_live, SIGNAL(triggered()), this, SLOT(on_live_triggered()));
	connect(ui->m_action_pause, SIGNAL(triggered()), this, SLOT(on_pause_triggered()));
	connect(ui->m_action_stop, SIGNAL(triggered()), this, SLOT(on_stop_triggered()));
	connect(ui->m_action_capture_filter, SIGNAL(triggered()), this, SLOT(on_capture_filter_triggered()));
	connect(ui->m_action_capture_pin, SIGNAL(triggered()), this, SLOT(on_capture_pin_triggered()));
}

void ImageViewQToolBar::FindAllPort()
{
	ui->m_combo_port->clear();
	std::vector<SSerInfo> port_container;
	port_container.clear();
	EnumSerialPorts(port_container);
	for (size_t sz = 0; sz < port_container.size(); ++sz)
	{
		auto start_position = port_container.at(sz).strFriendlyName.find("(");
		auto end_position = port_container.at(sz).strFriendlyName.find(")");
		auto iter_b = port_container.at(sz).strFriendlyName.cbegin() + start_position;
		auto iter_e = port_container.at(sz).strFriendlyName.cbegin() + end_position;

		std::string port_no_name(iter_b + 1, iter_e);
		ui->m_combo_port->addItem(QString::fromStdString(port_no_name));
	}
	if (port_container.size() > 0)
	{
		ui->m_combo_port->setCurrentIndex(0);
	}
}

void ImageViewQToolBar::InitialzeCameraList(std::list<std::string> camera_list)
{
	ui->m_combo_cameralist->clear();
	if (camera_list.size() <= 0)
	{
		ui->m_combo_cameralist->clear();
	}
	else
	{
		for (auto &iter : camera_list)
		{
			ui->m_combo_cameralist->addItem(QString::fromStdString(iter));
		}
	}
}

int ImageViewQToolBar::get_current_camera_index()
{
	return ui->m_combo_cameralist->currentIndex();
}

int ImageViewQToolBar::get_current_display_mode()
{
	return ui->m_combo_display_mode->currentIndex();
}

int ImageViewQToolBar::get_current_grab_mode()
{
	return ui->m_combo_grab_mode->currentIndex();
}

void ImageViewQToolBar::on_open_camera_triggered()
{
	emit send_open_camera_triggered();
}

void ImageViewQToolBar::on_close_camera_triggered()
{
	emit send_close_camera_triggered();
}

void ImageViewQToolBar::on_live_triggered()
{
	emit send_live_triggered();
}

void ImageViewQToolBar::on_pause_triggered()
{
	emit send_pause_triggered();
}

void ImageViewQToolBar::on_stop_triggered()
{
	emit send_stop_triggered();
}

void ImageViewQToolBar::on_capture_filter_triggered()
{
	emit send_capture_filter_triggered();
}

void ImageViewQToolBar::on_capture_pin_triggered()
{
	emit send_capture_pin_triggered();
}
