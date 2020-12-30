#ifndef DISPLAYVIEW_H
#define DISPLAYVIEW_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class DisplayView;
}

class DisplayView : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayView(QWidget *parent = nullptr);
    ~DisplayView();
	void BuildConnect();
private slots:
	void ReceiveLocationDepth(int x, int y, float depth);
	void ReceiveImageGray(int r, int g, int b);
	void ReceiveAvgDepth(float avg0, float avg1);
	void ReceiveXMData(QString read_data);
	void ReadPSensorData(QString psensor_data);
	void ReceiveDeviceSNData(QString);
	void UpdataUIMessage();
private:
    Ui::DisplayView *ui;
	QTimer updata_ui_message_;
};

#endif // DISPLAYVIEW_H
