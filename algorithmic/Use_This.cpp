#include <iostream>
#include <string>

#include "opencv2/highgui.hpp"
#include "opencv/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int theObject[2] = ( 0, 0 );
Rect objectboundingrectangle = Rect(0, 0, 0, 0);

void searchForMovement (Mat thresholdImage, Mat &cameraFeed){
    bool objectDetected = false;
    Mat temp;
    thresholdimage.copyto(temp);

    //output of findCountours
    vector< vector<Point> > countours;
    vector<Vec4i> hierarchy;
    /*void findContours(InputOutputArray image, OutputArrayOfArrays contours,
    OutputArray hierarchy, int mode, int method, Point offset=Point())

    int cvFindContours(CvArr* image, CvMemStorage* storage, CvSeq** first_contour,
    int header_size=sizeof(CvContour), int mode=CV_RETR_LIST, 
    Int method=CV_CHAIN_APPROX_SIMPLE, CvPoint offset=cvPoint(0,0) ) */

    //May need to add Point offset for ROI
    //CV_RETR_EXTERNAL retrieves only the extreme outer contours. It sets hierarchy[i][2]=hierarchy[i][3]=-1 for all the contours.
    /*CV_CHAIN_APPROX_SIMPLE compresses horizontal, vertical, and diagonal segments and leaves only their end points. For example, 
    an up-right rectangular contour is encoded with 4 points. */
    findCountours(temp, countours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    if (countours.size() > 0)objectDetected = true;
    else objectDetected = false;

    //This is based on the assumption that the biggest contour is the object we are looking for...might be problematic
    if (objectDetected) {
        vector< vector<Point> > largestContourVec;
        largestContourVec.push_back(contours.at(contours.size() - 1));
        
        //applying bounds and finding centeroid to determine the object's estimated position
        //Rect boundingRect(InputArray points)
        //CvRect cvBoundingRect(CvArr* points, int update=0 )
        //Parameters:	points – Input 2D point set, stored in std::vector or Ma
        objectBoundingRectangle = boundingRect(largestContourVec.at(0));
        int xpos = objectBoundingRectangle.x + objectBoundingRectangle.width / 2;
        int ypos = objectBoundingRectangle.y + objectBoundingRectangle.length / 2;

        //update the object position in arraty values
        theObject[0] = xpos, theObject[1] = ypos;
    }

    //make my life easierrrrrrr
    int x = theObject[0];
    int y = theObject[1];

    //drawing on object (I hope I'm doing this right)
    //Draw 4 lines (a plus sign) at the center of the object
    circle(cameraFeed, Point(x, y), 20, Scalar(0, 255, 0), 2);
    line(cameraFeed, Point(x, y - 25), 20, Scalar(0, 255, 0), 2);
    line(cameraFeed, Point(x, y + 25), 20, Scalar(0, 255, 0), 2);
    line(cameraFeed, Point(x - 25, y), 20, Scalar(0, 255, 0), 2);
    line(cameraFeed, Point(x + 25, y), 20, Scalar(0, 255, 0), 2);
    putText(cameraFeed, "Tracking object at ("+ x, Point(x, y), 1, 1, Scalar(0, 0, 255), 2);
}

//aright here we go
int main(int argc, char** argv)
{
    //capture video from camera
    VideoCapture cap(0);

    //If it doesn't work, exit the program
    if(!cap.isOpened())
    {
        cout << "Cannot open the camera" << endl;
        return -1;
    }

    //Making window 
    nameWindow("Control", CV_WINDOW_NORMAL);

    //hue (0 - 256)
    int H_MIN = 0;
    int H_MAX = 256;

    //saturation (0 - 256)
    int S_MIN = 0;    
    int S_MAX = 256;

    //value (0 - 256)
    int V_MIN = 0;
    int V_MAX = 256;

    //trackbars, might get rid of these later...might be cool we'll see
    cvCreateTrackbar("H_MIN", "Control", &H_MIN, 256);
    cvCreateTrackbar("H_MAX", "Control", &H_MAX, 256);

    cvCreateTrackbar("S_MIN", "Control", &S_MIN, 256);
    cvCreateTrackbar("S_MAX", "Control", &S_MAX, 256);

    cvCreateTrackbar("V_MIN", "Control", &V_MIN, 256);
    cvCreateTrackbar("V_MAX", "Control", &V_MAX, 256);

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
        // HSV matrix 


    }
}