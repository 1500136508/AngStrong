#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_angstrong.h"
#include "imageview.h"
#include "displayview.h"

class WidgetUI;
class TitleBar;
class EventHandlerGrabImage;
class AngStrong : public QMainWindow
{
    Q_OBJECT

public:
    AngStrong(QWidget *parent = Q_NULLPTR);
	~AngStrong();

	void SetQssSheetStyle(QString style);
private:
	void InitializeUI();
	void CreateCustomMainWindow();
	void SetDefaultQssStyle();
	void CreateDockWindow();
	void BuildConnect();

    Ui::AngStrongClass ui;
	WidgetUI *main_widgets_ui_;
	TitleBar *titlebar_ui_;
	ImageView imageview_;
	DisplayView dispview_;
	EventHandlerGrabImage *eventhandler_grabimage_ = nullptr;
};
