#include "tracking.h"
#include "ui_tracking.h"

Tracking::Tracking(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Tracking)
{
    ui->setupUi(this);
    // Connect button signal to appropriate slot
    connect(ui->execute, SIGNAL (released()), this, SLOT (execute()));
    connect(ui->exexute1, SIGNAL (released()), this, SLOT (exexute1()));
}

Tracking::~Tracking()
{
    delete ui;
}



/*bool selectObject = false;
//cv::Point origin;
cv::Rect selection;
int vmin = 10;
int trackObject = 0;
int vmax = 256;
int smin = 30;
bool showHist = true;
cv::UMat image;
  bool  backprojMode = false;*/
static cv::UMat image;
   static bool backprojMode ;
   static bool selectObject ;
   static int trackObject ;
   static bool showHist ;
   static cv::Rect selection;
   static int vmin=10 , vmax=256 , smin=30 ;
static void onMouse(int event, int x, int y, int, void*)
{
    static cv::Point origin;

    if (selectObject)
    {
        selection.x = std::min(x, origin.x);
        selection.y = std::min(y, origin.y);
        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);

        selection &= cv::Rect(0, 0, image.cols, image.rows);
    }

    switch (event)
    {
    case cv::EVENT_LBUTTONDOWN:
        origin = cv::Point(x, y);
        selection = cv::Rect(x, y, 0, 0);
        selectObject = true;
        break;
    case cv::EVENT_LBUTTONUP:
        selectObject = false;
        if (selection.width > 0 && selection.height > 0)
            trackObject = -1;
        break;
    default:
        break;
    }
}



static void help()
{
    std::cout << "\nThis is a demo that shows mean-shift based tracking using Transparent API\n"
            "You select a color objects such as your face and it tracks it.\n"
            "This reads from video camera (0 by default, or the camera number the user enters\n"
            "Usage: \n"
            "   ./camshiftdemo [camera number]\n";

    std::cout << "\n\nHot keys: \n"
            "\tESC - quit the program\n"
            "\ts - stop the tracking\n"
            "\tb - switch to/from backprojection view\n"
            "\th - show/hide object histogram\n"
            "\tp - pause video\n"
            "\tc - use OpenCL or not\n"
            "To initialize tracking, select the object with mouse\n";
}







void Tracking::cam()
{
    help();
/*  bool  backprojMode = false;
    bool selectObject = false;
  //  origin =cv::Point(0, 0);
    cv::Rect selection;
    int vmin = 10;
    int trackObject = 0;
    int vmax = 256;
    int smin = 30;
    bool showHist = true;
    cv::UMat image;*/
    cv::VideoCapture cap;
    cv::Rect trackWindow;
    int hsize = 16;
    float hranges[2] = { 0, 180 };

//   const char * const keys = { "{@camera_number| 0 | camera number}" };
 //  cv::CommandLineParser parser(argc, argv, keys);
//    int camNum = parser.get<int>(0);

    cap.open(0);
    // cv::VideoCapture cap;


    if (!cap.isOpened())
    {
        help();

        std::cout << "***Could not initialize capturing...***\n";
        std::cout << "Current parameter's value: \n";
   //     parser.printMessage();

       // return EXIT_FAILURE;
    }

    cv::namedWindow("Histogram", cv::WINDOW_NORMAL);
    cv::namedWindow("CamShift Demo", cv::WINDOW_NORMAL);
    cv::setMouseCallback("CamShift Demo", onMouse,0);
    // cv::setMouseCallback( "CAMShift Video Labeler", onMouse, 0 );
    cv::createTrackbar("Vmin", "CamShift Demo", &vmin, 256);
    cv::createTrackbar("Vmax", "CamShift Demo", &vmax, 256);
    cv::createTrackbar("Smin", "CamShift Demo", &smin, 256);

    cv::Mat frame, histimg(200, 320, CV_8UC3, cv::Scalar::all(0));
    cv::UMat hsv, hist, hue, mask, backproj;
    bool paused = false;

    for ( ; ; )
    {
        if (!paused)
        {
            cap >> frame;
            if (frame.empty())
                break;
        }

        frame.copyTo(image);

        if (!paused)
        {
            cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

            if (trackObject)
            {
                int _vmin = vmin, _vmax = vmax;

                cv::inRange(hsv, cv::Scalar(0, smin, std::min(_vmin, _vmax)),
                        cv::Scalar(180, 256, std::max(_vmin, _vmax)), mask);

                int fromTo[2] = { 0,0 };
                hue.create(hsv.size(), hsv.depth());
                cv::mixChannels(std::vector<cv::UMat>(1, hsv), std::vector<cv::UMat>(1, hue), fromTo, 1);

                if (trackObject < 0)
                {
                    cv::UMat roi(hue, selection), maskroi(mask, selection);
                    cv::calcHist(std::vector<cv::Mat>(1, roi.getMat(cv::ACCESS_READ)), std::vector<int>(1, 0),
                                 maskroi, hist, std::vector<int>(1, hsize), std::vector<float>(hranges, hranges + 2));
                    cv::normalize(hist, hist, 0, 255, cv::NORM_MINMAX);

                    trackWindow = selection;
                    trackObject = 1;

                    histimg = cv::Scalar::all(0);
                    int binW = histimg.cols / hsize;
                    cv::Mat buf (1, hsize, CV_8UC3);
                    for (int i = 0; i < hsize; i++)
                        buf.at<cv::Vec3b>(i) = cv::Vec3b(cv::saturate_cast<uchar>(i*180./hsize), 255, 255);
                    cv::cvtColor(buf, buf, cv::COLOR_HSV2BGR);

                    {
                        cv::Mat _hist = hist.getMat(cv::ACCESS_READ);
                        for (int i = 0; i < hsize; i++)
                        {
                            int val = cv::saturate_cast<int>(_hist.at<float>(i)*histimg.rows/255);
                            cv::rectangle(histimg, cv::Point(i*binW, histimg.rows),
                                       cv::Point((i+1)*binW, histimg.rows - val),
                                       cv::Scalar(buf.at<cv::Vec3b>(i)), -1, 8);
                        }
                    }
                }

                cv::calcBackProject(std::vector<cv::UMat>(1, hue), std::vector<int>(1, 0), hist, backproj,
                                    std::vector<float>(hranges, hranges + 2), 1.0);
                cv::bitwise_and(backproj, mask, backproj);

                cv::RotatedRect trackBox = cv::CamShift(backproj, trackWindow,
                                    cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 1));
                if (trackWindow.area() <= 1)
                {
                    int cols = backproj.cols, rows = backproj.rows, r = (std::min(cols, rows) + 5)/6;
                    trackWindow = cv::Rect(trackWindow.x - r, trackWindow.y - r,
                                       trackWindow.x + r, trackWindow.y + r) &
                                  cv::Rect(0, 0, cols, rows);
                }

                if (backprojMode)
                    cv::cvtColor(backproj, image, cv::COLOR_GRAY2BGR);

                {
                    cv::Mat _image = image.getMat(cv::ACCESS_RW);
                    cv::ellipse(_image, trackBox, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
                }
            }
        }
        else if (trackObject < 0)
            paused = false;

        if (selectObject && selection.width > 0 && selection.height > 0)
        {
            cv::UMat roi(image, selection);
            cv::bitwise_not(roi, roi);
        }

        cv::imshow("CamShift Demo", image);
        if (showHist)
            cv::imshow("Histogram", histimg);

        char c = (char)cv::waitKey(10);
        if (c == 81 || c ==113 )
            break;

        switch(c)
        {
        case 'b':
            backprojMode = !backprojMode;
            break;
        case 't':
            trackObject = 0;
            histimg = cv::Scalar::all(0);
            break;
        case 'h':
            showHist = !showHist;
            if (!showHist)
                cv::destroyWindow("Histogram");
            else
                cv::namedWindow("Histogram", cv::WINDOW_AUTOSIZE);
            break;
        case 'p':
            paused = !paused;
            break;
        case 'c':
            cv::ocl::setUseOpenCL(!cv::ocl::useOpenCL());
        default:
            break;
        }
    }

   // return EXIT_SUCCESS;
}

void Tracking::execute()
{
    ui->execute->setDisabled(true);

     runKalmanFilter();
}
void Tracking::runKalmanFilter() {
    // Camera frame
    cv::Mat frame;

    // >>>> Kalman Filter
    int stateSize = 6;
    int measSize = 4;
    int contrSize = 0;

    unsigned int type = CV_32F;
    cv::KalmanFilter kf(stateSize, measSize, contrSize, type);

    cv::Mat state(stateSize, 1, type);  // [x,y,v_x,v_y,w,h]
    cv::Mat meas(measSize, 1, type);    // [z_x,z_y,z_w,z_h]
    //cv::Mat procNoise(stateSize, 1, type)
    // [E_x,E_y,E_v_x,E_v_y,E_w,E_h]

    // Transition State Matrix A
    // Note: set dT at each processing step!
    // [ 1 0 dT 0  0 0 ]
    // [ 0 1 0  dT 0 0 ]
    // [ 0 0 1  0  0 0 ]
    // [ 0 0 0  1  0 0 ]
    // [ 0 0 0  0  1 0 ]
    // [ 0 0 0  0  0 1 ]
    cv::setIdentity(kf.transitionMatrix);

    // Measure Matrix H
    // [ 1 0 0 0 0 0 ]
    // [ 0 1 0 0 0 0 ]
    // [ 0 0 0 0 1 0 ]
    // [ 0 0 0 0 0 1 ]
    kf.measurementMatrix = cv::Mat::zeros(measSize, stateSize, type);
    kf.measurementMatrix.at<float>(0) = 1.0f;
    kf.measurementMatrix.at<float>(7) = 1.0f;
    kf.measurementMatrix.at<float>(16) = 1.0f;
    kf.measurementMatrix.at<float>(23) = 1.0f;

    // Process Noise Covariance Matrix Q
    // [ Ex   0   0     0     0    0  ]
    // [ 0    Ey  0     0     0    0  ]
    // [ 0    0   Ev_x  0     0    0  ]
    // [ 0    0   0     Ev_y  0    0  ]
    // [ 0    0   0     0     Ew   0  ]
    // [ 0    0   0     0     0    Eh ]
    //cv::setIdentity(kf.processNoiseCov, cv::Scalar(1e-2));
    kf.processNoiseCov.at<float>(0) = 1e-2;
    kf.processNoiseCov.at<float>(7) = 1e-2;
    kf.processNoiseCov.at<float>(14) = 5.0f;
    kf.processNoiseCov.at<float>(21) = 5.0f;
    kf.processNoiseCov.at<float>(28) = 1e-2;
    kf.processNoiseCov.at<float>(35) = 1e-2;

    // Measures Noise Covariance Matrix R
    cv::setIdentity(kf.measurementNoiseCov, cv::Scalar(1e-1));
    // <<<< Kalman Filter

    // Camera Index
    int idx = 0;

    // Camera Capture
    cv::VideoCapture cap;

    // >>>>> Camera Settings
    if (!cap.open(idx))
    {
        cout << "Webcam not connected.\n" << "Please verify\n";
        //return EXIT_FAILURE;
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 600);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 600);
    // <<<<< Camera Settings

    cout << "\nHit 'q' to exit...\n";

    char ch = 0;

    double ticks = 0;
    bool found = false;

    int notFoundCount = 0;

    // >>>>> Main loop
    while (ch != 'q' && ch != 'Q')
    {
        double precTick = ticks;
        ticks = (double) cv::getTickCount();

        double dT = (ticks - precTick) / cv::getTickFrequency(); //seconds

        // Frame acquisition
        cap >> frame;

        cv::Mat res;
        frame.copyTo( res );

        if (found)
        {
            // >>>> Matrix A
            kf.transitionMatrix.at<float>(2) = dT;
            kf.transitionMatrix.at<float>(9) = dT;
            // <<<< Matrix A

            cout << "dT:" << endl << dT << endl;

            state = kf.predict();
            cout << "State post:" << endl << state << endl;

            cv::Rect predRect;
            predRect.width = state.at<float>(4);
            predRect.height = state.at<float>(5);
            predRect.x = state.at<float>(0) - predRect.width / 2;
            predRect.y = state.at<float>(1) - predRect.height / 2;

            cv::Point center;
            center.x = state.at<float>(0);
            center.y = state.at<float>(1);
            cv::circle(res, center, 2, CV_RGB(255,0,0), -1);

            cv::rectangle(res, predRect, CV_RGB(255,0,0), 2);
        }

        // >>>>> Noise smoothing
        cv::Mat blur;
        cv::GaussianBlur(frame, blur, cv::Size(5, 5), 3.0, 3.0);
        // <<<<< Noise smoothing

        // >>>>> HSV conversion
        cv::Mat frmHsv;
        cv::cvtColor(blur, frmHsv, CV_BGR2HSV);
        // <<<<< HSV conversion

        // >>>>> Color Thresholding
        // Note: change parameters for different colors
        cv::Mat rangeRes = cv::Mat::zeros(frame.size(), CV_8UC1);
        cv::inRange(frmHsv, cv::Scalar(MIN_H_BLUE / 2, 100, 80),
                    cv::Scalar(MAX_H_BLUE / 2, 255, 255), rangeRes);
        // <<<<< Color Thresholding

        // >>>>> Improving the result
        cv::erode(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 2);
        cv::dilate(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 2);
        // <<<<< Improving the result

        // Thresholding viewing
        cv::imshow("Threshold", rangeRes);

        // >>>>> Contours detection
        vector<vector<cv::Point> > contours;
        cv::findContours(rangeRes, contours, CV_RETR_EXTERNAL,
                         CV_CHAIN_APPROX_NONE);
        // <<<<< Contours detection

        // >>>>> Filtering
        vector<vector<cv::Point> > balls;
        vector<cv::Rect> ballsBox;
        for (size_t i = 0; i < contours.size(); i++)
        {
            cv::Rect bBox;
            bBox = cv::boundingRect(contours[i]);

            float ratio = (float) bBox.width / (float) bBox.height;
            if (ratio > 1.0f)
                ratio = 1.0f / ratio;

            // Searching for a bBox almost square
            if (ratio > 0.75 && bBox.area() >= 400)
            {
                balls.push_back(contours[i]);
                ballsBox.push_back(bBox);
            }
        }
        // <<<<< Filtering

        cout << "Balls found:" << ballsBox.size() << endl;

        // >>>>> Detection result
        for (size_t i = 0; i < balls.size(); i++)
        {
            cv::drawContours(res, balls, i, CV_RGB(20,150,20), 1);
            cv::rectangle(res, ballsBox[i], CV_RGB(0,255,0), 2);

            cv::Point center;
            center.x = ballsBox[i].x + ballsBox[i].width / 2;
            center.y = ballsBox[i].y + ballsBox[i].height / 2;
            cv::circle(res, center, 2, CV_RGB(20,150,20), -1);

            stringstream sstr;
            sstr << "(" << center.x << "," << center.y << ")";
            cv::putText(res, sstr.str(),
                        cv::Point(center.x + 3, center.y - 3),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(20,150,20), 2);
        }
        // <<<<< Detection result

        // >>>>> Kalman Update
        if (balls.size() == 0)
        {
            notFoundCount++;
            cout << "notFoundCount:" << notFoundCount << endl;
            if( notFoundCount >= 100 )
            {
                found = false;
            }
            /*else
                kf.statePost = state;*/
        }
        else
        {
            notFoundCount = 0;

            meas.at<float>(0) = ballsBox[0].x + ballsBox[0].width / 2;
            meas.at<float>(1) = ballsBox[0].y + ballsBox[0].height / 2;
            meas.at<float>(2) = (float)ballsBox[0].width;
            meas.at<float>(3) = (float)ballsBox[0].height;

            if (!found) // First detection!
            {
                // >>>> Initialization
                kf.errorCovPre.at<float>(0) = 1; // px
                kf.errorCovPre.at<float>(7) = 1; // px
                kf.errorCovPre.at<float>(14) = 1;
                kf.errorCovPre.at<float>(21) = 1;
                kf.errorCovPre.at<float>(28) = 1; // px
                kf.errorCovPre.at<float>(35) = 1; // px

                state.at<float>(0) = meas.at<float>(0);
                state.at<float>(1) = meas.at<float>(1);
                state.at<float>(2) = 0;
                state.at<float>(3) = 0;
                state.at<float>(4) = meas.at<float>(2);
                state.at<float>(5) = meas.at<float>(3);
                // <<<< Initialization

                kf.statePost = state;

                found = true;
            }
            else
                kf.correct(meas); // Kalman Correction

            cout << "Measure matrix:" << endl << meas << endl;
        }
        // <<<<< Kalman Update

        // Final result
        cv::imshow("Tracking", res);

        // User key
        ch = cv::waitKey(1);
    }
    // <<<<< Main loop

    //return EXIT_SUCCESS;
}


void Tracking::exexute1()
{
    ui->exexute1->setDisabled(true);
    cam();
}
