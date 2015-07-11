/*
 * takePics.cpp
 *
 *  Created on: Jul 11, 2015
 *      Author: harish
 */

#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video.hpp>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "uiUtils.h"
#include "cppUtils.h"

using namespace std;
using namespace cv;

/* command line options */
bool verbose = false;
int camera = -1;
int snaps = 10;

const string camfeed = "Camera";

int snapcount = 1;



/* --------------------------------------------------------------------------------------------- */

void printusage()
{
	cout << "usage : gesture [-v] [-c <camera number>] " << endl;
	cout << "\t-v	Display verbose output. Used to output debug values to console" << endl;
	cout << "\t-c	Specify camera 0 or 1. If there is only one cam it is usually 0. Any additional USB cams will have higher numbers" << endl;
	cout << "\t-n	Number or snapts to take. Default is 10 is this parameter is not specified" << endl;
	cout << "\t-t	Train hsv palm histogram" << endl;
}


int captureVideo(int& cam)
{
	VideoCapture cap(cam);
	Mat frame;
	double curTick = (double)getCPUTickCount();
	string fileName;

	if(!cap.isOpened())
	{
		cout << "problem !" << endl;
		return -1;
	}

	if (verbose) cout << "snaps : " << snaps << endl;

	namedWindow(camfeed,1);
	for(;;)
	{
		char c = (char)waitKey(10);
		if( c == 27 ){
			break;
		}
		cap >> frame;

		if (((double)getTickCount() - curTick) > (getTickFrequency() * 2))
		{
			curTick = (double)getTickCount();
			fileName = "data/test" + cppUtils::toString(time(0)) + ".png";
			if (verbose) cout << "snap : " << fileName << endl;
			imwrite(fileName,frame);

			snapcount++;
			if (snapcount > snaps) break;
		}

		imshow(camfeed, frame);
	}
	return 0;
}

int main(int argc, char **argv) {

	int option;
	char *cam;
	char *snapnum;

	if (argc ==1 )
	{
		printusage();
		return 0;
	}

	cout << argv[0] << endl;

	while ((option = getopt (argc, argv, "c:vn:")) != -1)
	{
		switch (option)
		{
		case 'c':
			cam = optarg;
			camera = cppUtils::toInt(cam); // convert to integer
			cout << "capture from camera " << camera << endl;
			break;
		case 'v':
			verbose = true;
			break;
		case 'n':
			snapnum = optarg;
			snaps = cppUtils::toInt(snapnum);
			break;
		default:
			printusage();
			return 1;
		}
	}

	return captureVideo(camera);
}
