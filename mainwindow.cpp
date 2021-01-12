#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScreen>
#include <QRgb>
#include <QApplication>
#include <QDesktopWidget>
#include <iostream>
#include <thread>
#include <cstdlib>
#include <sstream>
#include <string>
#include <QImage>
#include "keymap.h"

bool running = false;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    running = false;
    delete ui;
}

void MainWindow::doColor(int delay) {
    KeyMap map(KeyMap::K70MK2, KeyMap::US);
    QStringList keyList = map.byPosition();
    QStringListIterator iterator(keyList);
    QImage image;

    while(running) {
        Key key;
        QRgb rgb;
        std::stringstream rgbstream;
        iterator.toFront();
        image = QApplication::primaryScreen() -> grabWindow(QApplication::desktop() -> winId()).toImage();

        while(iterator.hasNext()) {
            key = map.key(iterator.next());

            int x = (int)((double)key.x / map.width() * image.width());
            int y = (int)((double)key.y / map.height() * image.height());

            if(x >= 0 && y >= 0 && x <= image.width() && y <= image.height()) {
                rgb = image.pixel(x, y);



                rgbstream.str("");
                rgbstream.clear();
                rgbstream << std::hex << qRed(rgb) ;
                rgbstream << std::hex << qGreen(rgb);
                rgbstream << std::hex << qBlue(rgb);

                std::string command = "echo \"rgb " + std::string(key.name) + ":" + rgbstream.str() + "ff\" > /tmp/ckbpipe000";

                std::system(command.c_str());
            }
        }

        std::this_thread::sleep_for (std::chrono::milliseconds(delay));

    }
}

void MainWindow::on_pushButton_clicked()
{
    if(!running) {
        running = true;
        std::thread colorThread(doColor, ui->spinBox->value());
        colorThread.detach();
        ui->pushButton->setText("Stop!!!!!");
    }
    else {
        running = false;
        ui->pushButton->setText("Start!!!!!");
    }
}
