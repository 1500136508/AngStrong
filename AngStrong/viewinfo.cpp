#include "viewinfo.h"
#include "ui_viewinfo.h"

ViewInfo::ViewInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewInfo)
{
    ui->setupUi(this);
	setWindowFlag(Qt::FramelessWindowHint);
}

ViewInfo::~ViewInfo()
{
    delete ui;
}
