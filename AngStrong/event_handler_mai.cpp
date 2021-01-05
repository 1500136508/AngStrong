#pragma execution_character_set("utf-8")
#include "event_handler_mai.h"
#include "HalerThread.h"
#include "logmanager.h"
#include "definition_thread.h"

EventHandlerMain::EventHandlerMain()
{
	BuildConnect();

	std::thread read_psensor_thread(&EventHandlerMain::ReadPSensorThread, this);
	std::thread write_device_sn_thread(&EventHandlerMain::WriteDeviceSNThread, this);
	std::thread read_device_sn_thread(&EventHandlerMain::ReadDeviceSNThread, this);
	read_psensor_thread.detach();
	write_device_sn_thread.detach();
	read_device_sn_thread.detach();
}

EventHandlerMain::~EventHandlerMain()
{
	quite_program = true;
	is_run_ = false;
	CloseCamera();
	WaitThreadFinished();
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
			std::string write_psensor_info("error:Failed to write psensor data!");
			std::string write_psensor_command = "ARG WRITE:DIST=1";
			cc.write_comm(write_psensor_command, write_psensor_info);
			is_ready_to_read_device_sn_ = true;
			is_ready_to_read_psensor_ = true;
			read_psensor_.start(5000);
		}
	}
	is_run_ = true;
}

void EventHandlerMain::CloseCamera()
{
	is_run_ = false;
	is_com_open_ = false;
	WaitThreadFinished();
	cc.close_comm();
	read_psensor_.stop();

	emit SendPSensorData("--");
	emit SendWriteSN("--");
	emit SendReadSN("--");
}

void EventHandlerMain::Run()
{
	is_run_ = true;
}

void EventHandlerMain::Pause()
{
	is_run_ = false;
}

void EventHandlerMain::Stop()
{
	is_run_ = false;
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
	bool ret = connect(&read_psensor_, SIGNAL(timeout()), this, SLOT(ReadPSensorTimer()));
}

void EventHandlerMain::ReadPSensorTimer()
{
	is_ready_to_read_psensor_ = true;
	is_ready_to_read_device_sn_ = true;
}

void EventHandlerMain::ReadPSensorThread()
{
	while (!quite_program)
	{
		if (!is_run_)
		{
			Sleep(3);
			continue;
		}
		if (!is_com_open_)
		{
			Sleep(3);
			continue;
		}
		if (!is_ready_to_read_psensor_)
		{
			Sleep(3);
			continue;
		}
		is_read_psensor_finished = false;
		std::unique_lock<std::mutex> locker(mutex_);
		
		std::string read_psensor_info("error:Failed to read psensor data!");
		std::string read_psensor_command = "ARG READ:DIST";
		cc.write_comm(read_psensor_command, read_psensor_info);
		locker.unlock();
		int find_error_position = read_psensor_info.find("error");
		if (find_error_position != -1)
		{
			emit SendPSensorData("--");
			is_read_psensor_finished = true;
			is_ready_to_read_psensor_ = false;
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
				is_read_psensor_finished = true;
				is_ready_to_read_psensor_ = false;
			}
		}
		is_read_psensor_finished = true;
		is_ready_to_read_psensor_ = false;
	}
}

void EventHandlerMain::WriteDeviceSNThread()
{
	while (!quite_program)
	{
		if (!is_run_)
		{
			Sleep(3);
			continue;
		}
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
		is_write_device_sn_finished = false;
		is_ready_to_read_device_sn_ = false;
		std::unique_lock<std::mutex> locker(mutex_);
		std::string write_device_sn_info("Failed to write data!");
		std::string write_device_sn_command = "ARG WRITE:SN=" + device_sn_;
		cc.write_comm(write_device_sn_command, write_device_sn_info);
		locker.unlock();
		int find_error_position = write_device_sn_info.find("error");
		if (find_error_position != -1)
		{
			LogManager::Write("Failed to Write Device SN!");
			emit SendWriteSN("–¥»Î ß∞‹£°");
			is_write_device_sn_finished = true;
			continue;
		}
		LogManager::Write("Success to " + write_device_sn_info);
		emit SendWriteSN(QString::fromStdString(device_sn_));
		is_write_device_sn_ = false;
		is_write_device_sn_finished = true;
		is_ready_to_read_device_sn_ = true;
	}
}

void EventHandlerMain::ReadDeviceSNThread()
{
	while (!quite_program)
	{
		if (!is_run_)
		{
			Sleep(3);
			continue;
		}
		if (!is_com_open_)
		{
			Sleep(3);
			continue;
		}
		if (!is_ready_to_read_device_sn_)
		{
			Sleep(3);
			continue;
		}
		is_read_device_sn_finished = false;
		std::unique_lock<std::mutex> locker(mutex_);
		std::string read_device_sn_info("error:Failed to read data!");
		std::string read_device_sn_command = "ARG READ:SN";
		cc.write_comm(read_device_sn_command, read_device_sn_info);
		locker.unlock();
		int find_error_position = read_device_sn_info.find("error");
		if (find_error_position != -1)
		{
			LogManager::Write("Failed to Write Device SN!");
			emit SendReadSN("∂¡»° ß∞‹£°");
			is_write_device_sn_ = false;
			is_read_device_sn_finished = true;
			is_ready_to_read_device_sn_ = false;
			continue;
		}
		int find_sn_position = read_device_sn_info.find("=");
		auto iter_b = read_device_sn_info.cbegin() + find_sn_position + 1;
		auto iter_e = read_device_sn_info.cend()-2;
		std::string read_sn(iter_b, iter_e);
		emit SendReadSN(QString::fromStdString(read_sn));
		is_write_device_sn_ = false;
		is_read_device_sn_finished = true;
		is_ready_to_read_device_sn_ = false;
	}
}

void EventHandlerMain::WaitThreadFinished(long timeout)
{
	clock_t wait_start_time = clock();
	clock_t wait_time;
	while (true)
	{
		wait_time = clock() - wait_start_time;
		if (wait_time >= timeout)
		{
			break;
		}
		else if(is_read_psensor_finished && is_write_device_sn_finished
			&& is_read_device_sn_finished)
		{
			break;
		}
		else
		{
			Sleep(3);
			continue;
		}
	}
}
