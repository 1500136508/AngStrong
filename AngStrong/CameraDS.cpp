#include "CameraDS.h"
#include <atlbase.h>
#include <string>
#include <comutil.h>

#define MYFREEMEDIATYPE(mt)	{if ((mt).cbFormat != 0)		\
                    {CoTaskMemFree((PVOID)(mt).pbFormat);	\
                    (mt).cbFormat = 0;						\
                    (mt).pbFormat = NULL;					\
					 }											\
					 if ((mt).pUnk != NULL)						\
					 {											\
                    (mt).pUnk->Release();					\
                    (mt).pUnk = NULL;						\
					  }}

CameraDS::CameraDS()
{
	is_init_ = false;

	grap_builder_ = nullptr;
	media_control_ = nullptr;
	media_event_ = nullptr;
	capture_grap_builder2_ = nullptr;
	sample_grabber_ = nullptr;

	source_filter_ = nullptr;
	sample_grabber_filter_ = nullptr;
	render_filter_ = nullptr;

	video_window_ = nullptr;
	camera_stream_format_config_ = nullptr;

	callback_function_ = nullptr;
}

CameraDS::~CameraDS()
{
	Terminate();
}

bool CameraDS::Initialize()
{
	bool bReturn = false;
	do 
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if (FAILED(hr))
		{
			std::string err_msg = "Failed to CoInitialize Com Evironment";
			break;
		}

		if (!CreateInterfaces())
			break;

		is_init_ = true;
		bReturn = true;
	} while (false);
	return bReturn;
}

bool CameraDS::Terminate()
{
	bool bReturn = false;
	do
	{
		if (is_init_)
		{
			if (IsRunning())
			{
				Stop();
			}
			if (grap_builder_)
			{
				grap_builder_->Release();
				grap_builder_ = nullptr;
			}
			if (media_control_)
			{
				media_control_->Release();
				media_control_ = nullptr;
			}
			if (media_event_)
			{
				media_event_->Release();
				media_event_ = nullptr;
			}
			if (capture_grap_builder2_)
			{
				capture_grap_builder2_->Release();
				capture_grap_builder2_ = nullptr;
			}
			if (sample_grabber_)
			{
				sample_grabber_->Release();
				sample_grabber_ = nullptr;
			}
			if (source_filter_)
			{
				source_filter_->Release();
				source_filter_ = nullptr;
			}
			if (sample_grabber_filter_)
			{
				sample_grabber_filter_->Release();
				sample_grabber_filter_ = nullptr;
			}
			if (render_filter_)
			{
				render_filter_->Release();
				render_filter_ = nullptr;
			}
			if (video_window_)
			{
				video_window_->Release();
				video_window_ = nullptr;
			}
			if (camera_stream_format_config_)
			{
				camera_stream_format_config_->Release();
				camera_stream_format_config_ = nullptr;
			}

			CoUninitialize();
			RemoveGraphFromRot(g_dwGraphRegister_);
			is_init_ = false;
		}
		bReturn = true;
	} while (false);
	return bReturn;
}

bool CameraDS::Run()
{
	bool bReturn = false;
	do 
	{
		if (!is_init_)
			break;
		if (media_control_)
		{
			HRESULT hr = S_FALSE;
			hr = media_control_->Run();
			if (FAILED(hr))
			{
				// stop parts that started
				media_control_->Stop();
				break;
			}
			camera_status_ = ECameraStatus_Running;
		}
		bReturn = true;
	} while (false);
	return bReturn;
}

bool CameraDS::Pause()
{
	bool bReturn = false;
	do
	{
		if (!is_init_)
			break;
		if (media_control_)
		{
			HRESULT hr = S_FALSE;
			hr = media_control_->Pause();
			if (FAILED(hr))
			{
				// stop parts that started
				media_control_->Stop();
				break;
			}
			camera_status_ = ECameraStatus_Pause;
		}
		bReturn = true;
	} while (false);
	return bReturn;
}

bool CameraDS::Stop()
{
	bool bReturn = false;
	do
	{
		if (!is_init_)
			break;
		if (media_control_)
		{
			HRESULT hr = S_FALSE;
			hr = media_control_->Stop();
			if (FAILED(hr))
				break;
		}
		camera_status_ = ECameraStatus_Stop;
		bReturn = true;
	} while (false);
	return bReturn;
}

IPin * CameraDS::FindPin(IBaseFilter * filter, PIN_DIRECTION pin_direction)
{
	HRESULT hr = S_FALSE;
	IEnumPins* pEnumPins = nullptr;
	IPin* find_pin = nullptr;
	PIN_DIRECTION pin_direction_current;
	do 
	{
		if (!filter)
			break;

		hr = filter->EnumPins(&pEnumPins);
		if (FAILED(hr))
			break;

		while (pEnumPins->Next(1, &find_pin, NULL) == S_OK)
		{
			find_pin->QueryDirection(&pin_direction_current);
			if (pin_direction_current == pin_direction)
			{
				break;
			}
			else
			{
				find_pin = nullptr;
			}
		}
	} while (false);
	return find_pin;
}

bool CameraDS::SetCameraFormat(int camera_id, const int width, const int height, bool is_YUV2)
{
	bool bReturn = false;
	do 
	{
		Stop();
		RemoveConnections(source_filter_);
		HRESULT hr = S_FALSE;
		IAMStreamConfig *iconfig = nullptr;
		IPin *camera_output_pin = FindPin(source_filter_, PINDIR_OUTPUT);
		if (!camera_output_pin)
			break;

		hr = camera_output_pin->QueryInterface(IID_IAMStreamConfig, (void**)&iconfig);
		if (FAILED(hr))
			break;

		AM_MEDIA_TYPE   mt;
		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
		mt.majortype = MEDIATYPE_Video;
		if (is_YUV2) mt.subtype = MEDIASUBTYPE_YUY2;
		else mt.subtype = MEDIASUBTYPE_MJPG;
		mt.formattype = FORMAT_VideoInfo;
		hr = sample_grabber_->SetMediaType(&mt);
		MYFREEMEDIATYPE(mt);

		VIDEO_STREAM_CONFIG_CAPS scc;
		AM_MEDIA_TYPE *pmtConfig = nullptr;
		hr = iconfig->GetStreamCaps(camera_id, &pmtConfig, (BYTE *)&scc);
		if (FAILED(hr))
			break;
		if (is_YUV2)
		{
			VIDEOINFOHEADER *phead = (VIDEOINFOHEADER*)(pmtConfig->pbFormat);
			phead->bmiHeader.biWidth = width;
			phead->bmiHeader.biHeight = height;
			phead->bmiHeader.biCompression = MAKEFOURCC('Y', 'U', 'Y', '2');
		}
		else
		{
			VIDEOINFOHEADER *phead = (VIDEOINFOHEADER*)(pmtConfig->pbFormat);
			phead->bmiHeader.biWidth = width;
			phead->bmiHeader.biHeight = height;
		}
		hr = iconfig->SetFormat(pmtConfig);
		if (FAILED(hr))
			break;
		RenderCamera(ECapture_Mode_Capture);

		if (iconfig)
			iconfig->Release();
		if (camera_output_pin)
			camera_output_pin->Release();
		MYFREEMEDIATYPE(*pmtConfig);

		bReturn = true;
	} while (false);
	return bReturn;
}

void CameraDS::SetCallBack(ISampleGrabberCB *callback_function)
{
	sample_grabber_->SetBufferSamples(TRUE);
	sample_grabber_->SetOneShot(FALSE);
	if (!callback_function_)
	{
		callback_function_ = callback_function;
	}
	sample_grabber_->SetCallback(callback_function_, 1);
}

HRESULT CameraDS::AddGraphToRot(IUnknown * pUnkGraph, DWORD * pdwRegister)
{
	IMoniker *pMoniker = nullptr;
	IRunningObjectTable *pROT = nullptr;
	WCHAR wsz[128];
	HRESULT hr;

	if (!pUnkGraph || !pdwRegister)
		return E_POINTER;

	if (FAILED(GetRunningObjectTable(0, &pROT)))
		return E_FAIL;

	wsprintfW(wsz, L"FilterGraph %08x pid %08x\0", (DWORD_PTR)pUnkGraph,
		GetCurrentProcessId());

	hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if (SUCCEEDED(hr))
	{
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,
			pMoniker, pdwRegister);
		pMoniker->Release();
	}

	pROT->Release();
	return hr;
}

void CameraDS::RemoveGraphFromRot(DWORD pdwRegister)
{
	IRunningObjectTable *pROT;

	if (SUCCEEDED(GetRunningObjectTable(0, &pROT)))
	{
		pROT->Revoke(pdwRegister);
		pROT->Release();
	}
}

void CameraDS::ShowCameraCaptureFilterDialog()
{
	if (!is_init_)
		return;

	if (source_filter_ != NULL)
	{
		ISpecifyPropertyPages *pSpec;
		CAUUID cauuid;
		HRESULT hr;
		HWND ghwndApp = NULL;

		hr = source_filter_->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
		if (SUCCEEDED(hr))
		{
			hr = pSpec->GetPages(&cauuid);       //显示该页
			hr = OleCreatePropertyFrame(ghwndApp,   //父窗口
				30, 30,                      //Reserved
				NULL,                          //对话框标题
				1,                           //该滤波器的目标数目
				(IUnknown **)&source_filter_,         //目标指针数组
				cauuid.cElems,              //属性页数目
				(GUID *)cauuid.pElems,       //属性页的CLSID数组
				0,                            //本地标识
				0, NULL);                    //Reserved
			CoTaskMemFree(cauuid.pElems);     //释放内存、资源
			pSpec->Release();
		}
	}
}

void CameraDS::ShowCmaeraCapturePinDialog()
{
	if (!is_init_)
		return;

	HRESULT hr;       //返回值
	camera_stream_format_config_ = NULL;
	ISpecifyPropertyPages *pSpec = NULL;  //属性页接口
	HWND ghwndApp = 0;
	//只有停止后，才能进行引脚属性的设置
	media_control_->Stop();
	//RemoveConnections(source_filter_);

	hr = capture_grap_builder2_->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved, source_filter_,
		IID_IAMStreamConfig, (void **)&camera_stream_format_config_);
	if (hr != NOERROR)
		hr = capture_grap_builder2_->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, source_filter_,
			IID_IAMStreamConfig, (void **)&camera_stream_format_config_);
	CAUUID cauuid;       //所有属性页结构体
	hr = camera_stream_format_config_->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
	if (hr == S_OK)
	{
		hr = pSpec->GetPages(&cauuid);  //获取所有属性页
		//显示属性页
		hr = OleCreatePropertyFrame(ghwndApp, 30, 30, NULL, 1,
			(IUnknown **)&camera_stream_format_config_, cauuid.cElems,
			(GUID *)cauuid.pElems, 0, 0, NULL);
		//释放内存、资源
		CoTaskMemFree(cauuid.pElems);
		pSpec->Release();
		camera_stream_format_config_->Release();
	}
	RenderCamera(ECapture_Mode_Capture);
	//SetCallBack(callback_function_);
	Run();
}

int CameraDS::GetCameraWidth()
{
	if (!is_init_)
	{
		return -1;
	}
	AM_MEDIA_TYPE mt;
	HRESULT hr = sample_grabber_->GetConnectedMediaType(&mt);
	if (FAILED(hr))
	{
		return -1;
	}

	VIDEOINFOHEADER *videoHeader;
	videoHeader = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
	return videoHeader->bmiHeader.biWidth;
}

int CameraDS::GetCameraHeight()
{
	if (!is_init_)
	{
		return -1;
	}
	AM_MEDIA_TYPE mt;
	HRESULT hr = sample_grabber_->GetConnectedMediaType(&mt);
	if (FAILED(hr))
	{
		return -1;
	}

	VIDEOINFOHEADER *videoHeader;
	videoHeader = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
	return videoHeader->bmiHeader.biHeight;
}

void CameraDS::RemoveConnections(IBaseFilter * filter)
{
	IPin *pin = nullptr;
	IPin *to_be_connected = nullptr;
	ULONG u;
	IEnumPins *pins = nullptr;
	PIN_INFO pin_info;

	if (!filter)
		return;

	HRESULT hr = filter->EnumPins(&pins);
	if (FAILED(hr) || !pins)
	{
		return;
	}
	pins->Reset();

	while (hr == NOERROR)
	{
		hr = pins->Next(1, &pin, &u);
		if (hr == S_OK && pin)
		{
			pin->ConnectedTo(&to_be_connected);
			if (to_be_connected)
			{
				hr = to_be_connected->QueryPinInfo(&pin_info);
				if (hr == NOERROR)
				{
					if (pin_info.dir == PINDIR_INPUT)
					{
						RemoveConnections(pin_info.pFilter);
						grap_builder_->Disconnect(to_be_connected);
						grap_builder_->Disconnect(pin);
						grap_builder_->RemoveFilter(pin_info.pFilter);
					}
					pin_info.pFilter->Release();
				}
				to_be_connected->Release();
			}
			pin->Release();
		}
	}

	if (pins)
	{
		pins->Release();
	}
}

bool CameraDS::SetCameraDevice(const int device_id)
{
	bool bReturn = false;
	do 
	{
		if (!GetCameraDevice(device_id, &source_filter_))
			break;
		bReturn = true;
	} while (false);
	return bReturn;
}

bool CameraDS::CreateInterfaces()
{
	bool bReturn = false;
	do 
	{
		HRESULT hr;

		if (!grap_builder_)
		{
			hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
				IID_IGraphBuilder, (void **)&grap_builder_);
			if (FAILED(hr))
				break;
		}

		if (!capture_grap_builder2_)
		{
			hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
				IID_ICaptureGraphBuilder2, (void **)&capture_grap_builder2_);
			if (FAILED(hr))
				break;
		}
		hr = capture_grap_builder2_->SetFiltergraph(grap_builder_);
		if (FAILED(hr))
			break;

		if (!sample_grabber_filter_)
		{
			hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, 
				IID_IBaseFilter, (LPVOID *)&sample_grabber_filter_);
			if (FAILED(hr))
				break;
		}

		if (!render_filter_)
		{
			hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
				IID_IBaseFilter, reinterpret_cast<void**>(&render_filter_));
			if (FAILED(hr))
				break;
		}

		hr = grap_builder_->QueryInterface(IID_IMediaControl, (LPVOID *)&media_control_);
		if (FAILED(hr))
			break;

		hr = grap_builder_->QueryInterface(IID_IMediaEvent, (LPVOID *)&media_event_);
		if (FAILED(hr))
			break;

		hr = sample_grabber_filter_->QueryInterface(IID_ISampleGrabber, (void**)&sample_grabber_);
		if (FAILED(hr))
			break;

		bReturn = true;
	} while (false);
	return bReturn;
}

bool CameraDS::AddCaptureVideoFilter()
{
	bool bReturn = false;
	do 
	{
		HRESULT hr = S_FALSE;
		hr = grap_builder_->AddFilter(source_filter_, L"Camera_Device");
		if (FAILED(hr))
			break;

		hr = grap_builder_->AddFilter(sample_grabber_filter_, L"Sample_Grabber");
		if (FAILED(hr))
			break;

		hr = grap_builder_->AddFilter(render_filter_, L"NullRenderer");
		if (FAILED(hr))
			break;

		AddGraphToRot(grap_builder_, &g_dwGraphRegister_);
		bReturn = true;
	} while (false);
	return bReturn;
}

bool CameraDS::RenderCamera(ECapture_Mode mode)
{
	bool bReturn = false;
	do 
	{
		if (!is_init_)
			break;

		IPin *camera_output_pin = FindPin(source_filter_, PINDIR_OUTPUT);
		if (!camera_output_pin)
			break;
		IPin *sample_grabber_input = FindPin(sample_grabber_filter_, PINDIR_INPUT);
		if (!sample_grabber_input)
			break;
		IPin *sample_grabber_output = FindPin(sample_grabber_filter_, PINDIR_OUTPUT);
		if (!sample_grabber_output)
			break;
		IPin *render_input = FindPin(render_filter_, PINDIR_INPUT);
		if (!render_input)
			break;
		/*switch (mode)
		{
		case CameraDS::ECapture_Mode_Preview:
		{
			capture_grap_builder2_->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
				camera_output_pin, NULL, NULL);
		}
			break;
		case CameraDS::ECapture_Mode_Capture:
		{
			capture_grap_builder2_->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
				camera_output_pin, NULL, NULL);
		}
			break;
		default:
			break;
		}*/

		grap_builder_->Connect(camera_output_pin, sample_grabber_input);
		grap_builder_->Connect(sample_grabber_output, render_input);
		bReturn = true;
	} while (false);
	return bReturn;
}

bool CameraDS::GetCameraDevice(const int device_id, IBaseFilter ** device_filter)
{
	bool bReturn = false;
	do 
	{
		HRESULT hr;

		CComPtr<ICreateDevEnum> device_enum = nullptr;
		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
			IID_ICreateDevEnum, (void **)&device_enum);
		if (FAILED(hr))
		{
			std::string err_msg = "Couldn't create system enumerator!";
			break;
		}

		CComPtr<IEnumMoniker> enum_moniker = nullptr;
		hr = device_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enum_moniker, 0);
		if (FAILED(hr))
		{
			std::string err_msg = "Couldn't create class enumerator!";
			break;
		}
		if (!enum_moniker)
		{
			std::string err_msg = "No video capture device was detected!";
			break;
		}
		enum_moniker->Reset();

		IMoniker *moniker = nullptr;
		IBaseFilter *moniker_bind_object = nullptr;
		bool found_device = false;
		ULONG fetched_moniker;

		int index = 0;
		while (hr = enum_moniker->Next(1, &moniker, &fetched_moniker), hr == S_OK, index <= device_id)
		{
			IPropertyBag *pBag;
			hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
			if (SUCCEEDED(hr))
			{
				VARIANT var;
				var.vt = VT_BSTR;
				hr = pBag->Read(L"FriendlyName", &var, NULL);
				if (hr == NOERROR)
				{
					if (index == device_id)
					{
						found_device = true;
						IBaseFilter *ptemp = nullptr;
						moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)device_filter);
					}
					SysFreeString(var.bstrVal);
				}
				pBag->Release();
			}
			moniker->Release();
			index++;
		}

		bReturn = true;
	} while (false);
	return bReturn;
}

int CameraDS::CameraCount()
{
	int count = 0;
	do 
	{
		HRESULT hr = S_FALSE;

		if (!is_init_)
		{
			hr = CoInitialize(NULL);
			if (FAILED(hr))
				break;
		}

		// enumerate all video capture devices
		CComPtr<ICreateDevEnum> pCreateDevEnum = nullptr;
		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);
		if (FAILED(hr))
			break;

		CComPtr<IEnumMoniker> pEm;
		hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
		if (hr != NOERROR)
			break;

		pEm->Reset();
		ULONG cFetched;
		IMoniker *pM = nullptr;
		while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
		{
			count++;
		}

		pCreateDevEnum = nullptr;
		pEm = nullptr;
	} while (false);
	return count;
}

std::string CameraDS::GetCameraName(const int device_id)
{
	int name_buffer_size = 1024;
	HRESULT hr = S_FALSE;

	if (!is_init_)
	{
		hr = CoInitialize(NULL);
		if (FAILED(hr))
			return "";
	}

	// enumerate all video capture devices
	CComPtr<ICreateDevEnum> pCreateDevEnum;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (FAILED(hr))
		return"";

	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
	if (hr != NOERROR) return 0;

	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM = nullptr;
	char *camera_name = nullptr;
	int count = 0;
	while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
	{
		if (count == device_id)
		{
			IPropertyBag *pBag = 0;
			hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
			if (SUCCEEDED(hr))
			{
				VARIANT var;
				var.vt = VT_BSTR;
				hr = pBag->Read(L"FriendlyName", &var, NULL); //还有其他属性,像描述信息等等...
				if (hr == NOERROR)
				{
					//获取设备名称
					WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, camera_name, name_buffer_size, "", NULL);
					
					SysFreeString(var.bstrVal);
				}
				pBag->Release();
			}
			pM->Release();

			break;
		}
		count++;
	}

	pCreateDevEnum = NULL;
	pEm = NULL;

	return camera_name;
}

bool CameraDS::GetCameraName(int nCamID, char * sName, int nBufferSize)
{
	bool bReturn = false;
	do 
	{
		int count = 0;
		//CoInitialize(NULL);

		// enumerate all video capture devices
		CComPtr<ICreateDevEnum> pCreateDevEnum;
		HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);

		CComPtr<IEnumMoniker> pEm;
		hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
		if (hr != NOERROR) break;

		pEm->Reset();
		ULONG cFetched;
		IMoniker *pM;
		while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
		{
			if (count == nCamID)
			{
				IPropertyBag *pBag = 0;
				hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
				if (SUCCEEDED(hr))
				{
					VARIANT var;
					var.vt = VT_BSTR;
					hr = pBag->Read(L"FriendlyName", &var, NULL); //还有其他属性,像描述信息等等...
					if (hr == NOERROR)
					{
						//获取设备名称
						WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, sName, nBufferSize, "", NULL);

						SysFreeString(var.bstrVal);
					}
					pBag->Release();
				}
				pM->Release();

				break;
			}
			count++;
		}
		pCreateDevEnum = NULL;
		pEm = NULL;

		bReturn = true;
	} while (false);
	return bReturn;
}
