#pragma once
#include <opencv2\opencv.hpp>
#include "Ball.h"

class Track {
private:
	//maximum number of objects to prevent too much noise
	const int MAX_OBJECTS = 50;
	//if object smaller than minimum or larger than maximum, do not record
	const int MIN_OBJECT_AREA = 10 * 10;
	const int MAX_OBJECT_AREA = 400 * 400;
	//Morphological operations to reduce noise in threshold image
	void morphOps();
	//pointers to camera image and threshold image
	cv::Mat* image;
	cv::Mat* thresh;
public:
	Track(cv::Mat* image, cv::Mat* thresh);
	~Track();
	//Detect ball and store position information
	void track(Ball& ball);
};