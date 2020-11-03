#ifndef TOOLS_H
#define TOOLS_H

#include <QImage>
#include "opencv2/opencv.hpp"
#include <QCoreApplication>

class tools
{
public:
    struct ImagePair{
        QImage dframe;
        QImage cframe;
        int index;
    };

    struct Pairs{
        ImagePair* ip;
        int count;
    };

    static Pairs frames(const char* fileName);
    static int counts(const char* fileName);
    static void Loging(const char* text);
    static QImage mat2Qimgc(const cv::Mat &src);
    static QImage mat2Qimgd(const cv::Mat &source);
};

#endif // TOOLS_H
