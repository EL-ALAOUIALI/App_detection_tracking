#ifndef KALMAN_H
#define KALMAN_H

#include <QDialog>

namespace Ui {
class kalman;
}

class kalman : public QDialog
{
    Q_OBJECT

public:
    explicit kalman(QWidget *parent = 0);
    ~kalman();

private:
    Ui::kalman *ui;
};

#endif // KALMAN_H
