#include "event_handler_mai.h"
#include "HalerThread.h"
#include "logmanager.h"
#include "definition_thread.h"

EventHandlerMain::EventHandlerMain()
{
	BuildConnect();
	CreateHalerThread();
}

EventHandlerMain::~EventHandlerMain()
{
	quite_program = true;
	CloseCamera();
	CHalerThread::Terminate(EThreadSequence_pContext_ReadPSensor);
	CHalerThread::Terminate(EThreadSequence_pContext_WriteSN);
	CHalerThread::Terminate(EThreadSequence_pContext_ReadSN);
}

void EventHandlerMain::OpenCamera(int com_port)
{
	if (!is_com_open_)
	{
		if (cc.init_comm(com_port) != 0)
		{
			std::string log_msg_ = "fail to open the port!";
			LogManager::Write(log_msg_);
		}
		else
		{
			is_com_open_ = true;
		}
	}
}

void EventHandlerMain::CloseCamera()
{
	is_com_open_ = false;
	cc.close_comm();
}

void EventHandlerMain::Run()
{
	CHalerThread::Resume(EThreadSequence_pContext_ReadPSensor);
	CHalerThread::Resume(EThreadSequence_pContext_WriteSN);
	CHalerThread::Resume(EThreadSequence_pContext_ReadSN);
}

void EventHandlerMain::Pause()
{
	CHalerThread::Suspend(EThreadSequence_pContext_ReadPSensor);
	CHalerThread::Suspend(EThreadSequence_pContext_WriteSN);
	CHalerThread::Suspend(EThreadSequence_pContext_ReadSN);
}

void EventHandlerMain::Stop()
{
	CHalerThread::Suspend(EThreadSequence_pContext_ReadPSensor);
	CHalerThread::Suspend(EThreadSequence_pContext_WriteSN);
	CHalerThread::Suspend(EThreadSequence_pContext_ReadSN);
}

void EventHandlerMain::MouseMoveInfo(int x, int y)
{
	emit SendMouseInfo(x, y);
}

void EventHandlerMain::ReceiveReadXMData(bool read_xm_data, int com_index)
{
}

void EventHandlerMain::ReceiveSN(QString sn)
{
	device_sn_ = sn.toStdString();
	is_write_device_sn_ = true;
}

void EventHandlerMain::BuildConnect()
{
}

void EventHandlerMain::CreateHalerThread()
{
	CreateHalerThreadContext(pContext_ReadPSensor)
	{
		while (!quite_program)
		{
			if (!is_com_open_)
			{
				Sleep(3);
				continue;
			}
			std::unique_lock<std::mutex> locker(mutex_);
			locker.unlock();
		
			std::string write_psensor_info("error:Failed to write psensor data!");
			std::string read_psensor_info("error:Failed to read psensor data!");
			std::string write_psensor_command = "ARG WRITE:DIST=1";
			std::string read_psensor_command = "ARG READ:DIST";
			locker.lock();
			cc.write_comm(write_psensor_command, write_psensor_info);
			cc.write_comm(read_psensor_command, read_psensor_info);
			locker.unlock();
			int find_error_position = read_psensor_info.find("error");
			if (find_error_position != -1)
			{
				emit SendPSensorData("--");
				continue;
			}
			auto find_position = read_psensor_info.find("=");
			if (find_position >= 0)
			{
				auto iter_b = read_psensor_info.cbegin() + find_position + 1;
				auto iter_e = read_psensor_info.cend();
				std::string psensor(iter_b, iter_e);
				try
				{
					emit SendPSensorData(QString::number(std::stoi(psensor)));
				}
				catch (...)
				{
					std::string err_msg = "error";
				}
			}
		}
	};
	CreateHalerThreadContext(pContext_WriteSN)
	{
		while (!quite_program)
		{
			if (!is_com_open_)
			{
				Sleep(3);
				continue;
			}
			if (!is_write_device_sn_)
			{
				Sleep(3);
				continue;
			}
			std::unique_lock<std::mutex> locker(mutex_);
			locker.unlock();
			
			std::string write_device_sn_info("Failed to write data!");
			std::string write_device_sn_command = "ARG WRITE:SN=" + device_sn_;
			locker.lock();
			cc.write_comm(write_device_sn_command, write_device_sn_info);
			locker.unlock();
			int find_error_position = write_device_sn_info.find("error");
			if (find_error_position != -1)
			{
				LogManager::Write("Failed to Write Device SN!");
				emit SendDeviceSNData("–¥»Î ß∞‹£°");
				continue;
			}
			locker.lock();
			LogManager::Write("Success to " + write_device_sn_info);
			emit SendDeviceSNData(QString::fromStdString(device_sn_));
			is_write_device_sn_ = false;
			locker.unlock();
		}
	};
	CreateHalerThreadContext(pContext_ReadSN)
	{
		while (!quite_program)
		{
			if (!is_com_open_)
			{
				Sleep(3);
				continue;
			}
			std::unique_lock<std::mutex> locker(mutex_);
			locker.unlock();

			std::string read_device_sn_info("Failed to read data!");
			std::string read_device_sn_command = "ARG READ:SN";
			locker.lock();
			cc.write_comm(read_device_sn_command, read_device_sn_info);
			locker.unlock();
			int find_error_position = read_device_sn_info.find("error");
			if (find_error_position != -1)
			{
				LogManager::Write("Failed to Write Device SN!");
				emit SendDeviceSNData("∂¡»° ß∞‹£°");
				continue;
			}
			/*locker.lock();
			int find_sn_position = read_device_sn_info.find("=");
			auto iter_b = read_device_sn_info.cbegin() + find_sn_position + 1;
			auto iter_e = read_device_sn_info.cend();
			std::string read_sn(iter_b, iter_e);
			emit SendReadSN(QString::fromStdString(read_sn));
			is_write_device_sn_ = false;
			locker.unlock();*/
		}
	};
	/*CHalerThread::Run(pContext_ReadPSensor, EThreadSequence_pContext_ReadPSensor, false);
	CHalerThread::Run(pContext_WriteSN, EThreadSequence_pContext_WriteSN, false);
	CHalerThread::Run(pContext_ReadSN, EThreadSequence_pContext_ReadSN, false);*/
}
