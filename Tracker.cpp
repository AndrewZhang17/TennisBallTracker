#include <opencv2\opencv.hpp>
#include <chrono>
#include "Tracker.h"
#include "Ball.h"

using namespace std;
using namespace cv;

Tracker::Tracker(Mat& image, Mat& thresh) {
	this->image = &image;
	this->thresh = &thresh;
}

Tracker::~Tracker() {
	delete image;
	delete thresh;
}

void Tracker::morphOps() {
	Mat erodeElement = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));

	erode(*thresh, *thresh, erodeElement);
	erode(*thresh, *thresh, erodeElement);

	dilate(*thresh, *thresh, dilateElement);
	dilate(*thresh, *thresh, dilateElement);
	dilate(*thresh, *thresh, dilateElement);
	dilate(*thresh, *thresh, dilateElement);
	dilate(*thresh, *thresh, dilateElement);
}

void Tracker::track(Ball& ball) {
	Mat temp;
	thresh->copyTo(temp);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	//make sure tracking largest object
	double smallerArea = 0;
	//if object has been found
	bool objectFound = false;
	//location
	int x = 0, y = 0, radius = 0, time = 0;

	if (hierarchy.size() > 0) {
		int numObjects = (int) hierarchy.size();
		//check if noisy filter
		if (numObjects < MAX_OBJECTS) {
			for (int i = 0; i >= 0; i = hierarchy[i][0]) {
				Moments moment = moments((Mat)contours[i]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is more than 400 px by 400 px, probably just a bad filter
				//we only want the object with the largest area so we save smaller area each
				//iteration and compare it to the area in the next iteration.
				if (area > MIN_OBJECT_AREA && area < MAX_OBJECT_AREA) {
					if (area > smallerArea) {
						x = (int)(moment.m10 / area);
						y = (int)(moment.m01 / area);
						radius = (int)(sqrt(area / (atan(1) * 4)));
						time = (int)chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
						objectFound = true;
						smallerArea = area;
					}
				}
				else {
					objectFound = false;
				}
			}

			//if object is found, show on screen
			if (objectFound == true) {
				ball.addPosition(x, y, radius, time);
				putText(*image, "Tracking Object", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				circle(*image, Point(x, y), radius, Scalar(0, 255, 0));
			}
			else {
				time = (int)chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
				ball.clear(time);
			}
		}
		else {
			putText(*image, "TOO MUCH NOISE!", Point(0, 50), 1, 2, Scalar(0, 0, 255));
			ball.clear();
		}
	}
	else {
		ball.clear();
	}
}