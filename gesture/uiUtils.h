/*
 * uiUtils.h
 *
 *  Created on: Jul 4, 2015
 *      Author: harish
 */

#ifndef UIUTILS_H_
#define UIUTILS_H_

#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
using namespace cv;

class uiUtils {

public:
	uiUtils();
	virtual ~uiUtils();

	static Point leftPoint(vector<Point> list);
	static Point rightPoint(vector<Point> list);
	static void displaytext(Mat& frame,Point point, string text);
};

#endif /* UIUTILS_H_ */
