#ifndef VIEWINFO_H
#define VIEWINFO_H

#include <QWidget>

namespace Ui {
class ViewInfo;
}

class ViewInfo : public QWidget
{
    Q_OBJECT

public:
    explicit ViewInfo(QWidget *parent = nullptr);
    ~ViewInfo();

private:
    Ui::ViewInfo *ui;
};

#endif // VIEWINFO_H
