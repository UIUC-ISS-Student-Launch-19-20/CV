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

//s    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    //pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    //pyrUp(pyr, timg, image.size());


    // blur will enhance edge detection
    Mat timg(image);
    medianBlur(image, timg, 9);
    Mat gray0(timg.size(), CV_8U), gray;

    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for( int c = 0; c < 3; c++ )
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
                Canny(gray0, gray, 5, thresh, 5);
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
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

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
                    if( maxCosine < 0.3 )
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
        //dont detect the border
        if (p-> x > 3 && p->y > 3)
          polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, LINE_AA);
    }

    imshow(wndname, image);


}


int main(int /*argc*/, char** /*argv*/)
{
    VideoCapture cap(0);
    if (!cap.isOpened() )
    {
        cout<< "Cannot open the web cam" << endl;
        return -1;
    }

    Mat image;
    namedWindow(wndname, 1);
    vector<vector<Point> > squares;

    //hue (0 - 256)
    // max = 38 for yellow
    int H_MIN = 15;
    int H_MAX = 69;

    //saturation (0 - 256)
    int S_MIN = 100;    
    int S_MAX = 255;

    //value (0 - 256)
    int V_MIN = 100;
    int V_MAX = 255;

    // //trackbars, might get rid of these later...might be cool we'll see
    // cvCreateTrackbar("H_MIN", "Control", &H_MIN, 38);
    // cvCreateTrackbar("H_MAX", "Control", &H_MAX, 38);

    // cvCreateTrackbar("S_MIN", "Control", &S_MIN, 256);
    // cvCreateTrackbar("S_MAX", "Control", &S_MAX, 256);

    // cvCreateTrackbar("V_MIN", "Control", &V_MIN, 256);
    // cvCreateTrackbar("V_MAX", "Control", &V_MAX, 256);

    while (true)
    {
        // original image matrix
        Mat imgOriginal;
        bool bSuccess = cap.read(imgOriginal);
        //if shit breaks
        if (!bSuccess)
        {
            cout << "Cannot read video stream frame" << endl;
            break;
        }
        //Mat imgYUV;
        Mat imgHSV;
        // cvtColor(imgOriginal, imgYUV, CV_BGR2YUV);
        // vector<cv::Mat> channels;
        // split(imgYUV, channels);
        // equalizeHist(channels[0], channels[0]);
        // merge(channels, imgYUV);
        // cvtColor(imgYUV, imgOriginal, CV_YUV2BGR);

        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

        Mat imgThresholded;
        inRange(imgHSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), imgThresholded); //Threshold the image

        // findSquares(imgThresholded, squares);
        // drawSquares(imgThresholded, squares);
      
        //morphological opening (remove small objects from the foreground)
       // erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        //dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

        //morphological closing (fill small holes in the foreground)
        //dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
        //erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        imshow("Thresholded Image", imgThresholded); //show the thresholded image
        imshow("Original", imgOriginal); //show the original image
        if (waitKey(30) == 27)
        {
            cout << "esc key is pressed by user" << endl;
            break; 
       }
    }

   return 0;

}

//     static const char* names[] = { "imgs/2Stickies.jpg", "imgs/manyStickies.jpg",0 };
//     help();
//     namedWindow( wndname, 1 );
//     vector<vector<Point> > squares;

//     for( int i = 0; names[i] != 0; i++ )
//     {
//         Mat image = imread(names[i], 1);
//         if( image.empty() )
//         {
//             cout << "Couldn't load " << names[i] << endl;
//             continue;
//         }

//         findSquares(image, squares);
//         drawSquares(image, squares);
//         //imwrite( "out", image );
//         int c = waitKey();
//         if( (char)c == 27 )
//             break;
//     }

//     return 0;
// }
