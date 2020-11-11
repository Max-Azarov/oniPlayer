#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QFileDialog>
#include <QFile>
#include <QBasicTimer>
#include <QLabel>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <string>


#include "imagewindow.h"
#include "tools.h"

tools::Pairs pairs;
int tick = 0;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(this->ui->action, SIGNAL(triggered()), this, SLOT(Open()));
	window1 = new ImageWindow(this);
	imageLabel1 = new QLabel(window1);
	imageLabel1->setBackgroundRole(QPalette::Base);
	imageLabel1->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel1->setScaledContents(true);
	window2 = new ImageWindow(this);
	imageLabel2 = new QLabel(window2);
	imageLabel2->setBackgroundRole(QPalette::Base);
	imageLabel2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel2->setScaledContents(true);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));

	
}

void MainWindow::slotTimerAlarm()
{
	//play();
	tickPosition();
}

MainWindow::~MainWindow()
{
	delete ui;
	openni::OpenNI::shutdown();
}

void MainWindow::on_btnL_clicked()
{
	//left
	stepFrame(-1);
}

void MainWindow::on_btnPlay_clicked()
{
	if (m_isPlay) {
		timer->stop();
		ui->btnPlay->setText("Play");
		m_isPlay = false;
		qDebug() << "stop";
	}
	else {
		if (m_tick >= m_countOfFrames) { m_tick = 0; }
		timer->start(200);
		ui->btnPlay->setText("Stop");
		m_isPlay = true;
		qDebug() << "play";
	}
	
}

void MainWindow::stepFrame(int p) {
	if (pairs.count < 1) { return; }
	if (p > 0) {
		tick += 1;
		play();
	}
	else {
		tick -= 2;
		play();
	}
}



void MainWindow::tickPosition()
{
	ui->slider->setValue(m_tick);
}

void MainWindow::on_btnR_clicked()
{
	//right
	stepFrame(1);
}

void MainWindow::on_slider_sliderReleased()
{
	//slider
	int position = ui->slider->value();
	tick = position;
	play();
}

void MainWindow::on_slider_sliderMoved(int position)
{
	//slider
	tick = position;
	play();
}

void MainWindow::on_slider_sliderPressed()
{
	//slider
	int position = ui->slider->value();
	tick = position;
	play();
}

void MainWindow::Open()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString("Открыть файл"), QString("."), QString("ONI (*.oni)"));
	restart();
	if (fileName.size() > 0) {
		QFile f(fileName);
		if (f.open(QIODevice::ReadOnly))
		{
			openni::Status status = openni::STATUS_OK;
			status = m_device.open(f.fileName().toStdString().c_str());
			m_pColorStream = &openni::VideoStream();
			m_pDepthStream = &openni::VideoStream();
			m_vsArr[0] = m_pDepthStream;
			m_vsArr[1] = m_pColorStream;
			
			status = m_pDepthStream->create(m_device, openni::SENSOR_DEPTH);
			status = m_pColorStream->create(m_device, openni::SENSOR_COLOR);
			m_pbc = m_device.getPlaybackControl();

			f.close();
			show();
			ui->slider->setValue(0);
			
			int sw = ui->slider->width();
			m_countOfFrames = m_pbc->getNumberOfFrames(*m_pColorStream);
			if (m_countOfFrames > sw) {
				int step = int(m_countOfFrames / sw);
				ui->slider->setPageStep(step);
			}
			ui->slider->setMaximum(m_countOfFrames);

			window1->setWindowTitle("Depth");
			window1->setGeometry(30, 180, 640, 480);
			window1->show();

			window2->setWindowTitle("Color");
			window2->setGeometry(700, 180, 640, 480);
			window2->show();
			

			m_isExit = false;
			loop();
			if (m_device.isValid()) m_device.close();
			openni::OpenNI::shutdown();
			//play();
			//play();
			/*
			Status getProperty(int propertyId, T* value) const
			{
				int size = sizeof(T);
				return getProperty(propertyId, value, &size);
			}
			*/
		}
	}
}

void MainWindow::loop() {
	while (!m_isExit) {
		if (m_isPlay) {
			if (!m_isStartStream) {
				m_pDepthStream->start();
				m_pColorStream->start();
				m_isStartStream = true;
			}
			play();
		}
		else {
			if (m_isStartStream) {
				m_pDepthStream->stop();
				m_pColorStream->stop();
				m_isStartStream = false;
			}
		}
		QCoreApplication::processEvents();
	}
}

void MainWindow::play() {
	openni::Status status = openni::STATUS_OK;
	if (m_countOfFrames > 0) {
		std::string s = std::to_string(m_tick);
		s += "/";
		s += std::to_string(m_countOfFrames);
		ui->countframe->setText(s.c_str());

		if (m_tick >= m_countOfFrames) {
			timer->stop();
			ui->btnPlay->setText("Play");

			return;
		}

		//play
		openni::SensorType sensorType;
		QImage image;
		getImageFrame(sensorType, image);

		if (sensorType == openni::SensorType::SENSOR_COLOR)
		{
			imageLabel2->setGeometry(0, 0, 640, 480);
			imageLabel2->setPixmap(QPixmap::fromImage(image));
		}
		if (sensorType == openni::SensorType::SENSOR_DEPTH)
		{
			imageLabel1->setGeometry(0, 0, 640, 480);
			imageLabel1->setPixmap(QPixmap::fromImage(image));
		}

	}
	qDebug() << m_tick;
}

void MainWindow::getImageFrame(openni::SensorType& sensorType, QImage& image)
{
	openni::Status status = openni::STATUS_OK;
	cv::Mat cvFrame;
	openni::VideoFrameRef openFrame;
	
	int readyIndex = 0;
	openni::OpenNI::waitForAnyStream(m_vsArr, 2, &readyIndex);
	
	switch (readyIndex)
	{
	case 0:
	{
		sensorType = openni::SENSOR_DEPTH;
		//status = m_pbc->seek(*m_pDepthStream, frameIndex);
		status = m_pDepthStream->readFrame(&openFrame);
		cvFrame.create(openFrame.getHeight(), openFrame.getWidth(), CV_16UC1);
		cvFrame.data = (uchar*)openFrame.getData();
		image = mat2Qimgd(cvFrame);
		break;
	}
	case 1:
	{
		sensorType = openni::SENSOR_COLOR;
		//status = m_pbc->seek(*m_pColorStream, frameIndex);
		status = m_pColorStream->readFrame(&openFrame);
		const openni::RGB888Pixel* imageBuffer = (const openni::RGB888Pixel*)openFrame.getData();
		cvFrame.create(openFrame.getHeight(), openFrame.getWidth(), CV_8UC3);
		memcpy(cvFrame.data, imageBuffer, 3 * openFrame.getHeight()*openFrame.getWidth() * sizeof(uint8_t));
		cv::cvtColor(cvFrame, cvFrame, CV_BGR2RGB);
		image = mat2Qimgc(cvFrame);
		if (openFrame.isValid()) m_tick = openFrame.getFrameIndex();
		break;
	}
	}
	
	
	qDebug() << m_tick;
}

QImage MainWindow::mat2Qimgc(const cv::Mat &src) {
	cv::Mat temp;
	cvtColor(src, temp, CV_BGR2RGB);
	QImage dest((uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
	QImage dest2(dest);
	dest2.detach();
	return dest2;
}

QImage MainWindow::mat2Qimgd(const cv::Mat &source) {
	quint16* pSource = (quint16*)source.data;
	int pixelCounts = source.cols * source.rows;
	QImage dest(source.cols, source.rows, QImage::Format_RGB32);
	char* pDest = (char*)dest.bits();
	for (int i = 0; i < pixelCounts; i++, pSource++)
	{
		quint16 value = (*pSource) >> 4;
		if (value > 255) value = 255;
		*(pDest++) = 0;  // B
		*(pDest++) = (char)value;  // G
		*(pDest++) = (char)value;  // R
		*(pDest++) = 0;      // Alpha
	}
	return dest;
}

void MainWindow::restart() {
	if (m_device.isValid()) m_device.close();
	openni::OpenNI::shutdown();
	window1->hide();
	window2->hide();
	hide();
	m_pColorStream = nullptr;
	m_pDepthStream = nullptr;

	m_tick = 0;
	m_isPlay = true;
	m_isStartStream = false;
	m_isExit = true;
	openni::OpenNI::initialize();
}

//openni::Status status = openni::STATUS_OK;
//status = openni::OpenNI::initialize();
//openni::Device device;
//status = device.open(fileName);
//std::string info;

//info.append(m_device.getDeviceInfo().getName());
//info.append(" файл: ");
//info.append(m_device.fileName);

//openni::VideoStream videoStream;//depth, color;

//status = videoStream.create(m_device, sensorType);
//qDebug() << status;

//Log(status, "SENSOR_DEPTH");
//status = color.create(m_device, openni::SENSOR_COLOR);
//Log(status, "SENSOR_COLOR");

//color.start();
//depth.start();

/*
	while (numOfFrames > countframe && numOfFrames > dcountframe)
	{
		openni::OpenNI::waitForAnyStream(stream, 2, &changedIndex);
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
		//Loging(info.c_str());
	}
	*/
	/*
	depth.stop();
	color.stop();
	depth.destroy();
	color.destroy();
	device.close();
	openni::OpenNI::shutdown();
	return P;
	*/

	//if (sensorType == openni::SENSOR_COLOR) image = mat2Qimgc(cvFrame);

	/*
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
	*/
	//cv::Mat dframe;
		//openni::VideoFrameRef depthFrame;
		//int countframe = -1;
		//int dcountframe = -1;
		//int changedIndex;

		//openni::VideoStream** stream = new openni::VideoStream*[2];
		//stream[0] = &depth;
		//stream[1] = &color;

		//int numOfFrames = -2;
		//openni::Status rc = color.getProperty<int>(openni::STREAM_PROPERTY_NUMBER_OF_FRAMES, &numOfFrames);
		//if (rc != openni::STATUS_OK)
		//{
		//	numOfFrames = 0;
		//}

		//tools::Pairs P;
		//P.ip = new ImagePair[numOfFrames + 1]();
		//P.count = numOfFrames;
		//Status seek(const VideoStream& stream, int frameIndex);
		//m_pbc = m_device.getPlaybackControl();
