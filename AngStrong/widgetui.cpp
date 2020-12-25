#include "widgetui.h"
#include "ui_widgetui.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <windows.h>

WidgetUI::WidgetUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetUI)
{
    ui->setupUi(this);
	SetWindowsProperty();
}

WidgetUI::~WidgetUI()
{
    delete ui;
}

bool WidgetUI::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
	const int kBorder = 5;
	MSG* pMsg = (MSG*)message;
	switch (pMsg->message)
	{
	case WM_NCHITTEST:
	{
		QPoint pos = mapFromGlobal(QPoint(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)));
		bool bHorLeft = pos.x() < kBorder;
		bool bHorRight = pos.x() > this->width() - kBorder;
		bool bVertTop = pos.y() < kBorder;
		bool bVertBottom = pos.y() > this->height() - kBorder;
		if (bHorLeft && bVertTop)
		{
			*result = HTTOPLEFT;
		}
		else if (bHorLeft && bVertBottom)
		{
			*result = HTBOTTOMLEFT;
		}
		else if (bHorRight && bVertTop)
		{
			*result = HTTOPRIGHT;
		}
		else if (bHorRight && bVertBottom)
		{
			*result = HTBOTTOMRIGHT;
		}
		else if (bHorLeft)
		{
			*result = HTLEFT;
		}
		else if (bHorRight)
		{
			*result = HTRIGHT;
		}
		else if (bVertTop)
		{
			*result = HTTOP;
		}
		else if (bVertBottom)
		{
			*result = HTBOTTOM;
		}
		else
		{
			return false;
		}
		return true;
	}
	break;
	default:
		break;
	}

	return QWidget::nativeEvent(eventType, message, result);
}

void WidgetUI::SetWindowsProperty()
{
	setWindowFlags(Qt::FramelessWindowHint);//È¥µô±ß¿ò
}
