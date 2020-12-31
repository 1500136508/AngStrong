#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_angstrong.h"
#include <thread>
#include "imageview.h"
#include "displayview.h"

class WidgetUI;
class TitleBar;
class EventHandlerGrabImage;
class EventHandlerMain;
class AngStrong : public QMainWindow
{
    Q_OBJECT

public:
    AngStrong(QWidget *parent = Q_NULLPTR);
	~AngStrong();

	void SetQssSheetStyle(QString style);
protected:
	void keyPressEvent(QKeyEvent * keyValue) override;
signals:
	void SendSN(QString sn);
private:
	void InitializeUI();
	void CreateCustomMainWindow();
	void SetDefaultQssStyle();
	void CreateDockWindow();
	void BuildConnect();
	void ReleasePointer();

	void CheckSNThread();

    Ui::AngStrongClass ui;
	WidgetUI *main_widgets_ui_;
	TitleBar *titlebar_ui_;
	ImageView imageview_;
	DisplayView dispview_;
	EventHandlerGrabImage *eventhandler_grabimage_ = nullptr;
	EventHandlerMain    *eventhandler_main_ = nullptr;

	bool is_quite_program_ = false;
	QString sn_;
	clock_t key_press_time_;
	bool is_key_press_;
	std::thread *sn_thread_;
};
