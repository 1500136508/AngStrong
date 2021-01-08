#include "eventhandler_grabimage.h"
#include <imgproc.hpp>
#include <core.hpp>
#include <dsense_interface.h>
#include "logmanager.h"
#include "utilimage.hpp"

cv::Mat K_rgb = cv::Mat::zeros(3, 3, CV_32FC1);
cv::Mat R = cv::Mat::zeros(3, 3, CV_32FC1);
cv::Mat K_ir = cv::Mat::zeros(3, 3, CV_32FC1);
cv::Mat T = cv::Mat::zeros(3, 3, CV_32FC1);
cv::Mat tmpM = cv::Mat::zeros(3, 3, CV_32FC1);

unsigned char hash_grayscale[] = { 0, 15, 22, 27, 31, 35, 39, 42, 45, 47, 50,
								52, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73,
								74, 76, 78, 79, 81, 82, 84, 85, 87, 88, 90,
								91, 93, 94, 95, 97, 98, 99, 100, 102, 103,
								104, 105, 107, 108, 109, 110, 111, 112, 114,
								115, 116, 117, 118, 119, 120, 121, 122, 123,
								124, 125, 126, 127, 128, 129, 130, 131, 132,
								133, 134, 135, 136, 137, 138, 139, 140, 141,
								141, 142, 143, 144, 145, 146, 147, 148, 148,
								149, 150, 151, 152, 153, 153, 154, 155, 156,
								157, 158, 158, 159, 160, 161, 162, 162, 163,
								164, 165, 165, 166, 167, 168, 168, 169, 170,
								171, 171, 172, 173, 174, 174, 175, 176, 177,
								177, 178, 179, 179, 180, 181, 182, 182, 183,
								184, 184, 185, 186, 186, 187, 188, 188, 189,
								190, 190, 191, 192, 192, 193, 194, 194, 195,
								196, 196, 197, 198, 198, 199, 200, 200, 201,
								201, 202, 203, 203, 204, 205, 205, 206, 206,
								207, 208, 208, 209, 210, 210, 211, 211, 212,
								213, 213, 214, 214, 215, 216, 216, 217, 217,
								218, 218, 219, 220, 220, 221, 221, 222, 222,
								223, 224, 224, 225, 225, 226, 226, 227, 228,
								228, 229, 229, 230, 230, 231, 231, 232, 233,
								233, 234, 234, 235, 235, 236, 236, 237, 237,
								238, 238, 239, 240, 240, 241, 241, 242, 242,
								243, 243, 244, 244, 245, 245, 246, 246, 247,
								247, 248, 248, 249, 249, 250, 250, 251, 251,
								252, 252, 253, 253, 254, 255 };
ir_rgb_State rgb_param{ 0 };
unsigned char PD[1024];
float realpd[30];

EventHandlerGrabImage::EventHandlerGrabImage()
{
	// 初始化各种buffer
	depthFrame = cv::Mat(cv::Size(frameHeight, frameWidth), CV_8UC3);
	irFrame = cv::Mat(cv::Size(frameHeight, frameWidth), CV_8UC3);
	RGBFrame = cv::Mat(cv::Size(frameHeight, frameWidth), CV_8UC3);
	for (int i = 0; i < 3; i++) container.push_back(cv::Mat());
	irFrame.copyTo(container[0]);
	depthFrame.copyTo(container[1]);
	RGBFrame.copyTo(container[2]);

	irData = new unsigned char[frameWidth*frameHeight];
	irDataGamma = new unsigned char[frameWidth*frameHeight];
	depthData = new float[frameWidthRGB*frameHeightRGB];
	for (int i = 0; i < frameHeightRGB * frameWidthRGB; i++) depthData[i] = 500.0f;
	predepthData = new float[frameWidthRGB*frameHeightRGB];
	depthDataRGB = new float[frameWidthRGB*frameHeightRGB];
	tmpdepth = new float[frameWidthRGB*frameHeightRGB * 4];
	_buf = new unsigned char[frameHeightRGB * frameWidthRGB * (3 * sizeof(int) + sizeof(uchar))];
	_buf2 = new unsigned char[frameHeightRGB * frameWidthRGB * (4 * sizeof(int) + sizeof(uchar))];
}

EventHandlerGrabImage::~EventHandlerGrabImage()
{
	WaitGetImageFinished();
	ReleasePointer();
}

ULONG __stdcall EventHandlerGrabImage::AddRef()
{
	return 0;
}

ULONG __stdcall EventHandlerGrabImage::Release()
{
	return 0;
}

HRESULT __stdcall EventHandlerGrabImage::QueryInterface(REFIID riid, void ** ppvObject)
{
	return S_OK;
}

HRESULT __stdcall EventHandlerGrabImage::SampleCB(double Time, IMediaSample * pSample)
{
	return S_OK;
}

HRESULT __stdcall EventHandlerGrabImage::BufferCB(double Time, BYTE * pBuffer, long BufferLen)
{
	if (!pBuffer)
	{
		return S_FALSE;
	}

	clock_t start_time = clock();
	InitPDData(pBuffer);
	if (!IsNewImageData(pBuffer))
	{
		return S_FALSE;
	}
	is_get_image_finished = false;
	GetRGBImage(pBuffer);
	GetDepthImage(pBuffer);
	GetIRImage(pBuffer);
	DispImage();
	SendDepthImageData(depthDataRGB);
	is_get_image_finished = true;
	clock_t test_time = clock() - start_time;
	if (test_time < 34)
	{
		//Sleep(34 - test_time);
	}
	return S_OK;
}

void EventHandlerGrabImage::readpdData()
{
	unsigned char *ptr = PD + 16;
	int datalen = 30;
	int i = 0;
	while (i < datalen) {
		memcpy(&realpd[i], ptr, sizeof(float)); ptr += sizeof(float);
#ifdef DEBUG
		qDebug() << "param " << i << " " << realpd[i];
#endif
		i++;
	}
#ifdef EFE_FORMAT
	float ratio = 1080.f / 480.0f;
#else
	float ratio = 2;
#endif
	rgb_param.fxir = realpd[0] / 2;
	rgb_param.fyir = realpd[1] / 2;
	rgb_param.cxir = realpd[2] / 2;
	rgb_param.cyir = realpd[3] / 2;

	rgb_param.fxrgb = realpd[9] / ratio;
	rgb_param.fyrgb = realpd[10] / ratio;
	rgb_param.cxrgb = realpd[11] / ratio;
	rgb_param.cyrgb = realpd[12] / ratio;

	K_rgb.at<float>(0, 0) = rgb_param.fxrgb;
	K_rgb.at<float>(0, 2) = rgb_param.cxrgb;
	K_rgb.at<float>(1, 1) = rgb_param.fyrgb;
	K_rgb.at<float>(1, 2) = rgb_param.cyrgb;
	K_rgb.at<float>(2, 2) = 1;

	K_ir.at<float>(0, 0) = rgb_param.fxir;
	K_ir.at<float>(0, 2) = rgb_param.cxir;
	K_ir.at<float>(1, 1) = rgb_param.fyir;
	K_ir.at<float>(1, 2) = rgb_param.cyir;
	K_ir.at<float>(2, 2) = 1;

	rgb_param.R00 = realpd[18];
	rgb_param.R01 = realpd[19];
	rgb_param.R02 = realpd[20];

	rgb_param.R10 = realpd[21];
	rgb_param.R11 = realpd[22];
	rgb_param.R12 = realpd[23];

	rgb_param.R20 = realpd[24];
	rgb_param.R21 = realpd[25];
	rgb_param.R22 = realpd[26];

	R.at<float>(0, 0) = rgb_param.R00;
	R.at<float>(0, 1) = rgb_param.R01;
	R.at<float>(0, 2) = rgb_param.R02;
	R.at<float>(1, 0) = rgb_param.R10;
	R.at<float>(1, 1) = rgb_param.R11;
	R.at<float>(1, 2) = rgb_param.R12;
	R.at<float>(2, 0) = rgb_param.R20;
	R.at<float>(2, 1) = rgb_param.R21;
	R.at<float>(2, 2) = rgb_param.R22;

	rgb_param.T1 = realpd[27];
	rgb_param.T2 = realpd[28];
	rgb_param.T3 = realpd[29];

	T.at<float>(0, 2) = rgb_param.T1;
	T.at<float>(1, 2) = rgb_param.T2;
	T.at<float>(2, 2) = rgb_param.T3;
	tmpM = K_rgb * R*K_ir.inv() + K_rgb * T*K_ir.inv() / 600;
	setParam(rgb_param.fxrgb, rgb_param.fyrgb, rgb_param.cxrgb, rgb_param.cyrgb);
	std::ofstream logFile; logFile.open("moudule_param.yaml");
	logFile << "fx: " << rgb_param.fxrgb << std::endl;
	logFile << "fy: " << rgb_param.fyrgb << std::endl;
	logFile << "cx: " << rgb_param.cxrgb << std::endl;
	logFile << "cy: " << rgb_param.cyrgb << std::endl;
	logFile << "tx: " << 40 << std::endl;
	logFile << "mind: " << 300 << std::endl;
	logFile << "maxd: " << 2000 << std::endl;
	logFile << "f0: " << rgb_param.fxrgb << std::endl;
	logFile.close();
}

int EventHandlerGrabImage::setParam(float _fx, float _fy, float _cx, float _cy)
{
	fx = _fx;
	fy = _fy;
	cx = _cx;
	cy = _cy;
	return 0;
}

int YUY2_to_RGB24(void* const data, const int width, const int height)
{
	if (data == NULL)
		return -1;

	//int width = width; // 640
	//int height = height; // 400
	unsigned char* const data_src = (unsigned char *)data;
	unsigned char* const data_dst = (unsigned char *)data;

	/*
	 * transformat data stream from YUY2 to RGB24
	 */
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t index = (y * width + x) * 3;

			int Y = *(data_src + index + 0);
			int U = *(data_src + index + 1);
			int V = *(data_src + index + 2);

			int C = Y - 16;
			int D = U - 128;
			int E = V - 128;

			int R = (298 * C + 409 * E + 128) >> 8;
			int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
			int B = (298 * C + 516 * D + 128) >> 8;

			/*int R = Y + 1.4075 * (V - 128);
			int G = Y + 0.3455 * (U - 128) - 0.7169 * (V - 128);
			int B = Y + 1.7790 * (U - 128);*/

			*(data_dst + index + 0) = R > 255 ? 255 : (R < 0 ? 0 : R);
			*(data_dst + index + 1) = G > 255 ? 255 : (G < 0 ? 0 : G);
			*(data_dst + index + 2) = B > 255 ? 255 : (B < 0 ? 0 : B);
		}
	}

	return 0;
}

void EventHandlerGrabImage::GetRGBImage(BYTE * rgb_image_data)
{
	try
	{
		uchar* ptr = rgb_image_data + frameHeight * frameWidth * 2;
		if (!ptr)
		{
			LogManager::Write("ptr is nll!");
			return;
		}
		cv::Mat rgbyuv(cv::Size(frameWidthRGB, frameHeightRGB), CV_8U, ptr);
		if (rgbyuv.empty())
		{
			LogManager::Write("rgbyuv is empty!");
			return;
		}
		RGBFrame = cv::imdecode(rgbyuv, CV_LOAD_IMAGE_COLOR);
		if (RGBFrame.empty())
		{
			LogManager::Write("RGBFrame is empty!");
			return;
		}
		cv::transpose(RGBFrame, RGBFrame);
		cv::flip(RGBFrame, RGBFrame, 0);
		cv::flip(RGBFrame, RGBFrame, -1);
		RGBFrame.copyTo(container[2]);
	}
	catch (cv::Exception &e)
	{
		std::string err_msg = e.what();
		LogManager::Write("Failed to Grab RGB Image!");
		return;
	}
}

void EventHandlerGrabImage::GetDepthImage(BYTE * depth_image_data)
{
	try
	{
		int flag = depth_image_data[frameHeight*frameWidth * 2 - 1];
		if (flag == 2 || flag == 6)
		{
			float* dst = depthData;
			unsigned short* tmp = (unsigned short*)depth_image_data;
			for (int y = 0; y < frameHeight; y++)
			{
				for (int x = 0; x < frameWidth; x++)
				{
					dst[x*frameHeight + (frameHeight - 1 - y)] = (float)(tmp[x] >> 4);
				}
				tmp += frameWidth;
			}
			denoise(depthData, 0, 200, 80, _buf, 2000, frameHeight, frameWidth);
			filling(edge.data, depthData, 0, 300, 10, _buf2, frameHeight, frameWidth);

			depth2RGB(depthData, depthDataRGB, tmpdepth, frameHeightRGB, frameWidthRGB, frameHeight, frameWidth, rgb_param);
			depthFrame = cv::Mat(cv::Size(frameHeightRGB, frameWidthRGB), CV_32FC1, depthDataRGB);
			cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
			dilate(depthFrame, depthFrame, element);
			memcpy(depthDataRGB, depthFrame.data, frameWidthRGB*frameHeightRGB * sizeof(float));
			depthFrame = dealDepthMapColor(depthDataRGB, frameHeightRGB, frameWidthRGB);
			depthFrame.copyTo(container[1]);
		}
	}
	catch (cv::Exception &e)
	{
		std::string err_msg = e.what();
		LogManager::Write("Failed to Grab Depth Image!");
		return;
	}
}

void EventHandlerGrabImage::GetIRImage(BYTE * ir_image_data)
{
	try
	{
		int flag = ir_image_data[frameHeight*frameWidth * 2 - 1];
		if (flag == 1)
		{
			memcpy(&irT, ir_image_data, sizeof(long long));
			unsigned char* dst = irData;
			unsigned char* dst2 = irDataGamma;
			unsigned short* tmp = (unsigned short*)ir_image_data;
			for (int y = 0; y < frameHeight; y++)
			{
				for (int x = 0; x < frameWidth; x++)
				{
					dst[x*frameHeight + (frameHeight - 1 - y)] = (unsigned char)(tmp[x] >> 8);
					dst2[x*frameHeight + (frameHeight - 1 - y)] = hash_grayscale[(unsigned char)(tmp[x] >> 8)];
				}
				tmp += frameWidth;
			}
			irFrame = cv::Mat(cv::Size(frameHeight, frameWidth), CV_8UC1, irData);
			irFrame.copyTo(irFrame16bit);
			cv::cvtColor(irFrame, irFrame, cv::COLOR_GRAY2BGR);
			Canny(irFrame, edge_clear, 35, 70);
			GaussianBlur(edge_clear, edge_th, cv::Size(3, 3), 0.5);
			threshold(edge_th, edge, 10, 255, cv::THRESH_BINARY);
			irFrame = cv::Mat(cv::Size(frameHeight, frameWidth), CV_8UC1, irDataGamma);
			cv::cvtColor(irFrame, irFrame, cv::COLOR_GRAY2BGR);
			cv::warpPerspective(irFrame, irFrameAlign, tmpM, cv::Size(480, 848));
			irFrameAlign.copyTo(container[0]);
		}
	}
	catch (cv::Exception &e)
	{
		std::string err_msg = e.what();
		LogManager::Write("Failed to Grab IR Image!");
		return;
	}
}

bool EventHandlerGrabImage::IsNewImageData(uchar * image_data)
{
	bool bReturn = false;
	do
	{
		memcpy(&rgbT, image_data + 640 * 480 * 2 - 8, sizeof(long long));
		if (rgbT == lastRgbT)
		{
			break;
		}
		else
		{
			lastRgbT = rgbT;
		}
		bReturn = true;
	} while (false);
	return bReturn;
}

void EventHandlerGrabImage::InitPDData(uchar * image_data)
{
	if (!getParam)
	{
		memcpy(PD, image_data + 640 * 480 * 2 - 1032, 1024);
		readpdData();
		getParam = true;
	}
}

void EventHandlerGrabImage::DispImage()
{
	if (getParam)
	{
		try
		{
			emit send_image(container[2], container[1], container[0]);
		}
		catch (cv::Exception &e)
		{
			std::string err_msg = e.what();
			return;
		}
	}
}

void EventHandlerGrabImage::SendDepthImageData(float * depth_image_data)
{
	if (m_MouseX >= 0 && m_MouseY >= 0)
	{
		int xhere = m_MouseX % frameHeightRGB;
		int yhere = m_MouseY % frameWidthRGB;
		if (m_MouseX >= frameHeightRGB)
		{
			emit SendLocationDepth(xhere, yhere, depth_image_data[yhere*frameHeightRGB + xhere]);
		}
		else
		{
			emit SendLocationDepth(-1, -1, 0);
		}
	}
}

void EventHandlerGrabImage::ReceiveMouseInfo(int x, int y)
{
	m_MouseX = x;
	m_MouseY = y;
}

void EventHandlerGrabImage::ReceiveCameraFormat(int width, int height)
{
	width_ = width;
	height_ = height;
}

void EventHandlerGrabImage::WaitGetImageFinished(long timeout)
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
		else if (is_get_image_finished)
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

void EventHandlerGrabImage::ReleasePointer()
{
	program_quite = true;
	if (irData)
	{
		delete[] irData;
		irData = nullptr;
	}
	if (depthData)
	{
		delete[] depthData;
		depthData = nullptr;
	}
	if (predepthData)
	{
		delete[] predepthData;
		predepthData = nullptr;
	}
	if (depthDataRGB)
	{
		delete[] depthDataRGB;
		depthDataRGB = nullptr;
	}
	if (tmpdepth)
	{
		delete[] tmpdepth;
		tmpdepth = nullptr;
	}
	if (_buf)
	{
		delete[] _buf;
		_buf = nullptr;
	}
	if (_buf2)
	{
		delete[] _buf2;
		_buf2 = nullptr;
	}
	RGBFrame.release();
	depthFrame.release();
	irFrame.release();
	for (size_t sz = 0; sz < 3;++sz)
	{
		container[sz].release();
	}
	container.clear();
}
