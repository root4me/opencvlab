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

using namespace std;
using namespace cv;

/* command line options */
bool verbose = false;
int camera = -1;
bool train = false; // Train histogram

/* capture histogram */
bool capture = false;
Mat savedHist; // saved histogram
int channels[] = { 0, 1 };
float hRange[] = { 0, 180 };
float sRange[] = { 0, 256 };
const float* ranges[] = { hRange, sRange };



/** @brief load hsv histogram from file
 *
 */
Mat loadHistogram()
{

	Mat hist;
	FileStorage storage("palmHistogram.xml", FileStorage::READ);
	storage["hist"] >> hist;
	storage.release();

	if (verbose)
	{
		cout << hist << endl;
	}

	return hist;
}

void displayText(Mat& im, string text, int row)
{

	putText(im, text, Point(im.cols - 100,row * 20), FONT_HERSHEY_PLAIN , 1, Scalar(255,255,255),1,1);
}

/** @brief Isolates contours of the palm
 *
 */
void isolatePalmContour(const Mat& frame){

	vector<vector<Point> > contours;
	findContours(frame, contours, RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE);

	Mat afterContours;
	afterContours = Mat::zeros(frame.size(), CV_8UC3);

	for (int i=0; i< contours.size(); i++)
	{
		if (contourArea(contours[i]) > 1000)
		{
			drawContours(afterContours, contours, i, Scalar(0, 180, 0), 1, 8);
		}
	}

	stringstream s_contours;
	s_contours << "contours " << contours.size();

	displayText(afterContours,s_contours.str(), 1);
	displayText(afterContours,"fingers  0", 2);

	imshow("Isolate Palm Contours", afterContours);
}

Mat getBackProjection(Mat& frame){

	Mat src; Mat hsv; Mat hue;

	if (savedHist.rows == 0) {
		savedHist = loadHistogram();
	}

	cvtColor( frame, hsv, COLOR_BGR2HSV );

	/// Get Backprojection
	Mat backproj;
	calcBackProject( &hsv, 1, channels, savedHist, backproj, ranges, 1, true );

	//dilate( backproj, backproj, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	//erode( backproj, backproj, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	/// Draw the backproj
	imshow( "After Back Projection", backproj );

	return backproj;
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
	int hbins = 50, sbins = 60;
	int histSize[] = {hbins, sbins};

	Mat hist;

	calcHist( &hsv, 1, channels, Mat(), // do not use mask
			hist, 2, histSize, ranges,
			true, // the histogram is uniform
			false );

	normalize( hist, hist, 0, 255, NORM_MINMAX, -1, Mat() );

	if (verbose) cout << hist << endl;

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
	roiRect.y = frame.rows/3 - 50;
	roiRect.height = frame.rows/3 + 100;
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
		if( c == 27 ){
			break;
		}
		cap >> frame;

		switch (c)
		{
		case ' ':
			capture = true;
			break;
		default :
			;
		}

		if (train) {
			drawTrainROI(frame);
		}else
		{
			isolatePalmContour(getBackProjection(frame));
		}

		imshow("Camera", frame);
	}
	return 0;
}

void printusage()
{
	cout << "usage : gesture [-v] [-c <camera number>] " << endl;
	cout << "\t-v	Display verbose output. Used to output debug values to console" << endl;
	cout << "\t-c	Specify camera 0 or 1. If there is only one cam it is usually 0. Any additional USB cams will have higher numbers" << endl;
	cout << "\t-t	Train hsv palm histogram" << endl;
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
