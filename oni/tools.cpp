#include <stdio.h>
#include <string>
#include "OpenNI2\Include\OpenNI.h"
#include <QtDebug>
#include <QCoreApplication>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <QSplashScreen>
#include <QMetaType>

using namespace openni;
using namespace std;

#include "tools.h"

struct ImagePair {
	QImage dframe;
	QImage cframe;
	int index;
};


void Log(Status status, const char* text)
{
	if (status == STATUS_OK)
	{
		qDebug() << text;
	}
	else {
		qDebug() << OpenNI::getExtendedError();
	}
}

int tools::counts(const char* fileName)
{
	Status status = STATUS_OK;
	status = OpenNI::initialize();
	Device device;
	status = device.open(fileName);
	VideoStream color;
	status = color.create(device, SENSOR_COLOR);

	color.start();

	int numOfFrames = -1;
	Status rc = color.getProperty<int>(STREAM_PROPERTY_NUMBER_OF_FRAMES, &numOfFrames);
	if (rc != STATUS_OK)
	{
		numOfFrames = 0;
	}

	color.stop();
	color.destroy();
	device.close();
	OpenNI::shutdown();

	return numOfFrames;

}

tools::Pairs tools::frames(const char* fileName)
{
	Status status = STATUS_OK;
	status = OpenNI::initialize();
	Device device;
	status = device.open(fileName);
	string info;

	info.append(device.getDeviceInfo().getName());
	info.append(" файл: ");
	info.append(fileName);

	Log(status, info.c_str());
	VideoStream depth, color;

	status = depth.create(device, SENSOR_DEPTH);
	Log(status, "SENSOR_DEPTH");
	status = color.create(device, SENSOR_COLOR);
	Log(status, "SENSOR_COLOR");

	color.start();
	depth.start();

	QPixmap pixmap("loading.jpg");
	QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);

	splash.showMessage("");
	splash.show();

	cv::Mat frame;
	openni::VideoFrameRef colorFrame;

	cv::Mat dframe;
	openni::VideoFrameRef depthFrame;
	int countframe = -1;
	int dcountframe = -1;
	int changedIndex;

	VideoStream** stream = new VideoStream*[2];
	stream[0] = &depth;
	stream[1] = &color;

	int numOfFrames = -2;
	Status rc = color.getProperty<int>(STREAM_PROPERTY_NUMBER_OF_FRAMES, &numOfFrames);
	if (rc != STATUS_OK)
	{
		numOfFrames = 0;
	}

	tools::Pairs P;
	P.ip = new ImagePair[numOfFrames + 1]();
	P.count = numOfFrames;

	while (numOfFrames > countframe && numOfFrames > dcountframe)
	{
		OpenNI::waitForAnyStream(stream, 2, &changedIndex);
		QCoreApplication::processEvents();

		switch (changedIndex)
		{
		case 0:
		{
			countframe++;
			depth.readFrame(&depthFrame);
			dframe.create(depthFrame.getHeight(), depthFrame.getWidth(), CV_16UC1);
			dframe.data = (uchar*)depthFrame.getData();
			//cv::imshow( "Depth", dframe );
			QImage dimage = tools::mat2Qimgd(dframe);
			P.ip[countframe].index = countframe;
			//pairs[countframe].dframe = dframe;
			P.ip[countframe].dframe = dimage;

			break;
		}
		case 1:
		{
			dcountframe++;
			color.readFrame(&colorFrame);
			const openni::RGB888Pixel* imageBuffer = (const openni::RGB888Pixel*)colorFrame.getData();
			frame.create(colorFrame.getHeight(), colorFrame.getWidth(), CV_8UC3);
			memcpy(frame.data, imageBuffer, 3 * colorFrame.getHeight()*colorFrame.getWidth() * sizeof(uint8_t));
			cv::cvtColor(frame, frame, CV_BGR2RGB);
			//cv::imshow( "RGB", frame );
			//QImage image = QImage(frame.data, frame.cols, frame.rows, QImage::Format_RGB888).rgbSwapped();
			QImage image = tools::mat2Qimgc(frame);
			P.ip[dcountframe].index = dcountframe;
			//pairs[countframe].cframe = frame;
			P.ip[dcountframe].cframe = image;

			break;
		}
		}
		std::string info = std::to_string(countframe);
		info += " ";
		info += std::to_string(numOfFrames);
		splash.showMessage(info.c_str(), Qt::AlignBottom | Qt::AlignCenter);
		//Loging(info.c_str());
	}

	splash.finish(splash.window());
	depth.stop();
	color.stop();
	depth.destroy();
	color.destroy();
	device.close();
	OpenNI::shutdown();
	return P;
}

QImage tools::mat2Qimgc(const cv::Mat &src) {
	cv::Mat temp;
	cvtColor(src, temp, CV_BGR2RGB);
	QImage dest((uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
	QImage dest2(dest);
	dest2.detach();
	return dest2;
}

QImage tools::mat2Qimgd(const cv::Mat &source) {
	quint16* pSource = (quint16*)source.data;
	int pixelCounts = source.cols * source.rows;
	QImage dest(source.cols, source.rows, QImage::Format_RGB32);
	char* pDest = (char*)dest.bits();
	for (int i = 0; i < pixelCounts; i++)
	{
		quint8 value = (quint8)((*(pSource)) >> 8);
		*(pDest++) = value;  // B
		*(pDest++) = value;  // G
		*(pDest++) = value;  // R
		*(pDest++) = 0;      // Alpha
		pSource++;
	}
	return dest;
}

void tools::Loging(const char* text)
{
	qDebug() << text;
}
