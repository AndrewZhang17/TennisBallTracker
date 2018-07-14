#pragma once
#include <opencv2\opencv.hpp>
#include "Ball.h"

class Tracker {
private:
	//maximum number of objects to prevent too much noise
	const int MAX_OBJECTS = 50;
	//if object smaller than minimum or larger than maximum, do not record
	const int MIN_OBJECT_AREA = 10 * 10;
	const int MAX_OBJECT_AREA = 400 * 400;
	//pointers to camera image and threshold image
	cv::Mat* image;
	cv::Mat* thresh;
public:
	Tracker(cv::Mat& image, cv::Mat& thresh);
	~Tracker();
	//Morphological operations to reduce noise in threshold image
	void morphOps();
	//Detect ball and store position information
	void track(Ball& ball);
};