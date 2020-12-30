#ifndef IMAGEVIEW_QTOOLBAR_H
#define IMAGEVIEW_QTOOLBAR_H

#include <QWidget>
#include <list>

namespace Ui {
class ImageViewQToolBar;
}

class ImageViewQToolBar : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewQToolBar(QWidget *parent = nullptr);
    ~ImageViewQToolBar();

	void SetQssSheetStyle(QString sheet_style);
	void FindAllPort();
	void InitialzeCameraList(std::list<std::string> camera_list);
	int get_current_camera_index();
	int get_current_display_mode();
	int get_current_grab_mode();
private slots:
	void on_open_camera_triggered();
	void on_close_camera_triggered();
	void on_live_triggered();
	void on_pause_triggered();
	void on_stop_triggered();
	void on_capture_filter_triggered();
	void on_capture_pin_triggered();
signals:
	void send_open_camera_triggered();
	void send_close_camera_triggered();
	void send_live_triggered();
	void send_pause_triggered();
	void send_stop_triggered();
	void send_capture_filter_triggered();
	void send_capture_pin_triggered();
private:
	void InitializeUI();
	void InitializeCombo();
	void BuildConnect();

    Ui::ImageViewQToolBar *ui;
};

#endif // IMAGEVIEW_QTOOLBAR_H
