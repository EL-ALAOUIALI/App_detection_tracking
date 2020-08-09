#include "dectrack.h"
#include "ui_dectrack.h"
#include "detection.h"
#include "tracking.h"


DecTrack::DecTrack(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DecTrack)
{
    ui->setupUi(this);
}

DecTrack::~DecTrack()
{
    delete ui;
}

void DecTrack::on_pushButton_clicked()
{
  /*  Detection detection;
    detection.setModal(true);
    detection.exec();*/
    detection detection;
    detection.setModal(true);
    detection.exec();

}

void DecTrack::on_pushButton_2_clicked()
{
    Tracking tracking;
    tracking.setModal(true);
    tracking.exec();
}

