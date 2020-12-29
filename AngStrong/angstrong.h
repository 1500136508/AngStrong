#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_angstrong.h"
#include "imageview.h"

class WidgetUI;
class TitleBar;
class AngStrong : public QMainWindow
{
    Q_OBJECT

public:
    AngStrong(QWidget *parent = Q_NULLPTR);
	~AngStrong();
private:
	void ModifyUI();
	void CreateCustomMainWindow();
	void SetQssStyle(QString style);
	void SetDefaultQssStyle();

    Ui::AngStrongClass ui;
	WidgetUI *main_widgets_ui_;
	TitleBar *titlebar_ui_;
	ImageView imageview_;
};
