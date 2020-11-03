#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QFileDialog>
#include <QFile>
#include <QBasicTimer>
#include <QLabel>
#include <QGraphicsPixmapItem>

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

	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));

}

void MainWindow::slotTimerAlarm()
{
	play();
	tickPosition();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_btnL_clicked()
{
	//left
	stepFrame(-1);
}

void MainWindow::on_btnPlay_clicked()
{
	if (pairs.count < 1) { return; }

	if (timer->isActive()) {
		timer->stop();
		ui->btnPlay->setText("Play");
	}
	else {
		if (tick >= pairs.count) { tick = 0; }
		timer->start(28);
		ui->btnPlay->setText("Stop");

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

void MainWindow::play() {
	if (pairs.count > 0) {
		std::string s = std::to_string(tick);
		s += "/";
		s += std::to_string(pairs.count);
		ui->countframe->setText(s.c_str());

		if (tick >= pairs.count - 1) {
			timer->stop();
			ui->btnPlay->setText("Play");
		}

		if (tick >= pairs.count) {
			return;
		}

		//play

		QImage color = pairs.ip[tick].cframe;
		QImage depth = pairs.ip[tick].dframe;

		if (tick == 0) {
			window1->setWindowTitle("Depth");
			window1->setGeometry(30, 180, 640, 480);
			window1->show();

			window2->setWindowTitle("Color");
			window2->setGeometry(700, 180, 640, 480);
			window2->show();

		}

		imageLabel1->setGeometry(0, 0, 640, 480);
		imageLabel1->setPixmap(QPixmap::fromImage(depth));


		imageLabel2->setGeometry(0, 0, 640, 480);
		imageLabel2->setPixmap(QPixmap::fromImage(color));


		tick += 1;

	}
}

void MainWindow::tickPosition()
{
	ui->slider->setValue(tick);
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
	if (fileName.size() > 0) {
		QFile f(fileName);
		if (f.open(QIODevice::ReadOnly))
		{
			window1->hide();
			window2->hide();
			hide();
			//static int count = tools::counts(f.fileName().toStdString().c_str());
			pairs = tools::frames(f.fileName().toStdString().c_str());
			f.close();
			show();
			ui->slider->setValue(0);
			int sw = ui->slider->width();
			int m = pairs.count;
			if (m > sw) {
				int step = int(m / sw);
				ui->slider->setPageStep(step);
			}
			ui->slider->setMaximum(m);
			tick = 0;
			play();
		}
	}
}
