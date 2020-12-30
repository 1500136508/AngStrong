#include "displayview.h"
#include "ui_displayview.h"
#include <windows.h>
#include <QPalette>
#include <QDebug>

DisplayView::DisplayView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayView)
{
    ui->setupUi(this);
	BuildConnect();
	updata_ui_message_.start(5000);
}

DisplayView::~DisplayView()
{
    delete ui;
}

void DisplayView::BuildConnect()
{
	connect(&updata_ui_message_, SIGNAL(timeout()), this, SLOT(UpdataUIMessage()));
}

void DisplayView::ReceiveImageGray(int r, int g, int b)
{
	//ui->m_lab_rgb->setText("(R:" + QString::number(r) + " G:" + QString::number(g)+" B:" + QString::number(b)+")");
}

void DisplayView::ReceiveAvgDepth(float avg0, float avg1)
{
	//QString qstrAvgInfo("");
	/*ui->m_lab_avg_depth01->setText(QString::number(avg0));
	ui->m_lab_avg_depth02->setText(QString::number(avg1));*/
}

void DisplayView::ReceiveXMData(QString read_data)
{
	ui->m_lab_read_data->setText(read_data);
}

void DisplayView::ReadPSensorData(QString psensor_data)
{
	ui->m_lab_read_data->setText(psensor_data);
}

void DisplayView::ReceiveDeviceSNData(QString message)
{
	ui->m_lab_device_sn->setText(message);
}

void DisplayView::UpdataUIMessage()
{
	ui->m_lab_device_sn->setText("--");
}

void DisplayView::ReceiveLocationDepth(int x, int y, float depth)
{
	QString info;
	info.sprintf("%.3f", depth);
#if DEBUG
	qDebug() << depth;
#endif
	ui->m_lab_mouse_xy->setText("X:" + QString::number(x) + " Y:" + QString::number(y));
	ui->m_lab_depth->setText(info);
}