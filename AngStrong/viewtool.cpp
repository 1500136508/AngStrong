#include "viewtool.h"
#include "ui_viewtool.h"

ViewTool::ViewTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewTool)
{
    ui->setupUi(this);
	setWindowFlag(Qt::FramelessWindowHint);
}

ViewTool::~ViewTool()
{
    delete ui;
}
