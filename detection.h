#ifndef DETECTION_H
#define DETECTION_H

#include <QDialog>
// hog

#include <iostream>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/ml.hpp>
//hog
namespace Ui {
class detection;
}

class detection : public QDialog
{
    Q_OBJECT

public:
    explicit detection(QWidget *parent = 0);
    ~detection();
    void drawHOG(std::vector<float>::const_iterator hog,  int numberOfBins, cv::Mat &image,float scale);
    void drawHOGDescriptors(const cv::Mat &image, cv::Mat &hogImage, cv::Size cellSize,int nBins);

private slots:
    void executehog();

private:
    Ui::detection *ui;
    void HOG();
};
//hog




#endif // DETECTION_H
