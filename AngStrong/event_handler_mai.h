#pragma once
#include <QObject>
#include <thread>
#include <mutex>
#include <QTimer>
#include "eventhandler.h"
#include "otp_interface.h"
class EventHandlerMain :public QObject ,public EventHandler
{
	Q_OBJECT
public:
	EventHandlerMain();
	~EventHandlerMain();
public:
	void OpenCamera(int com_port)override;
	void CloseCamera()override;
	void Run()override;
	void Pause()override;
	void Stop()override;
	void OnDisplayModeChange(int current_mode)override;
	void MouseMoveInfo(int x, int y)override;

	void GetImageBuffer(double Time, BYTE * pBuffer, long BufferLen,ImageViewQView *view)override;
private slots:
	void ReceiveReadXMData(bool read_xm_data, int com_index);
	void ReceiveSN(QString sn);
	void ReadPSensorTimer();
signals:
	void SendPSensorData(QString psensor_data);
	void SendWriteSN(QString message);
	void SendReadSN(QString sn);
	void SendMouseInfo(int x, int y);
private:
	void BuildConnect();
	void ReadPSensorThread();
	void WriteDeviceSNThread();
	void ReadDeviceSNThread();
	void WaitThreadFinished(long timeout = 5000);

	com_controler cc;
	std::mutex mutex_;
	bool quite_program = false;
	
	int com_index_ = -1;
	std::string device_sn_;
	volatile bool is_run_ = false;
	volatile bool is_com_open_ = false;
	volatile bool is_read_psensor_ = true;
	volatile bool is_write_device_sn_ = false;
	volatile bool is_read_device_sn_ = false;
	bool is_read_psensor_finished = true;
	bool is_write_device_sn_finished = true;
	bool is_read_device_sn_finished = true;
	bool is_ready_to_read_device_sn_ = false;
	bool is_ready_to_read_psensor_ = false;

	QTimer read_psensor_;
};

