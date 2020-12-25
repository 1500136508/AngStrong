#include "titlebar.h"
#include "ui_titlebar.h"
#include <qt_windows.h>
#include <QMouseEvent>

TitleBar::TitleBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleBar)
{
    ui->setupUi(this);
	BuildConnect();
}

TitleBar::~TitleBar()
{
    delete ui;
}

void TitleBar::BuildConnect()
{
	connect(ui->m_btn_min, SIGNAL(clicked()), this, SLOT(on_btn_min_clicked()));
	connect(ui->m_btn_max, SIGNAL(clicked()), this, SLOT(on_btn_max_clicked()));
	connect(ui->m_btn_close, SIGNAL(clicked()), this, SLOT(on_btn_close_clicked()));
}

void TitleBar::on_btn_min_clicked()
{
	QWidget *pWindow = this->window();
	pWindow->showMinimized();
}

void TitleBar::on_btn_max_clicked()
{
	QWidget *pWindow = this->window();
	pWindow->isMaximized() ? pWindow->showNormal() : pWindow->showMaximized();
}

void TitleBar::on_btn_close_clicked()
{
	QWidget *pWindow = this->window();
	pWindow->close();
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent * event)
{
	Q_UNUSED(event);
	emit ui->m_btn_max->clicked();
}

void TitleBar::mousePressEvent(QMouseEvent * event)
{
	if (ReleaseCapture())
	{
		QWidget *pWindow = this->window();
		if (pWindow->isWindow())
		{
			SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
		}
	}
	event->ignore();
}