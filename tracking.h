#ifndef TRACKING_H
#define TRACKING_H

#include <QDialog>
// new
// Module "core"
#include <opencv2/core/core.hpp>

// Module "highgui"
#include <opencv2/highgui/highgui.hpp>

// Module "imgproc"
#include <opencv2/imgproc/imgproc.hpp>

// Module "video"
#include <opencv2/video/video.hpp>

// Output
#include <iostream>

// Vector
#include <vector>

using namespace std;

// >>>>> Color to be tracked
#define MIN_H_BLUE 200
#define MAX_H_BLUE 300
// <<<<< Color to be tracked

// end new
//begin cam
#include "opencv2/core/utility.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <cctype>
//end cam

namespace Ui {
class Tracking;
}

class Tracking : public QDialog
{
    Q_OBJECT

public:
    explicit Tracking(QWidget *parent = 0);
    ~Tracking();


private slots:
    void execute();
    void exexute1();

private:
   /* static cv::UMat image;
    static bool backprojMode ;
    static bool selectObject ;
    static int trackObject ;
    static bool showHist ;
    static cv::Rect selection;
    static int vmin , vmax , smin ;
*/
private:
    Ui::Tracking *ui;
    void runKalmanFilter();
    void cam();

};

#endif // TRACKING_H
