/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action;
    QWidget *centralWidget;
    QSlider *slider;
    QPushButton *btnL;
    QPushButton *btnPlay;
    QPushButton *btnR;
    QLabel *countframe;
    QMenuBar *menuBar;
    QMenu *menuFile;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(600, 92);
        action = new QAction(MainWindow);
        action->setObjectName(QString::fromUtf8("action"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        slider = new QSlider(centralWidget);
        slider->setObjectName(QString::fromUtf8("slider"));
        slider->setGeometry(QRect(10, 45, 550, 19));
        slider->setFocusPolicy(Qt::NoFocus);
        slider->setPageStep(10);
        slider->setOrientation(Qt::Horizontal);
        btnL = new QPushButton(centralWidget);
        btnL->setObjectName(QString::fromUtf8("btnL"));
        btnL->setGeometry(QRect(10, 10, 75, 23));
        btnPlay = new QPushButton(centralWidget);
        btnPlay->setObjectName(QString::fromUtf8("btnPlay"));
        btnPlay->setGeometry(QRect(90, 10, 75, 23));
        btnR = new QPushButton(centralWidget);
        btnR->setObjectName(QString::fromUtf8("btnR"));
        btnR->setGeometry(QRect(170, 10, 75, 23));
        countframe = new QLabel(centralWidget);
        countframe->setObjectName(QString::fromUtf8("countframe"));
        countframe->setGeometry(QRect(260, 14, 100, 16));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(action);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "OpenNI", nullptr));
        action->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
#if QT_CONFIG(shortcut)
        action->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        btnL->setText(QCoreApplication::translate("MainWindow", "<", nullptr));
        btnPlay->setText(QCoreApplication::translate("MainWindow", "Play", nullptr));
        btnR->setText(QCoreApplication::translate("MainWindow", ">", nullptr));
        countframe->setText(QCoreApplication::translate("MainWindow", "0/0", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
