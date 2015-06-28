/*
 * gesture.cpp
 *
 *  Created on: Jun 21, 2015
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

/* command line options */
bool verbose = false;
int camera = -1;
bool train = false; // Train histogram

/* capture histogram */
bool capture = false;

using namespace std;
using namespace cv;

/** @brief load hsv histogram from file
 *
 */
MatND loadHistogram()
{

	MatND hist;
	FileStorage storage("palmHistogram.xml", FileStorage::READ);
	storage["hist"] >> hist;
	storage.release();

	if (verbose)
	{
		cout << hist << endl;
	}

	return hist;

}

/** @brief Capture histogram of a region and save
 * Majority of this is from opencv sample calcHist_demo.cpp
 */
void captureHistogram(Mat& frame)
{
	Mat hsv;

	cvtColor(frame, hsv, CV_BGR2HSV);

	// Quantize the hue to 30 levels
	// and the saturation to 32 levels
	int hbins = 30, sbins = 32;
	int histSize[] = {hbins, sbins};
	// hue varies from 0 to 179, see cvtColor
	float hranges[] = { 0, 180 };
	// saturation varies from 0 (black-gray-white) to 255 (pure spectrum color)
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	MatND hist;
	// we compute the histogram from the 0-th and 1-st channels
	int channels[] = {0, 1};

	calcHist( &hsv, 1, channels, Mat(), // do not use mask
			hist, 2, histSize, ranges,
			true, // the histogram is uniform
			false );


	cout << hist << endl;

	FileStorage fs("palmHistogram.xml", FileStorage::WRITE);
	fs << "hist" << hist;
	fs.release();
}

/** @brief Display a region of interest to capture palm histogram
 */
void drawTrainROI(Mat& frame)
{
	Rect roiRect;
	roiRect.x = frame.cols/3;
	roiRect.y = frame.rows/3;
	roiRect.height = frame.rows/3;
	roiRect.width = frame.cols/3;

	Mat roi(frame,roiRect);
	if (!capture)
	{
		bitwise_not(roi, roi);
	}
	else
	{
		if (verbose) cout << "Capturing histogram" << endl;
		capture = !capture;

		captureHistogram(roi);
	}
}

int captureVideo(int& cam)
{
	VideoCapture cap(cam);
	Mat frame;

	if(!cap.isOpened())
	{
		cout << "problem !" << endl;
		return -1;
	}

	namedWindow("Camera",1);
	for(;;)
	{
		char c = (char)waitKey(10);
		if( c == 27 ) break;
		cap >> frame;

		switch (c)
		{
		case ' ':
			capture = true;
			break;
		default :
			;
		}

		if (train) drawTrainROI(frame);

		imshow("Camera", frame);
	}
	return 0;
}

void printusage()
{
	cout << "usage : gesture [-v] [-c <camera number>] " << endl;
	cout << "\t-v	Display verbose output. Used to output debug values to console" << endl;
	cout << "\t-c	Specify camera 0 or 1. If there is only one cam it is usually 0. Any additional USB cams will have higher numbers" << endl;
}

int main(int argc, char **argv) {

	int option;
	char *cam;

	if (argc ==1 ) printusage();

	cout << argv[0] << endl;

	while ((option = getopt (argc, argv, "c:vt")) != -1)
	{
		switch (option)
		{
		case 'c':
			cam = optarg;
			camera = atoi(cam); // convert to integer
			cout << "capture from camera " << camera << endl;
			break;
		case 'v':
			verbose = true;
			break;
		case 't':
			train =  true;
			break;
		default:
			printusage();
			return 1;
		}
	}

	return captureVideo(camera);
}
