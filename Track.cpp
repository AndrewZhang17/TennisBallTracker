#include <opencv2\opencv.hpp>
#include <chrono>
#include "Track.h"
#include "Ball.h"

using namespace std;
using namespace cv;

Track::Track(Mat* image, Mat* thresh) {
	this->image = image;
	this->thresh = thresh;
}

Track::~Track() {
	delete image;
	delete thresh;
}

void Track::morphOps() {
	Mat erodeElement = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));

	erode(*thresh, *thresh, erodeElement);
	erode(*thresh, *thresh, erodeElement);
	erode(*thresh, *thresh, erodeElement);
	erode(*thresh, *thresh, erodeElement);

	dilate(*thresh, *thresh, dilateElement);
	dilate(*thresh, *thresh, dilateElement);
	dilate(*thresh, *thresh, dilateElement);
	dilate(*thresh, *thresh, dilateElement);
	dilate(*thresh, *thresh, dilateElement);
}

void Track::track(Ball& ball) {
	morphOps();
	Mat temp;
	thresh->copyTo(temp);
	GaussianBlur(*thresh, *thresh, Size(9, 9), 2, 2);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(temp, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	vector<Vec3f> circles;
	HoughCircles(*thresh, circles, CV_HOUGH_GRADIENT, 1, thresh->rows, 100, 30, 0, 0);

	//make sure tracking largest object
	double smallerArea = 0;
	//if object has been found
	bool objectFound = false;
	//location
	int x = 0, y = 0, radius = 0, time = 0;

	if (hierarchy.size() > 0) {
		int numObjects = (int)hierarchy.size();
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
			if (objectFound || circles.size() > 0) {
				int xAvg = 0, yAvg = 0, radiusAvg = 0, count = 0;
				putText(*image, "Tracking Object", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				if (objectFound) {
					xAvg += x;
					yAvg += y;
					radiusAvg += radius;
					count++;
					circle(*image, Point(x, y), radius, Scalar(0, 255, 0));
				}
				if (circles.size() > 0) {
					xAvg += circles[0][0];
					yAvg += circles[0][1];
					radiusAvg += circles[0][2];
					count++;
					circle(*image, Point(circles[0][0], circles[0][1]), circles[0][2], Scalar(255, 0, 0));
				}
				xAvg /= count;
				yAvg /= count;
				radiusAvg /= count;
				ball.addPosition(xAvg, yAvg, radiusAvg, time);	
				circle(*image, Point(xAvg, yAvg), radiusAvg, Scalar(0, 0, 255));
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