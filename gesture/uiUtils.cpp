/*
 * uiUtils.cpp
 *
 *  Created on: Jul 4, 2015
 *      Author: harish
 */

#include "uiUtils.h"

#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;


uiUtils::uiUtils() {
	// TODO Auto-generated constructor stub

}

uiUtils::~uiUtils() {
	// TODO Auto-generated destructor stub
}

/// Least column
bool sortpointbyy(cv::Point a, cv::Point b) {
	return a.y < b.y;
}

/// Least row
bool sortpointbyx(cv::Point a, cv::Point b) {
	return a.x < b.x;
}

Point uiUtils::leftPoint(vector<Point> list)
{
	std::vector<cv::Point> sortedList;
	sortedList = list;

	sort(sortedList.begin(), sortedList.end(), sortpointbyx);
	return sortedList[0];
}

Point uiUtils::rightPoint(vector<Point> list)
{
	std::vector<cv::Point> sortedList;
	sortedList = list;

	sort(sortedList.begin(), sortedList.end(), sortpointbyy);
	return sortedList[0];
}

void uiUtils::displaytext(Mat& frame,Point point, string text, int size)
{
	putText(frame, text, point, FONT_HERSHEY_PLAIN  , size, Scalar(255,255,255),1,1);
}

Scalar uiUtils::color(uiUtils::colors c)
{
	switch(c)
	{
	case uiUtils::white :
		return Scalar (255,255,255);
	case uiUtils::red :
		return Scalar (0,0,255);
	case uiUtils::lime :
		return Scalar (0,255,0);
	case uiUtils::blue :
		return Scalar (255,0,0);
	case uiUtils::yellow :
		return Scalar (0,255,255);
	case uiUtils::cyan :
		return Scalar (255,255,0);
	case uiUtils::magenta :
		return Scalar (255,0,255);
	case uiUtils::silver :
		return Scalar (192,192,192);
	case uiUtils::gray :
		return Scalar (128,128,128);
	case uiUtils::maroon :
		return Scalar (0,0,128);
	case uiUtils::olive :
		return Scalar (0,128,128);
	case uiUtils::green :
		return Scalar (0,128,0);
	case uiUtils::purple :
		return Scalar (128,0,128);
	case uiUtils::teal :
		return Scalar (128,128,0);
	case uiUtils::navy :
		return Scalar (128,0,0);
	default:
		return Scalar(0,0,0);
	}
}
