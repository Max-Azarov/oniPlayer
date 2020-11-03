/********************************************************************************
** Form generated from reading UI file 'imagewindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGEWINDOW_H
#define UI_IMAGEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ImageWindow
{
public:
    QWidget *centralwidget;

    void setupUi(QMainWindow *ImageWindow)
    {
        if (ImageWindow->objectName().isEmpty())
            ImageWindow->setObjectName(QString::fromUtf8("ImageWindow"));
        ImageWindow->resize(640, 480);
        centralwidget = new QWidget(ImageWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        ImageWindow->setCentralWidget(centralwidget);

        retranslateUi(ImageWindow);

        QMetaObject::connectSlotsByName(ImageWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ImageWindow)
    {
        ImageWindow->setWindowTitle(QCoreApplication::translate("ImageWindow", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ImageWindow: public Ui_ImageWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEWINDOW_H
