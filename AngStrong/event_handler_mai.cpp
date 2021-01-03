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
	WaitThreadFinished();
	CloseCamera();
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
	is_run_ = true;
}

void EventHandlerMain::CloseCamera()
{
	is_run_ = false;
	is_com_open_ = false;
	cc.close_comm();
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
		/*if (!is_com_open_)
		{
			Sleep(3);
			continue;
		}*/
		static int gg = 1000;
		is_read_psensor_finished = false;
		//std::lock_guard<std::mutex> locker(mutex_);
		std::string write_psensor_info("error:Failed to write psensor data!");
		std::string read_psensor_info("error:Failed to read psensor data!");
		std::string write_psensor_command = "ARG WRITE:DIST=1";
		std::string read_psensor_command = "ARG READ:DIST";
		/*cc.write_comm(write_psensor_command, write_psensor_info);
		cc.write_comm(read_psensor_command, read_psensor_info);*/
		test = gg;
		write_psensor_info = "=1";
		read_psensor_info = "="+std::to_string(test);
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
		if (gg >= 10000)
		{
			gg = 1000;
		}
		else
		{
			++gg;
		}
		is_read_psensor_finished = true;
		Sleep(1000);
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
		/*if (!is_com_open_)
		{
			Sleep(3);
			continue;
		}*/
		if (!is_write_device_sn_)
		{
			Sleep(3);
			continue;
		}
		static int gg = 10;
		is_write_device_sn_finished = false;
		//std::lock_guard<std::mutex> locker(mutex_);
		std::string write_device_sn_info("Failed to write data!");
		std::string write_device_sn_command = "ARG WRITE:SN=" + device_sn_;
		//cc.write_comm(write_device_sn_command, write_device_sn_info);
		test = gg;
		write_device_sn_info = device_sn_+"+"+std::to_string(test);
		int find_error_position = write_device_sn_info.find("error");
		if (find_error_position != -1)
		{
			LogManager::Write("Failed to Write Device SN!");
			emit SendDeviceSNData("Ð´ÈëÊ§°Ü£¡");
			continue;
		}
		LogManager::Write("Success to " + write_device_sn_info);
		emit SendDeviceSNData(QString::fromStdString(device_sn_));
		is_write_device_sn_ = false;
		if (gg >= 1000)
		{
			gg = 10;
		}
		else
		{
			++gg;
		}
		is_write_device_sn_finished = true;
		Sleep(1000);
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
		/*if (!is_com_open_)
		{
			Sleep(3);
			continue;
		}*/
		static int gg = 10000;
		is_read_device_sn_finished = false;
		//std::lock_guard<std::mutex> locker(mutex_);
		std::string read_device_sn_info("Failed to read data!");
		std::string read_device_sn_command = "ARG READ:SN";
		//cc.write_comm(read_device_sn_command, read_device_sn_info);
		test = gg;
		read_device_sn_info = "="+std::to_string(test);
		int find_error_position = read_device_sn_info.find("error");
		if (find_error_position != -1)
		{
			LogManager::Write("Failed to Write Device SN!");
			emit SendDeviceSNData("¶ÁÈ¡Ê§°Ü£¡");
			continue;
		}
		int find_sn_position = read_device_sn_info.find("=");
		auto iter_b = read_device_sn_info.cbegin() + find_sn_position + 1;
		auto iter_e = read_device_sn_info.cend();
		std::string read_sn(iter_b, iter_e);
		emit SendReadSN(QString::fromStdString(read_sn));
		is_write_device_sn_ = false;
		if (gg >= 20000)
		{
			gg = 10000;
		}
		else
		{
			++gg;
		}
		is_read_device_sn_finished = true;
		Sleep(1000);
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
