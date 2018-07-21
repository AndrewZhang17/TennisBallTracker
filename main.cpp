#include <opencv2\opencv.hpp>
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

static void onMouse(int event, int x, int y, int flags, void *param) {
	if (event == EVENT_LBUTTONDOWN) {
		Mat* hsvImg = (Mat*)param;
		for (int i = -4; i <= 4; i++) {
			for (int j = -4; j <= 4; j++) {
				Vec3b intensity = hsvImg->at<Vec3b>(y + i, x + j);
				MIN_H = min((int)intensity.val[0], MIN_H);
				MAX_H = max((int)intensity.val[0], MAX_H);
				MIN_S = min((int)intensity.val[1], MIN_S);
				MAX_S = max((int)intensity.val[1], MAX_S);
				MIN_V = min((int)intensity.val[2] - 10, MIN_V);
				MAX_V = max((int)intensity.val[2] + 10, MAX_V);
			}
		}

	}
	else if (event == EVENT_RBUTTONDOWN) {
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

	Ball ball = Ball();
	Track tr = Track(&image, &threshold);
	int speed = 0;

	//Initialize capture
	VideoCapture cap;
	cap.open(0);

	//Create windows to show images
	namedWindow(cameraWindow, WINDOW_AUTOSIZE);
	namedWindow(HSVWindow, WINDOW_AUTOSIZE);
	namedWindow(thresholdWindow, WINDOW_AUTOSIZE);

	//MIN_H = 25;
	//MAX_H = 75;
	//MIN_S = 60;
	//MAX_S = 200;
	//MIN_V = 50;
	//MAX_V = 255;

	setMouseCallback(cameraWindow, onMouse, (void*)&HSV);
	while (1) {
		//store camera feed to image
		cap.read(image);

		//convert to HSV
		cvtColor(image, HSV, COLOR_BGR2HSV);

		//filter HSV image to threshold
		inRange(HSV, Scalar(MIN_H, MIN_S, MIN_V), Scalar(MAX_H, MAX_S, MAX_V), threshold);

		//Detect ball and store position information
		tr.track(ball);
		//find speed
		speed = max(ball.speed(), speed);
		putText(image, to_string(speed), Point(0, 400), FONT_HERSHEY_COMPLEX, 3, Scalar(255, 255, 0), 3);


		//print images to screen
		imshow(cameraWindow, image);
		imshow(HSVWindow, HSV);
		imshow(thresholdWindow, threshold);

		//delay 1ms
		waitKey(1);
	}

	return 0;
}
