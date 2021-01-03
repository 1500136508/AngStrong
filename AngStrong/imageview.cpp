#include "imageview.h"
#include "ui_imageview.h"
#include <windows.h>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QFile>
#include <QTextStream>
#include <QSplitter>
#include <QDebug>
#include <qedit.h>

ImageView::ImageView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageView)
{
    ui->setupUi(this);
	InitializeUI();
}

ImageView::~ImageView()
{
    delete ui;
	ReleasePointer();
}

bool ImageView::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
	qDebug() << "ImageView nativeEvent";
	CreateBorderEvent(eventType, message, result);
	return QWidget::nativeEvent(eventType, message, result);
}

void ImageView::InitializeUI()
{
	setWindowFlag(Qt::FramelessWindowHint);
	ui->m_graphicsview->setParent(this);
}

void ImageView::SetQssSheetStyle(QString sheet_style)
{
	setStyleSheet(sheet_style);
	ui->m_graphicsview->SetQssSheetStyle(sheet_style);
}

void ImageView::SetTitle(QString title)
{
	ui->m_lab_title->setText(title);
}

void ImageView::SetGrabImageCallBack(ISampleGrabberCB * callback)
{
	if (!grabimage_callback_)
	{
		grabimage_callback_ = callback;
	}
	ui->m_graphicsview->SetGrabImageCallBack(grabimage_callback_);
}

void ImageView::AddEventHandler(EventHandler * event_handler)
{
	ui->m_graphicsview->AddEventHandler(event_handler);
}

void ImageView::ReleasePointer()
{
}

bool ImageView::CreateBorderEvent(const QByteArray & eventType, void * message, long * result)
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
	return false;
}

void ImageView::ReceiveImage(cv::Mat image_rgb, cv::Mat image_depth, cv::Mat image_ir)
{
	ui->m_graphicsview->DisplayImage(image_rgb,image_depth,image_ir);
}