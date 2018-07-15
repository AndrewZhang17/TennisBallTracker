#include <opencv2\opencv.hpp>
#include <opencv2\tracking.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Ball.h"
#include "Track.h"

using namespace cv;
using namespace std;

//Windows to show camera feed
const string cameraWindow = "Camera";
const string HSVWindow = "HSV";
const string thresholdWindow = "Threshold";

//Lower and upper bounds of tennis ball color
int MIN_H = 255;
int MAX_H = 0;
int MIN_S = 255;
int MAX_S = 0;
int MIN_V = 255;
int MAX_V = 0;

Rect2d roi;

static void onMouse(int event, int x, int y, int flags, void *param) {
	if (event == EVENT_LBUTTONDOWN) {
		Mat* hsvImg = (Mat*)param;
		Vec3b intensity = hsvImg->at<Vec3b>(y, x);
		MIN_H = min((int)intensity.val[0], MIN_H) - 20;
		MAX_H = max((int)intensity.val[0], MAX_H) + 20;
		MIN_S = min((int)intensity.val[1], MIN_S) - 20;
		MAX_S = max((int)intensity.val[1], MAX_S) + 20;
		MIN_V = min((int)intensity.val[2], MIN_V) - 20;
		MAX_V = max((int)intensity.val[2], MIN_V) + 20;
	}
	if (event == EVENT_RBUTTONDOWN) {
		MIN_H = 255;
		MAX_H = 0;
		MIN_S = 255;
		MAX_S = 0;
		MIN_V = 255;
		MAX_V = 0;
	}
}

int main(int argc, char* argv[]) {

	//Create matrix to store camera, hsv, and threshold images
	Mat image;
	Mat HSV;
	Mat threshold;

	
	Ptr<Tracker> tracker = TrackerKCF::create();

	Ball ball = Ball();
	Track tr = Track(image, threshold);

	//Initialize capture
	VideoCapture cap;
	cap.open(0);

	//Create windows to show images
	namedWindow("Trackbars", WINDOW_NORMAL);
	namedWindow(cameraWindow, WINDOW_NORMAL);
	namedWindow(HSVWindow, WINDOW_NORMAL);
	namedWindow(thresholdWindow, WINDOW_NORMAL);

	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar("H_MIN", "Trackbars", &MIN_H, MAX_H);
	createTrackbar("H_MAX", "Trackbars", &MAX_H, MAX_H);
	createTrackbar("S_MIN", "Trackbars", &MIN_S, MAX_S);
	createTrackbar("S_MAX", "Trackbars", &MAX_S, MAX_S);
	createTrackbar("V_MIN", "Trackbars", &MIN_V, MAX_V);
	createTrackbar("V_MAX", "Trackbars", &MAX_V, MAX_V);
	/*setTrackbarPos("H_MIN", "Trackbars", 25);
	setTrackbarPos("H_MAX", "Trackbars", 75);
	setTrackbarPos("S_MIN", "Trackbars", 60);
	setTrackbarPos("S_MAX", "Trackbars", 200);
	setTrackbarPos("V_MIN", "Trackbars", 50);
	setTrackbarPos("V_MAX", "Trackbars", 255);*/

	//MIN_H = 25;
	//MAX_H = 75;
	//MIN_S = 60;
	//MAX_S = 200;
	//MIN_V = 50;
	//MAX_V = 255;


	while (1) {
		//store camera feed to image
		cap.read(image);

		//convert to HSV
		cvtColor(image, HSV, COLOR_BGR2HSV);

		Point pt = Point();
		setMouseCallback(cameraWindow, onMouse, (void*)&HSV);

		
		

		//filter HSV image to threshold
		inRange(HSV, Scalar(MIN_H, MIN_S, MIN_V), Scalar(MAX_H, MAX_S, MAX_V), threshold);

		//Morphological operations to reduce noise in threshold image
		tr.morphOps();

		//Detect ball and store position information
		tr.track(ball);
		//find speed
		ball.speed();

		//print images to screen
		imshow(cameraWindow, image);
		imshow(HSVWindow, HSV);
		imshow(thresholdWindow, threshold);

		//delay 1ms
		waitKey(1);
	}

	return 0;
}
