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

	enum colors{
		white,red,lime,blue,yellow,cyan,magenta,silver,gray,maroon,olive,green,purple,teal,navy
	};

	static Point leftPoint(vector<Point> list);
	static Point rightPoint(vector<Point> list);
	static void displaytext(Mat& frame,Point point, string text, int size = 1);
	static Scalar color(colors c);

};

#endif /* UIUTILS_H_ */
