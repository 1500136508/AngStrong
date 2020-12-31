#pragma once
class EventHandler
{
public:
	EventHandler();
	~EventHandler();
public:
	virtual void OpenCamera(int com_port) = 0;
	virtual void CloseCamera() = 0;
	virtual void Run() = 0;
	virtual void Pause() = 0;
	virtual void Stop() = 0;

	virtual void MouseMoveInfo(int x, int y) = 0;
};

