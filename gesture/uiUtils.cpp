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


extern bool verbose;


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

enum lineOrientation {
	horizontal, vertical, angular
};

/*
 * structure to hold line attributes
 * direction and angle
 */
struct lineSruct {
	lineOrientation d;
	double angle;
};

/*
 * Angle between two points
 */
double angleInDegrees(cv::Point p1, cv::Point p2) {
	double rad = atan2(fabs(p1.y - p2.y), fabs(p1.x - p2.x)); // radians
	if (verbose) std::cout << "-> angleindegrees() - angle  " << p1 << " : " << p2 << " : " << (rad * 180) / M_PI << std::endl;
	return (rad * 180) / M_PI; // convert to degree
}

/*
 * Orientation (horizontal, vertical, angular) if a line is drawn between two points
 */
lineSruct lineSlope(cv::Point p1, cv::Point p2) {

	double angle = angleInDegrees(p1, p2);

	lineSruct l;
	l.angle = angle;

	if (angle < 20)
		l.d = horizontal;
	else if (angle > 85 && angle < 95)
		l.d = vertical;
	else
		l.d = angular;

	return l;
}

/*
 * Angle between lines connecting point 0 -> point 1 and point 0 -> point 2
 */
double uiUtils::innerAngle( Point pt1, Point pt2, Point pt0 )
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (acos((dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10)) * 180) / M_PI;
}

