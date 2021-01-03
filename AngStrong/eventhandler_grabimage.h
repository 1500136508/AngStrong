#pragma once
#include <QObject>
#include <qedit.h>
#include <opencv.hpp>

#define EFE_FORMAT

class EventHandlerGrabImage:public QObject,public ISampleGrabberCB
{
	Q_OBJECT
public:
	EventHandlerGrabImage();
	~EventHandlerGrabImage();

	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

	HRESULT STDMETHODCALLTYPE SampleCB(double Time, IMediaSample *pSample);
	HRESULT STDMETHODCALLTYPE BufferCB(double Time, BYTE *pBuffer, long BufferLen);
	
	void readpdData();
	// 设置内外参
	int setParam(float _fx, float _fy, float _cx, float _cy);
	void GetRGBImage(BYTE *rgb_image_data);
	void GetDepthImage(BYTE *depth_image_data);
	void GetIRImage(BYTE *ir_image_data);

	bool IsNewImageData(uchar *image_data);
	void InitPDData(uchar *image_data);
	bool IsInitPDData()const { return getParam; }
	void DispImage();
	void SendDepthImageData(float *depth_image_data);
private slots:
	void ReceiveCameraFormat(int width, int height);
	void ReceiveMouseInfo(int x, int y);
signals:
	void send_image(cv::Mat image_rgb,cv::Mat image_depth,cv::Mat image_ir);
	void SendLocationDepth(int x, int y, float depth);
private:
	int width_;
	int height_;

	float fx = 0;
	float fy = 0;
	float cx = 0;
	float cy = 0;
private:
	void WaitGetImageFinished(long timeout = 5000);

	std::vector<cv::Mat> container;

	clock_t time1, time2, startTime, stopTime;
	int datalen = 1280;

	const int frameHeight = 400;
	const int frameWidth = 640;
#ifdef EFE_FORMAT
	const int frameHeightRGB = 480;
	const int frameWidthRGB = 848;
#else
	const int frameHeightRGB = 400;
	const int frameWidthRGB = 640;
#endif

	cv::Mat edge = cv::Mat::zeros(cv::Size(frameHeight, frameWidth), CV_8UC1);
	cv::Mat edge_clear = cv::Mat::zeros(cv::Size(frameWidth, frameHeight), CV_8UC1);
	cv::Mat edge_th = cv::Mat::zeros(cv::Size(frameWidth, frameHeight), CV_8UC1);

	cv::Mat irFrame;
	cv::Mat irFrameAlign;
	cv::Mat temp;
	cv::Mat depthFrame;
	cv::Mat RGBFrame;
	cv::Mat combineFrame;
	cv::Mat irFrame16bit;

	unsigned char* irData;
	unsigned char* irDataGamma;
	float* depthData;
	float* predepthData;
	float* depthDataRGB;
	float* tmpdepth;
	unsigned char* rgbData;

	//当前鼠标位置
	int m_MouseX;
	int m_MouseY;
	//AvgArea
	bool calcArea = false;
	bool getFirstArea = false;
	bool getSecondArea = false;
	int isWarp = false;

	int realX1 = -1;
	int realY1 = -1;
	int realX2 = -1;
	int realY2 = -1;

	int realX1s = -1;
	int realY1s = -1;
	int realX2s = -1;
	int realY2s = -1;


	unsigned char* _buf;
	unsigned char* _buf2;


	long long rgbT;
	long long irT;
	long long depthT;
	long long lastRgbT;

	bool program_quite = false;
	bool getParam = false;
	volatile bool is_running_ = false;
	bool is_get_image_finished = true;
};

