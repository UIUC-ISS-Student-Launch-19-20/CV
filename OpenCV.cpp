// The "Square Detector" program.
// It loads several images sequentially and tries to find squares in
// each image

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <math.h>
#include <string.h>

using namespace cv;
using namespace std;

int thresh = 50, N = 5;
const char* wndname = "Square Detection";
const string trackbarWindowName = "Trackbars";

    //hue (0 - 256)
    // max = 38 for yellow
    int H_MIN = 22;
    int H_MAX = 38;

    //saturation (0 - 256)
    int S_MIN = 90;    
    int S_MAX = 256;

    //value (0 - 256)
    int V_MIN = 150;
    int V_MAX = 256;

// int theObject[2] = {0, 0};
// Rect objectBoundingRectangle = Rect(0, 0, 0, 0);
// bool objectDetected = false;
// Mat temp;



//int to string helper function
string intToString(int number){

    //this function has a number input and string output
    std::stringstream ss;
    ss << number;
    return ss.str();
}

void on_trackbar( int, void* )
{//This function gets called whenever a
	// trackbar position is changed
}

void createTrackbars(){
	//create window for trackbars


    namedWindow(trackbarWindowName,0);

	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH),
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->
    createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
    createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
    createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
    createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
    createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
    createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );


}

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}


// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
static void findSquares( const Mat& image, vector<vector<Point> >& squares )
{
    squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());

    // Mat timg(image);
    // medianBlur(image, timg, 9);
    // Mat gray0(timg.size(), CV_8U), gray;

    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for( int c = 0; c < image.channels(); c++ )
    {
        int ch[] = {c, 0};
        mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for( int l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
            }

            // find contours and store them all as a list
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            vector<Point> approx;


            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.06, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 1000 &&
                    isContourConvex(Mat(approx)) )
                {
                    double maxCosine = 0;

                    for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( maxCosine < .9)
                        squares.push_back(approx);
                }
            }
        }
    }
}


// the function draws all the squares in the image
static void drawSquares( Mat& image, const vector<vector<Point> >& squares )
{
    for( size_t i = 0; i < squares.size(); i++ )
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255, 255), 3, CV_8U);
    }

    imshow(wndname, image);
}

void getPosition(Mat& imageThres, Mat& imgOrig) { 
    Moments oMoments = moments(imageThres);
        
    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;

    if (dArea > 10000) {
        int posX = dM10/dArea;
        int posY = dM01/dArea;
            
        putText(imgOrig,"Tracking object at (" + intToString(posX)+","+intToString(posY)+")",Point(posX,posY),1,1,Scalar(0,0,0),2);

    }
}


int main(int /*argc*/, char** /*argv*/)
{
    Mat imgOriginal;
    Mat imgHSV;
    Mat imgThresholded;
    

    VideoCapture cap(0);
    if (!cap.isOpened() )
    {
        cout<< "Cannot open the web cam" << endl;
        return -1;
    }

    namedWindow(wndname, 1);
    vector<vector<Point> > squares;
    
    while (true)
    {

        bool bSuccess = cap.read(imgOriginal);
        //if shit breaks
        if (!bSuccess)
        {
            cout << "Cannot read video stream frame" << endl;
            break;
        }
        
        
        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

        inRange(imgHSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), imgThresholded); //Threshold the image

        GaussianBlur(imgThresholded, imgThresholded, cv::Size(3, 3), 0);   //Blur Effect
        //morphological opening (remove small objects from the foreground)
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
        //dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) ); 

        //morphological closing (fill small holes in the foreground)
        //dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) ); 
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );

        //do it again
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );

        getPosition(imgThresholded, imgOriginal);

        createTrackbars();

        imshow("Thresholded Image", imgThresholded); //show the thresholded image
        imshow("Original", imgOriginal); //show the original image

        findSquares(imgThresholded, squares);
        drawSquares(imgOriginal, squares);

        if (waitKey(30) == 27)
        {
            cout << "ya broke it :( " << endl;
            break; 
       }
    }

   return 0;

}