#ifndef DECTRACK_H
#define DECTRACK_H

#include <QMainWindow>

namespace Ui {
class DecTrack;
}

class DecTrack : public QMainWindow
{
    Q_OBJECT

public:
    explicit DecTrack(QWidget *parent = 0);
    ~DecTrack();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    //void on_pushButton_3_clicked();


private:
    Ui::DecTrack *ui;


};

#endif // PRIN_H
