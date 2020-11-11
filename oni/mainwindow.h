#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QImage>
#include "imagewindow.h"
#include <QLabel>

#include <OpenNI.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>

//class Device;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void slotTimerAlarm();

    void on_btnL_clicked();

    void on_btnPlay_clicked();

    void on_btnR_clicked();

    void on_slider_sliderReleased();

    void on_slider_sliderMoved(int position);

    void on_slider_sliderPressed();

    void Open();

    void play();
    void stepFrame(int p); 
    void tickPosition();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    ImageWindow *window1;
    ImageWindow *window2;
    QLabel *imageLabel1;
    QLabel *imageLabel2;

	openni::Device m_device;
	openni::PlaybackControl* m_pbc;
	openni::VideoStream* m_pDepthStream;
	openni::VideoStream* m_pColorStream;
	openni::VideoStream** m_vsArr;
	int m_tick;
	int m_countOfFrames;
	bool m_isPlay;
	bool m_isExit;
<<<<<<< HEAD
=======
	bool m_isStartStream;
>>>>>>> 9216dec

private:
	void getImageFrame(openni::SensorType& sensorType, QImage& image);
	QImage mat2Qimgc(const cv::Mat &);
	QImage mat2Qimgd(const cv::Mat &);
	void loop();
	void restart();
	
};
#endif // MAINWINDOW_H
