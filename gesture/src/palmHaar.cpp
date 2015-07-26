/*
 * palmHaar.cpp
 *
 *  Created on: Jul 13, 2015
 *      Author: harish
 *
 *  detect palm using haar cascade
 */

#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video.hpp>
#include "opencv2/objdetect/objdetect.hpp"

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
char* inputImage;


int findPalm(char *image, char *cascadexml)
{
	cout << image << endl;
	CascadeClassifier palmCascade;

	palmCascade.load(cascadexml);

	vector<Rect> palms;
	Mat frame_gray;

	Mat frame = imread(image,1);

	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	palmCascade.detectMultiScale( frame_gray, palms, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

	for( size_t i = 0; i < palms.size(); i++ )
	{
		rectangle(frame,palms[i],uiUtils::color(uiUtils::green),2);
	}

	imshow("input", frame);

	waitKey(0);

	return 0;
}

int captureVideo(int& cam, char *cascadexml)
{
	VideoCapture cap(cam);
	Mat frame;
	CascadeClassifier palmCascade;

	palmCascade.load(cascadexml);

	vector<Rect> palms;
	Mat frame_gray;

	if(!cap.isOpened())
	{
		cout << "problem !" << endl;
		return -1;
	}

	namedWindow("Camera",1);
	for(;;)
	{
		char c = (char)waitKey(10);
		if( c == 27 ){
			break;
		}
		cap >> frame;

		cvtColor( frame, frame_gray, CV_BGR2GRAY );
		equalizeHist( frame_gray, frame_gray );

		palmCascade.detectMultiScale( frame_gray, palms, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

		for( size_t i = 0; i < palms.size(); i++ )
		{
			rectangle(frame,palms[i],uiUtils::color(uiUtils::green),2);
		}

		imshow("Camera", frame);
	}
	return 0;
}

void printusage()
{
	cout << "usage : gesture [-v] [-c <camera number>] " << endl;
	cout << "\t-v	Display verbose output. Used to output debug values to console" << endl;
	//cout << "\t-c	Specify camera 0 or 1. If there is only one cam it is usually 0. Any additional USB cams will have higher numbers" << endl;
	cout << "\t-h	haar cascade classifier xml" << endl;
	cout << "\t-i	Image file" << endl;
}

int main(int argc, char **argv) {

	int option;
	char *image;
	char *cascade;

	if (argc ==1 )
	{
		printusage();
		return 0;
	}

	cout << argv[0] << endl;

	while ((option = getopt (argc, argv, "h:vi:")) != -1)
	{
		switch (option)
		{
		case 'h':
			cascade = optarg;
			break;
		case 'v':
			verbose = true;
			break;
		case 'i':
			image = optarg;
			break;
		default:
			printusage();
			return 1;
		}
	}

	//return findPalm(image, cascade);
	int cam = 1;

	return captureVideo(cam,cascade);

}
