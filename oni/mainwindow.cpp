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
#include <QCloseEvent>
#include <QThread>
#include <QDesktopWidget>
#include <QLayout>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(this->ui->action, SIGNAL(triggered()), this, SLOT(Open()));
	
	window = new QMainWindow;
	QWidget* centralWidget = new QWidget(window);
	window->setCentralWidget(centralWidget);
	
	window->setWindowFlags(Qt::WindowMinMaxButtonsHint);
	
	colorChanel = new QLabel;
	depthChanel = new QLabel;
	QGridLayout* hbl = new QGridLayout;
	hbl->addWidget(depthChanel, 0, 0,  Qt::AlignRight);
	hbl->addWidget(colorChanel, 0, 1, Qt::AlignLeft);
	window->centralWidget()->setLayout(hbl);
	QPalette pall;
	pall.setColor(window->backgroundRole(), Qt::black);
	window->setPalette(pall);
	window->setAutoFillBackground(true);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
	this->ui->btnL->setEnabled(false);
	this->ui->btnR->setEnabled(false);
	this->ui->btnPlay->setEnabled(false);
	this->ui->slider->setEnabled(false);
}

void MainWindow::slotTimerAlarm()
{
	tickPosition();
}

MainWindow::~MainWindow()
{
	delete ui;
	openni::OpenNI::shutdown();
}

void MainWindow::on_btnPlay_clicked()
{
	if (m_isPlay) {
		ui->btnPlay->setText("Play");
		m_isPlay = false;
	}
	else {
		ui->btnPlay->setText("Stop");
		m_isPlay = true;
		if (m_tick >= m_countOfFrames) {
			m_tick = 0;
			m_pbc->seek(*m_pDepthStream, m_tick);
		}
	}
}

void MainWindow::tickPosition()
{
	ui->slider->setValue(m_tick);
	std::string s = std::to_string(m_tick);
	s += "/";
	s += std::to_string(m_countOfFrames);
	ui->countframe->setText(s.c_str());
}

void MainWindow::on_btnR_clicked()
{
	// right
	int position = m_tick + 1;
	//qDebug() << position << "position";
	position = (position > m_countOfFrames ? m_countOfFrames : position);
	m_pbc->seek(*m_pDepthStream, position);
	play();
}

void MainWindow::on_btnL_clicked()
{
	// left
	int position = m_tick - 1;
	//qDebug() << position << "position";
	position = (position < m_numFirstFrame ? m_numFirstFrame : position);
	m_pbc->seek(*m_pDepthStream, position);
	play();
	
}

void MainWindow::on_slider_sliderMoved(int position)
{
	//slider
}

void MainWindow::on_slider_sliderReleased()
{
	//slider
	int position = ui->slider->value();
	position = (position > m_countOfFrames ? m_countOfFrames : position);
	position = (position < m_numFirstFrame ? m_numFirstFrame : position);
	m_pbc->seek(*m_pDepthStream, position);
	m_tick = position;
	m_isPlay = false;
	ui->btnPlay->setText("Play");
	timer->start(50);
	play();
}

void MainWindow::on_slider_sliderPressed()
{
	//slider
	timer->stop();
	m_isPlay = false;
}

void MainWindow::Open()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString("Открыть файл"), QString("."), QString("ONI (*.oni)"));
	
	if (fileName.size() > 0) {
		QFile f(fileName);
		if (f.open(QIODevice::ReadOnly))
		{
			restart();
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
			
			m_countOfFrames = m_pbc->getNumberOfFrames(*m_pColorStream);

			ui->slider->setMaximum(m_countOfFrames);
			this->ui->btnPlay->setEnabled(true);
			
			loop();
			timer->stop();
			openni::OpenNI::shutdown();
			if (m_device.isValid()) m_device.close();
		}
	}
}

void MainWindow::loop() {
	while (!m_isExit) {
		if (m_isPlay) {
			if (!m_isStartStream) {
				m_pDepthStream->start();
				m_pColorStream->start();
				m_pbc->setSpeed(1.0);
				this->ui->btnL->setEnabled(false);
				this->ui->btnR->setEnabled(false);
				m_isStartStream = true;
			}
			play();
		}
		else {
			if (m_isStartStream) {
				m_pbc->setSpeed(-1.0);
				this->ui->btnL->setEnabled(true);
				this->ui->btnR->setEnabled(true);
				m_isStartStream = false;
			}
			QThread::msleep(5);
		}
		QCoreApplication::processEvents();
	}
}

void MainWindow::play() {
	openni::Status status = openni::STATUS_OK;
	if (m_countOfFrames > 0) {

		if (m_tick > m_countOfFrames) {
			return;
		}

		if (m_tick >= m_countOfFrames) {
			m_isPlay = false;
			ui->btnPlay->setText("Play");
		}

		//play
		openni::SensorType sensorType;
		QImage image;

		getImageFrame(sensorType, image);
		if (sensorType == openni::SensorType::SENSOR_COLOR)
		{
			colorChanel->setGeometry(0, 0, m_frameWidth, m_frameHeight);
			colorChanel->setPixmap(QPixmap::fromImage(image));
		}
		if (sensorType == openni::SensorType::SENSOR_DEPTH)
		{
			depthChanel->setGeometry(0, 0, m_frameWidth, m_frameHeight);
			depthChanel->setPixmap(QPixmap::fromImage(image));
		}
		getImageFrame(sensorType, image);
		if (sensorType == openni::SensorType::SENSOR_COLOR)
		{
			colorChanel->setGeometry(0, 0, m_frameWidth, m_frameHeight);
			colorChanel->setPixmap(QPixmap::fromImage(image));
		}
		if (sensorType == openni::SensorType::SENSOR_DEPTH)
		{
			depthChanel->setGeometry(0, 0, m_frameWidth, m_frameHeight);
			depthChanel->setPixmap(QPixmap::fromImage(image));
		}
	}
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
		status = m_pDepthStream->readFrame(&openFrame);
		cvFrame.create(openFrame.getHeight(), openFrame.getWidth(), CV_16UC1);
		cvFrame.data = (uchar*)openFrame.getData();
		image = mat2Qimgd(cvFrame);
		if (openFrame.isValid()) m_tick = openFrame.getFrameIndex();
		//qDebug() << openFrame.getFrameIndex() << "depth";
		//qDebug() << openFrame.getTimestamp()/1000 << "depth";
		break;
	}
	case 1:
	{
		sensorType = openni::SENSOR_COLOR;
		status = m_pColorStream->readFrame(&openFrame);
		const openni::RGB888Pixel* imageBuffer = (const openni::RGB888Pixel*)openFrame.getData();
		cvFrame.create(openFrame.getHeight(), openFrame.getWidth(), CV_8UC3);
		memcpy(cvFrame.data, imageBuffer, 3 * openFrame.getHeight()*openFrame.getWidth() * sizeof(uint8_t));
		cv::cvtColor(cvFrame, cvFrame, CV_BGR2RGB);
		image = mat2Qimgc(cvFrame);
		//qDebug() << openFrame.getFrameIndex() << "color";
		//qDebug() << openFrame.getTimestamp()/1000 << "color";
		break;
	}
	}
	if (!m_bNumFirstFrame) {
		int screenWidth = QApplication::desktop()->screenGeometry().width();
		int screenHeight = QApplication::desktop()->screenGeometry().height();
		m_numFirstFrame = m_tick;
		this->ui->slider->setMinimum(m_numFirstFrame);
		m_frameWidth = openFrame.getWidth();
		m_frameHeight = openFrame.getHeight();
		window->setWindowTitle("Depth/Color");
		window->move(screenWidth / 2 - m_frameWidth, this->frameSize().height());
		window->resize(m_frameWidth * 2, m_frameHeight);
		window->show();

		m_bNumFirstFrame = true;

		//qDebug() << m_pColorStream->getVideoMode().getResolutionX() << "color VideoMode";
		//qDebug() << m_pDepthStream->getVideoMode().getResolutionY() << "depth VideoMode";
	}
	//qDebug() << m_tick;
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
	openni::OpenNI::shutdown();
	if (m_device.isValid()) m_device.close();

	timer->start(50);
	m_isExit = false;
	m_isPlay = false;
	ui->btnPlay->setText("Play");
	m_bNumFirstFrame = false;
	
	window->hide();
	this->ui->slider->setEnabled(true);

	hide();
	m_pColorStream = nullptr;
	m_pDepthStream = nullptr;
	m_tick = 0;
	
	m_isStartStream = false;
	openni::OpenNI::initialize();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	m_isExit = true;
	openni::OpenNI::shutdown();
	if (m_device.isValid()) m_device.close();
	QApplication::quit();
	event->accept();
}