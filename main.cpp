#include <iostream>
#include <string>

#include "OpenCV.cpp"

using namespace cv;
using namespace std;

int main(int /*argc*/, char** /*argv*/)
{
    Mat imgOriginal;

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

		std::pair<int, int> position = detect(imgOriginal);

		std::cout << "(" << position.first << ", " << position.second << ")" << endl;

        if (waitKey(30) == 27)
        {
            cout << "ya broke it :( " << endl;
            break;
       }
    }

   return 0;

}
