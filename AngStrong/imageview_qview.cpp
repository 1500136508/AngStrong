#pragma execution_character_set("utf-8")
#include "imageview_qview.h"
#include <windows.h>
#include <dbt.h>
#include <list>
#include <QMenu>
#include <QFileDialog>
#include <QMouseEvent>
#include "definitionmenu.h"
#include "imageview_qview.h"
#include "CameraDS.h"
#include "logmanager.h"
#include "definition_camera.h"
#include "eventhandler.h"

ImageViewQView::ImageViewQView(QWidget *pParent /* = nullptr */)
	:QGraphicsView(pParent)
	,imageview_qscene_(std::make_shared<ImageViewQScene>(new ImageViewQScene(pParent)))
	,imageview_qpix_(std::make_shared<ImageViewQPixmap>(new ImageViewQPixmap()))
	,imageview_rect_item_(std::make_shared<GraphicsRectItem>(new GraphicsRectItem()))
	,imageview_toolbar_(std::make_shared<ImageViewQToolBar>(new ImageViewQToolBar()))
	,camera_(new CameraDS())
{
	InitializeUI();
	BuildConnect();
	for (int i = 0; i < 3; i++) container_.push_back(cv::Mat());
}

ImageViewQView::ImageViewQView(QGraphicsScene *scene, QWidget *parent /* = nullptr */)
	: QGraphicsView(scene, parent)
	, imageview_qscene_(std::make_shared<ImageViewQScene>(scene))
	, imageview_qpix_(std::make_shared<ImageViewQPixmap>(new ImageViewQPixmap()))
	, imageview_rect_item_(std::make_shared<GraphicsRectItem>(new GraphicsRectItem()))
	, imageview_toolbar_(std::make_shared<ImageViewQToolBar>(new ImageViewQToolBar()))
	, camera_(new CameraDS())
{
	InitializeUI();
}

ImageViewQView::~ImageViewQView()
{
	CloseCamera();
	ReleasePointer();
}

void ImageViewQView::AddEventHandler(EventHandler * event_handler)
{
	if (!event_handler_)
	{
		event_handler_ = event_handler;
	}
}

bool ImageViewQView::OpenCamera(const int camera_id, const int widht, const int height, bool is_YUV2)
{
	bool bReturn = false;
	do
	{
		if (!camera_->Initialize())
			break;

		if (!camera_->SetCameraDevice(camera_id))
			break;
		if (!camera_->AddCaptureVideoFilter())
			break;
		if (!camera_->SetCameraFormat(camera_id, widht, height, is_YUV2))
			break;
		camera_->SetCallBack(grabimage_callback_function_);

		camera_->Run();
		bReturn = true;
	} while (false);
	return bReturn;
}

bool ImageViewQView::CloseCamera()
{
	bool bReturn = false;
	do
	{
		if (!camera_->Terminate())
			break;
		bReturn = true;
	} while (false);
	return bReturn;
}

void ImageViewQView::SetGrabImageCallBack(ISampleGrabberCB * callback)
{
	grabimage_callback_function_ = callback;
}

void ImageViewQView::DisplayImage(cv::Mat image_rgb, cv::Mat image_depth, cv::Mat image_ir)
{
	try
	{
		int display_mode = imageview_toolbar_->get_current_display_mode();
		switch (display_mode)
		{
		case EDisplayMode_IR_Depth_RGB:
		{
			image_ir.copyTo(container_[0]);
			image_depth.copyTo(container_[1]);
			image_rgb.copyTo(container_[2]);
			cv::hconcat(container_, combine_image_);
		}
		break;
		case EDisplayMode_IR:
		{
			combine_image_ = image_ir;
		}
		break;
		case EDisplayMode_Depth:
		{
			combine_image_ = image_depth;
		}
		break;
		case EDisplayMode_RGB:
		{
			combine_image_ = image_rgb;
		}
		break;
		case EDisplayMode_IR_Depth_RGBAddDepth:
		{
			cv::Mat plus = image_rgb / 2 + image_depth / 2;
			plus.copyTo(container_[2]);
			cv::hconcat(container_, combine_image_);
		}
		break;
		default:
			break;
		}

		if (imageview_qpix_)
		{
			qImage = cvMat2QImage(combine_image_);
			m_ImageWidth = qImage.width();
			m_ImageHeight = qImage.height();
			if (!qImage.isNull())
			{
				imageview_qpix_->setPixmap(QPixmap::fromImage(qImage));
				imageview_qpix_->setScale(imageview_qpix_->GetScale());
				if (first_time_to_live_)
				{
					ZoomFit();
					first_time_to_live_ = false;
				}
			}
		}
	}
	catch (cv::Exception &e)
	{
		std::string err_msg = e.what();
		LogManager::Write(err_msg);
		return;
	}
}

void ImageViewQView::mouseMoveEvent(QMouseEvent * event)
{
	if (imageview_qpix_)
	{
		if (!imageview_qpix_->pixmap().isNull())
		{
			CalcInfo(event->localPos());
		}
	}
	QGraphicsView::mouseMoveEvent(event);
}

void ImageViewQView::mousePressEvent(QMouseEvent * event)
{
	QGraphicsView::mousePressEvent(event);
}

void ImageViewQView::mouseReleaseEvent(QMouseEvent * event)
{
	QGraphicsView::mouseReleaseEvent(event);
}

void ImageViewQView::wheelEvent(QWheelEvent * event)
{
	return QGraphicsView::wheelEvent(event);
}

void ImageViewQView::paintEvent(QPaintEvent * event)
{
	return QGraphicsView::paintEvent(event);
}

void ImageViewQView::resizeEvent(QResizeEvent *)
{
	ZoomFit();
	imageview_toolbar_->setParent(this);
	QRect view_size = geometry();
	imageview_toolbar_->resize(QSize(view_size.width(), 50));
	imageview_toolbar_->move(0, 0);
	imageview_toolbar_->show();
}

void ImageViewQView::contextMenuEvent(QContextMenuEvent * event)
{
	CreateCustomRightButtonMenu();
}

void ImageViewQView::enterEvent(QEvent * event)
{
	imageview_toolbar_->setParent(this);
	QRect view_size = geometry();
	imageview_toolbar_->resize(QSize(view_size.width(), 50));
	imageview_toolbar_->move(0, 0);
	imageview_toolbar_->show();
}

void ImageViewQView::leaveEvent(QEvent * event)
{
	//imageview_toolbar_->hide();
}

bool ImageViewQView::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
	MSG* msg = reinterpret_cast<MSG*>(message);
	int msgType = msg->message;
	if (msgType == WM_DEVICECHANGE)
	{
		imageview_toolbar_->FindAllPort();
		UpdataCameraList();
		PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
		switch (msg->wParam)
		{
		case DBT_DEVICEARRIVAL:
		{
			if (lpdb->dbch_devicetype = DBT_DEVTYP_DEVICEINTERFACE)
			{
				Sleep(50);
				/*LogManager::Write("检测到插入相机");
				InitCamera();
				m_ParamView.FindAllPort();*/
			}
		}
		break;
		case DBT_DEVICEREMOVECOMPLETE:
		{
			if (lpdb->dbch_devicetype = DBT_DEVTYP_DEVICEINTERFACE)
			{
				int a = 0;
				//LogManager::Write("检测到拔出相机");
				//if (!m_pMainImageView->m_pCamera->CameraIsStillHere())
				//{
				//	emit m_pMainImageView->SendCameraStatus(ECameraStatus_Close);
				//}
				//InitCamera();//检查camera
				//m_ParamView.FindAllPort();
			}
			break;
		}
		}
	}
	return QWidget::nativeEvent(eventType, message, result);
}

void ImageViewQView::on_open_clicked()
{
	Open();
}

void ImageViewQView::on_save_clicked()
{
	Save();
}

void ImageViewQView::on_close_clicked()
{
}

void ImageViewQView::on_zoomIn_clicked()
{
}

void ImageViewQView::on_zoomOut_clicked()
{
}

void ImageViewQView::on_zoomFit_clicked()
{
	ZoomFit();
}

void ImageViewQView::on_measure_clicked()
{
}

void ImageViewQView::on_measureRect_clicked()
{
}

void ImageViewQView::on_measureCircle_clicked()
{
}

void ImageViewQView::ReceiveLiveTriggered()
{
	if (camera_->Run())
	{
		if (event_handler_)
		{
			event_handler_->Run();
		}
	}
}

void ImageViewQView::ReceivePauseTriggered()
{
	if (camera_->Pause())
	{
		if (event_handler_)
		{
			event_handler_->Pause();
		}
	}
}

void ImageViewQView::ReceiveStopTriggered()
{
	if (camera_->Stop())
	{
		if (event_handler_)
		{
			event_handler_->Stop();
		}
	}
}

void ImageViewQView::ReceiveCaptureFilterTriggered()
{
	camera_->ShowCameraCaptureFilterDialog();
}

void ImageViewQView::ReceiveCapturePinTriggered()
{
	camera_->ShowCmaeraCapturePinDialog();
}

void ImageViewQView::InitializeUI()
{
	setAttribute(Qt::WA_DeleteOnClose);
	imageview_qscene_->addItem(imageview_qpix_.get());//添加像元元素绑定到Scene
	setScene(imageview_qscene_.get());//将QGraphicsView添加Scene;
	setMouseTracking(true);

	RegisterDevice();
	imageview_toolbar_->FindAllPort();
	UpdataCameraList();
	imageview_toolbar_->hide();
}

void ImageViewQView::BuildConnect()
{
	connect(imageview_toolbar_.get(), SIGNAL(send_open_camera_triggered()), this, SLOT(ReceiveOpenCameraTriggered()));
	connect(imageview_toolbar_.get(), SIGNAL(send_close_camera_triggered()), this, SLOT(ReceiveCloseCameraTriggered()));
	connect(imageview_toolbar_.get(), SIGNAL(send_live_triggered()), this, SLOT(ReceiveLiveTriggered()));
	connect(imageview_toolbar_.get(), SIGNAL(send_pause_triggered()), this, SLOT(ReceivePauseTriggered()));
	connect(imageview_toolbar_.get(), SIGNAL(send_stop_triggered()), this, SLOT(ReceiveStopTriggered()));
	connect(imageview_toolbar_.get(), SIGNAL(send_capture_filter_triggered()), this, SLOT(ReceiveCaptureFilterTriggered()));
	connect(imageview_toolbar_.get(), SIGNAL(send_capture_pin_triggered()), this, SLOT(ReceiveCapturePinTriggered()));
}

void ImageViewQView::CreateCustomRightButtonMenu()
{
	bool add_to_main_mune = true;
	QAction *m_action[EImageViewMenu_Total];//声明动作
	QString m_strMenuName[EImageViewMenu_Total];
	QMenu *m_menu;//声明菜单
	QMenu *m_menu_child_measure;//测量子菜单
	std::vector<std::function<void()>> m_fvec;

	for (auto v : m_action)
	{
		v = nullptr;
	}
	//配置菜单
	m_strMenuName[EImageViewMenu_Open] = "打开";
	m_strMenuName[EImageViewMenu_Save] = "保存";
	m_strMenuName[EImageViewMenu_Close] = "关闭";
	m_strMenuName[EImageViewMenu_ZoomIn] = "放大";
	m_strMenuName[EImageViewMenu_ZoomOut] = "缩小";
	m_strMenuName[EImageViewMenu_ZoomFit] = "适应";
	m_strMenuName[EImageViewMenu_Measure] = "测量";
	m_strMenuName[EImageViewMenu_Child_MeasureRect] = "矩形";
	m_strMenuName[EImageViewMenu_Child_MeasureCircle] = "圆形";
	m_fvec.push_back(std::bind(&ImageViewQView::on_open_clicked, this));
	m_fvec.push_back(std::bind(&ImageViewQView::on_save_clicked, this));
	m_fvec.push_back(std::bind(&ImageViewQView::on_close_clicked, this));
	m_fvec.push_back(std::bind(&ImageViewQView::on_zoomIn_clicked, this));
	m_fvec.push_back(std::bind(&ImageViewQView::on_zoomOut_clicked, this));
	m_fvec.push_back(std::bind(&ImageViewQView::on_zoomFit_clicked, this));
	m_fvec.push_back(std::bind(&ImageViewQView::on_measure_clicked, this));
	m_fvec.push_back(std::bind(&ImageViewQView::on_measureRect_clicked, this));
	m_fvec.push_back(std::bind(&ImageViewQView::on_measureCircle_clicked, this));
	m_menu = new QMenu(this);
	m_menu_child_measure = new QMenu(this);
	for (size_t i = EImageViewMenu_Open; i < EImageViewMenu_Total; ++i)
	{
		m_action[i] = new QAction(m_strMenuName[i]);
		switch (i)
		{
		case EImageViewMenu_ZoomIn:
		{
			m_menu->addSeparator();
		}
		break;
		case EImageViewMenu_Child_MeasureRect:
		case EImageViewMenu_Child_MeasureCircle:
		{
			m_menu_child_measure->addAction(m_action[i]);
			add_to_main_mune = false;
		}
		break;
		default:
			break;
		}
		if (add_to_main_mune)
		{
			m_menu->addAction(m_action[i]);
		}
		connect(m_action[i], &QAction::triggered, this, m_fvec.at(i));
		add_to_main_mune = true;
	}
	m_action[EImageViewMenu_Measure]->setMenu(m_menu_child_measure);//父菜单添加子菜单
	m_menu->addMenu(m_menu_child_measure);//将子菜单添加到主菜单
	m_menu->addMenu(m_menu_child_measure);//将子菜单添加到主菜单
	m_menu->setStyleSheet("background-color: rgba(74, 79, 81, 1); color: rgb(187, 187, 187);selection-background-color:#4b6eaf; ");;
	m_menu_child_measure->setStyleSheet("background-color: rgba(74, 79, 81, 1); color: rgb(187, 187, 187);selection-background-color:#4b6eaf; ");
	m_menu->exec(QCursor::pos());//显示菜单

	//内存清理与释放
	if (m_menu)
	{
		delete m_menu;
		m_menu = nullptr;
	}
	if (m_menu_child_measure)
	{
		delete m_menu_child_measure;
		m_menu_child_measure = nullptr;
	}
	for (auto v : m_action)
	{
		if (v)
		{
			delete v;
			v = nullptr;
		}
	}
}

void ImageViewQView::ReleasePointer()
{
	if (camera_)
	{
		delete camera_;
		camera_ = nullptr;
	}
	/*if (event_handler_)
	{
		delete event_handler_;
		event_handler_ = nullptr;
	}*/
}

void ImageViewQView::RegisterDevice()
{
	const GUID GUID_DEVINTERFACE_LIST[] =
	{
		{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
		{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } }
	};

	HDEVNOTIFY hDevNotify;
	DEV_BROADCAST_DEVICEINTERFACE NotifacationFiler;
	ZeroMemory(&NotifacationFiler, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
	NotifacationFiler.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotifacationFiler.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

	for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
	{
		NotifacationFiler.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];

		hDevNotify = RegisterDeviceNotification((HANDLE)this->winId(), &NotifacationFiler, DEVICE_NOTIFY_WINDOW_HANDLE);
		if (!hDevNotify)
		{
			//qDebug() << QStringLiteral("注册失败！") <<endl;
#ifdef DEBUG
			qDebug() << QStringLiteral("error") << endl;
#endif // DEBUG
		}
	}
}

void ImageViewQView::SetImage(cv::Mat mat)
{
	if (imageview_qpix_)
	{
		qImage = cvMat2QImage(mat);
		m_ImageWidth = qImage.width();
		m_ImageHeight = qImage.height();
		if (!qImage.isNull())
		{
			imageview_qpix_->setPixmap(QPixmap::fromImage(qImage));
			imageview_qpix_->setScale(imageview_qpix_->GetScale());
		}
	}
}

void ImageViewQView::ReceiveOpenCameraTriggered()
{
	int camera_index = imageview_toolbar_->get_current_camera_index();
	if (camera_index < 0)
	{
		LogManager::Write("Failed to Live:No Camera!");
		return;
	}
	if (OpenCamera(camera_index, 640, 480, true))
	{
		if (event_handler_)
		{
			int current_port = imageview_toolbar_->get_current_port();
			event_handler_->OpenCamera(current_port);
		}
	}
}

void ImageViewQView::ReceiveCloseCameraTriggered()
{
	if (CloseCamera())
	{
		if (event_handler_)
		{
			event_handler_->CloseCamera();
		}
	}
}

void ImageViewQView::UpdataCameraList()
{
	std::list<std::string> camera_name_list;
	camera_name_list.clear();
	auto camera_counts = camera_->CameraCount();
	if (camera_counts > 0)
	{
		char camera_name_char[100];
		for (auto i = 0; i < camera_counts; i++)
		{
			camera_->GetCameraName(i, camera_name_char, 100);
			std::string camera_name_string(camera_name_char);
			camera_name_list.push_back(camera_name_string);
		}
	}
	imageview_toolbar_->InitialzeCameraList(camera_name_list);
}

void ImageViewQView::CalcInfo(QPointF point)
{
	qreal view_x = point.x();
	qreal view_y = point.y();

	QPointF qPoint1 = imageview_qpix_->scenePos();//图像坐标
	QPointF qPoint2 = mapToScene(view_x, view_y);//当前鼠标坐标
	qreal imageW = imageview_qpix_->pixmap().width()*imageview_qpix_->GetScale();
	qreal imageH = imageview_qpix_->pixmap().height()*imageview_qpix_->GetScale();
	qreal ratio_x = m_ImageWidth / (imageW*1.0);
	qreal ratio_y = m_ImageHeight / (imageH*1.0);
	if (qPoint2.x() >= qPoint1.x() && qPoint2.x() <= qPoint1.x() + imageW &&
		qPoint2.y() >= qPoint1.y() && qPoint2.y() <= qPoint1.y() + imageH)//判断鼠标是否在图像上
	{

		QPointF p1((qPoint2.x() - qPoint1.x()), (qPoint2.y() - qPoint1.y()));
		qreal x = p1.x() * ratio_x;
		qreal y = p1.y() * ratio_y;

		if (qImage.valid(x, y))//判断坐标是否有效
		{
			QColor color = qImage.pixel(floor(x), floor(y));
			int GrayValue_R = color.red();
			int GrayValue_G = color.green();
			int GrayValue_B = color.blue();
			//emit SendImageGray(GrayValue_R, GrayValue_G, GrayValue_B);
#ifdef DEBUG
			qDebug() << "X:" << x << "Y:" << y << "R:" << GrayValue_R << " G:" << GrayValue_G << " B:" << GrayValue_B;
#endif
		}
		if (event_handler_)
		{
			event_handler_->MouseMoveInfo(x, y);
		}
	}
	else
	{
#ifdef DEBUG
		qDebug() << "out of range";
#endif
		if (event_handler_)
		{
			event_handler_->MouseMoveInfo(-1, -1);
		}
	}
}

QImage ImageViewQView::cvMat2QImage(const cv::Mat & mat)
{
	if (mat.type() == CV_8UC1 || mat.type() == CV_32FC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	else if (mat.type() == CV_8UC3)
	{
		const uchar *pSrc = (const uchar*)mat.data;
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{
		const uchar *pSrc = (const uchar*)mat.data;
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		return QImage();
	}
}

cv::Mat ImageViewQView::QImage2cvMat(QImage image)
{
	cv::Mat mat;
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(mat, mat, CV_BGR2RGB);
		break;
	case QImage::Format_Indexed8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}

void ImageViewQView::SetQssSheetStyle(QString sheet_style)
{
	setStyleSheet(sheet_style);
	imageview_toolbar_->SetQssSheetStyle(sheet_style);
}

void ImageViewQView::Zoom(QPointF pointF, double fScale)
{
}

void ImageViewQView::ZoomIn(QPointF poinF, double fScale)
{
}

void ImageViewQView::ZoomOut(QPointF pointF, double fScale)
{
}

void ImageViewQView::ZoomFit()
{
	if (!imageview_qscene_ || !imageview_qpix_)
	{
		return;
	}

	if (imageview_qpix_->pixmap().isNull())
	{
		return;
	}
	QRectF viewRect = geometry();
	imageview_qscene_->setSceneRect(0, 0, viewRect.width(), viewRect.height()); //将坐标原点设在显示窗口的左上角
	QPixmap pix = QPixmap::fromImage(qImage);
	QPixmap pix_sacle = pix.scaled(viewRect.width(), viewRect.height(), Qt::KeepAspectRatio);
	float w = pix_sacle.width()*1.0 / m_ImageWidth * 1.0;
	float h = pix_sacle.height()*1.0 / m_ImageHeight * 1.0;
	imageview_qpix_->SetFit(w);
	//设置居中显示位置
	int pWidth = pix_sacle.width();
	int pHeight = pix_sacle.height();
	int vWidth = viewRect.width();
	int vHeight = viewRect.height();
	if (vWidth > pWidth)
	{
		qreal fDeta_x = (vWidth - pWidth) / 2;
		imageview_qpix_->setPos(fDeta_x, 0);
	}
	else if (vHeight > pHeight)
	{
		qreal fDeta_y = (vHeight - pHeight) / 2;
		imageview_qpix_->setPos(0, fDeta_y);
	}
}

bool ImageViewQView::Open()
{
	bool bReturn = false;

	do
	{
		try
		{
			QString filename;
			filename = QFileDialog::getOpenFileName(this, tr("选择图像"), "", tr("Images (*.png *.bmp *.jpg *.tif *.GIF *.tiff)"));
			if (filename.isEmpty())
			{
				break;
			}
			else
			{
				std::string str = filename.toStdString();  // 将filename转变为string类型；
				m_Image = cv::imread(str);

				qImage = cvMat2QImage(m_Image);
				m_ImageWidth = qImage.width();
				m_ImageHeight = qImage.height();
				imageview_qpix_->setPixmap(QPixmap::fromImage(qImage));
#ifdef DEBUG
				qDebug() << "ImageWidth:" << m_ImageWidth << "ImageHeight:" << m_ImageHeight;
#endif

				//处理显示位置,默认为图像居中显示
				ZoomFit();
			}
		}
		catch (cv::Exception &e)
		{
			const char *err_msg = e.what();
			break;
		}

		bReturn = true;
	} while (false);

	return bReturn;
}

bool ImageViewQView::Save()
{
	bool bReturn = false;

	do
	{
		if (!qImage.isNull())
		{
			QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("*.bmp;; *.png;; *.jpg;; *.tif;; *.gif *.tiff")); //选择路径
			qImage.save(filename);
		}
		else
		{
			break;
		}

		bReturn = true;
	} while (false);

	return bReturn;
}
