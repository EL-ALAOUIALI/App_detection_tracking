#include "kalman.h"
#include "ui_kalman.h"

kalman::kalman(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kalman)
{
    ui->setupUi(this);
}

kalman::~kalman()
{
    delete ui;
}
