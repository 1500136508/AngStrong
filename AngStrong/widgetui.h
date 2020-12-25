#ifndef WIDGETUI_H
#define WIDGETUI_H

#include <QWidget>

namespace Ui {
class WidgetUI;
}

class WidgetUI : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetUI(QWidget *parent = nullptr);
    ~WidgetUI();
protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private:
	void SetWindowsProperty();

    Ui::WidgetUI *ui;
};

#endif // WIDGETUI_H
