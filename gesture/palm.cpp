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

#include "uiUtils.h"
#include "cppUtils.h"

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

int fingers = 0;

/* Load hsv histogram from file
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


/* Isolate palm contour and idetify fingers
 *
 */
void isolatePalmContour(const Mat& frame){

	vector<vector<Point> > contours;
	findContours(frame, contours, RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE);

	Mat afterContours;
	afterContours = Mat::zeros(frame.size(), CV_8UC3);

	vector<Point> hull;
	vector<int> hullInt;
	vector<Vec4i> defects;
	Rect bRect;
	vector< vector<Point2f> > triangles;
	vector< vector<Point2f> > largeAngleTriangles;

	int cCount = 0;
	int fCount =0;

	for (int i=0; i< contours.size(); i++)
	{

		if (contourArea(contours[i]) > 30000)
		{
			circle(afterContours,uiUtils::leftPoint(contours[i]), cvRound(5), uiUtils::color(uiUtils::white), 5, LINE_4);

			uiUtils::displayText(afterContours,uiUtils::leftPoint(contours[i]), "area : " + cppUtils::toString(contourArea(contours[i])));

			fCount = 0;
			cCount++;

			drawContours(afterContours, contours, i, Scalar(0, 180, 0), 1, 8);

			// get the convex hull
			convexHull(Mat(contours[i]), hull,true, true);
			convexHull(Mat(contours[i]), hullInt,true, false);

			bRect = boundingRect(hull);

			rectangle(afterContours,bRect,Scalar(255,255,0), 1, LINE_4);

			Moments mo = moments(contours[i]);
			Point2f centroid = Point2f( mo.m10/mo.m00 , mo.m01/mo.m00 );

			circle(afterContours,centroid, cvRound(2), uiUtils::color(uiUtils::magenta), 1, LINE_4);

			//find the hull defect points and mark
			convexityDefects(contours[i], hullInt, defects);
			for (int j=0; j < defects.size() ; j++)
			{

				// if defect is too close to the convex hull, ignore
				if(defects[j][3] > 15000 && (centroid.y > contours[i][defects[j][0]].y))
				{
					//cout << "defects " << contours[i][defects[j][0]].y << " .. ceontroid" << centroid.y << endl;

					vector<Point2f> triangle;
					triangle.push_back(Point (contours[i][defects[j][0]]));
					triangle.push_back(Point (contours[i][defects[j][1]]));
					triangle.push_back(Point (contours[i][defects[j][2]]));

					circle(afterContours,Point (contours[i][defects[j][2]]), cvRound(2), uiUtils::color(uiUtils::red), 1, LINE_4);

					if (minEnclosingTriangle(triangle , triangle ) > 10000)
					{
						if (uiUtils::innerAngle(triangle[2] , triangle[0], triangle[1]) < 85)
						{

							triangles.push_back(triangle);

							circle(afterContours,triangle[0], cvRound(4), uiUtils::color(uiUtils::white), 1, LINE_4);
							circle(afterContours,triangle[2], cvRound(4), uiUtils::color(uiUtils::lime), 1, LINE_4);
							circle(afterContours,triangle[1], cvRound(5), uiUtils::color(uiUtils::yellow), 1, LINE_4);

							line(afterContours,Point (contours[i][defects[j][0]]),Point (contours[i][defects[j][1]]),uiUtils::color(uiUtils::cyan),1, LINE_4);
							line(afterContours,Point (contours[i][defects[j][0]]),Point (contours[i][defects[j][2]]),uiUtils::color(uiUtils::green),1, LINE_4);
							line(afterContours,Point (contours[i][defects[j][1]]),Point (contours[i][defects[j][2]]),uiUtils::color(uiUtils::green),1, LINE_4);

						}
						else
						{
							largeAngleTriangles.push_back(triangle);
						}
					}
				}

			}
		}
	}

	stringstream s_contours, f_count, sf_count, lt;
	s_contours << "contours " << cCount;
	if (triangles.size() == 0)
	{
		if (largeAngleTriangles.size() == 1)
		{
			fingers = triangles.size() + 1;
		}
		else
		{
			fingers = triangles.size();
		}
	}
	else
	{
		fingers = triangles.size() + 1;
	}

	f_count << "fingers " << fingers;
	sf_count << "single " << largeAngleTriangles.size();

	uiUtils::displayText(afterContours,Point (afterContours.cols - 100, 10), "contours : " + cppUtils::toString(cCount));
	uiUtils::displayText(afterContours,Point (afterContours.cols - 100, 30), "fingers : " + cppUtils::toString(fingers));
	uiUtils::displayText(afterContours,Point (afterContours.cols - 100, 50), "single : " + cppUtils::toString(largeAngleTriangles.size()));

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

	dilate( backproj, backproj, getStructuringElement(MORPH_ELLIPSE, Size(1, 1)) );
	//erode( backproj, backproj, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	/// Draw the backproj
	imshow( "After Back Projection", backproj );

	return backproj;
}



/** Capture histogram of a region and save
 * Majority of this is from opencv sample calcHist_demo.cpp followed by a save
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

/** Display a region of interest to capture palm histogram
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

			stringstream f_count;
			f_count << "->" << fingers;

			uiUtils::displayText(frame,Point (frame.cols - 100, 30), f_count.str(),3);
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
			camera = cppUtils::toInt(cam); // convert to integer
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
