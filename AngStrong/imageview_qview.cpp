#pragma execution_character_set("utf-8")
#include "imageview_qview.h"
#include <QMenu>
#include <QFileDialog>
#include "definitionmenu.h"
#include "imageview_qview.h"

ImageViewQView::ImageViewQView(QWidget *pParent /* = nullptr */)
	:QGraphicsView(pParent)
	,imageview_qscene_(std::make_shared<ImageViewQScene>(new ImageViewQScene(pParent)))
	,imageview_qpix_(std::make_shared<ImageViewQPixmap>(new ImageViewQPixmap()))
	,imageview_rect_item_(std::make_shared<GraphicsRectItem>(new GraphicsRectItem()))
	,imageview_toolbar_(std::make_shared<ImageViewQToolBar>(new ImageViewQToolBar()))
{
	InitializeUI();
}

ImageViewQView::ImageViewQView(QGraphicsScene *scene, QWidget *parent /* = nullptr */)
	: QGraphicsView(scene, parent)
	, imageview_qscene_(std::make_shared<ImageViewQScene>(scene))
	, imageview_qpix_(std::make_shared<ImageViewQPixmap>(new ImageViewQPixmap()))
	, imageview_rect_item_(std::make_shared<GraphicsRectItem>(new GraphicsRectItem()))
	, imageview_toolbar_(std::make_shared<ImageViewQToolBar>(new ImageViewQToolBar()))
{
	InitializeUI();
}

ImageViewQView::~ImageViewQView()
{
	ReleasePointer();
}

void ImageViewQView::mouseMoveEvent(QMouseEvent * event)
{
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

void ImageViewQView::contextMenuEvent(QContextMenuEvent * event)
{
	CreateCustomRightButtonMenu();
}

void ImageViewQView::enterEvent(QEvent * event)
{
	imageview_toolbar_->setParent(this);
	QRect view_size = geometry();
	imageview_toolbar_->resize(QSize(view_size.width(), 25));
	imageview_toolbar_->move(0, 0);
	imageview_toolbar_->show();
}

void ImageViewQView::leaveEvent(QEvent * event)
{
	imageview_toolbar_->hide();
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

void ImageViewQView::InitializeUI()
{
	setAttribute(Qt::WA_DeleteOnClose);
	imageview_qscene_->addItem(imageview_qpix_.get());//添加像元元素绑定到Scene
	setScene(imageview_qscene_.get());//将QGraphicsView添加Scene;
	setMouseTracking(true);
	imageview_toolbar_->hide();
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
