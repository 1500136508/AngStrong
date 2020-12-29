#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>

namespace Ui {
class TitleBar;
}

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar();

	void BuildConnect();
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent *event)override;
	virtual void mousePressEvent(QMouseEvent *event)override;
protected slots:
	void on_btn_min_clicked();
	void on_btn_max_clicked();
	void on_btn_close_clicked();
private:

    Ui::TitleBar *ui;
};

#endif // TITLEBAR_H
