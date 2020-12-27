#ifndef VIEWTOOL_H
#define VIEWTOOL_H

#include <QWidget>

namespace Ui {
class ViewTool;
}

class ViewTool : public QWidget
{
    Q_OBJECT

public:
    explicit ViewTool(QWidget *parent = nullptr);
    ~ViewTool();

private:
    Ui::ViewTool *ui;
};

#endif // VIEWTOOL_H
