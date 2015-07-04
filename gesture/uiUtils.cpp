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

void uiUtils::displaytext(Mat& frame,Point point, string text)
{
	putText(frame, text, point, FONT_HERSHEY_PLAIN  , 1, Scalar(255,255,255),1,1);
}
