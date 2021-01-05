#pragma once
#include <QWidget>
#include "DShow.h"
#include <qedit.h>
class CameraDS
{
public:
	enum ECapture_Mode
	{
		ECapture_Mode_Preview = 0,
		ECapture_Mode_Capture,
		ECapture_Mode_Counts,
	};
	enum ECameraStatus
	{
		ECameraStatus_Unknow = 0,
		ECameraStatus_Running,
		ECameraStatus_Pause,
		ECameraStatus_Stop,
		ECameraStatus_Counts,
	};

	explicit CameraDS();
	virtual ~CameraDS();

	bool Initialize();
	bool Terminate();

	bool Run();
	bool Pause();
	bool Stop();

	bool IsRunning()const {if (camera_status_ == ECameraStatus_Running) return true; else return false; }
	bool IsPause()const { if (camera_status_ == ECameraStatus_Pause) return true; else return false; }
	bool IsStop()const { if (camera_status_ == ECameraStatus_Stop) return true; else return false; }

	IPin* FindPin(IBaseFilter *filter, PIN_DIRECTION pin_direction);
	bool SetCameraFormat(int camera_id,const int width,const int height,bool is_YUV2=false);
	void SetCallBack(ISampleGrabberCB *callback_function);

	HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	void RemoveGraphFromRot(DWORD pdwRegister);

	void ShowCameraCaptureFilterDialog();
	void ShowCmaeraCapturePinDialog();

	int GetCameraWidth();
	int GetCameraHeight();

	bool CreateInterfaces();
	bool AddCaptureVideoFilter();
	bool RenderCamera(ECapture_Mode mode);
	void RemoveConnections(IBaseFilter *filter);
	bool SetCameraDevice(const int device_id);
	int CameraCount();
	std::string GetCameraName(const int device_id);
	bool GetCameraName(int nCamID, char* sName, int nBufferSize);
private:
	bool GetCameraDevice(const int device_id, IBaseFilter **device_filter);
	void ReleasePointer();

	IGraphBuilder *grap_builder_ = nullptr;//Graph builder object
	IMediaControl *media_control_ = nullptr;
	IMediaEventEx *media_event_ = nullptr;
	ICaptureGraphBuilder2 *capture_grap_builder2_ = nullptr;// Capture graph builder
	ISampleGrabber *sample_grabber_ = nullptr;// standard transform filter

	IBaseFilter *source_filter_ = nullptr;
	IBaseFilter *sample_grabber_filter_ = nullptr;
	IBaseFilter *render_filter_ = nullptr;
	IBaseFilter *video_mixing_render_filter_ = nullptr;  //视频渲染器

	IVideoWindow *video_window_ = nullptr;
	IAMStreamConfig *camera_stream_format_config_ = nullptr;    //流配置接口
	                                               
	bool is_init_;
	ECameraStatus camera_status_ = ECameraStatus_Unknow;
	DWORD g_dwGraphRegister_;

	ISampleGrabberCB *callback_function_;
};

