/*
 * haarMark.cpp
 *
 *  Created on: Jul 17, 2015
 *      Author: harish
 *
 *  mark object and generate positive list file for training
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
#include <fstream>

#include "uiUtils.h"
#include "cppUtils.h"

using namespace std;
using namespace cv;

/* command line options */
bool verbose = false;
//char* inputImage = NULL;
char* outFileName = NULL;

const string winName = "Input Image";

Mat frame;
string imageName;

bool selectObject = false;
Point origin;
Rect selection;

void insertActionText()
{
	uiUtils::displayText(frame,Point (10,10),"Mouse to select .. space bar to save selection to file",1);
}

static void onMouse( int event, int x, int y, int, void* )
{

	if( selectObject )
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);
		selection.height = std::abs(y - origin.y);

		selection &= Rect(0, 0, frame.cols, frame.rows);
	}

	switch( event )
	{
	case EVENT_LBUTTONDOWN:
		origin = Point(x,y);
		selection = Rect(x,y,0,0);
		selectObject = true;
		break;
	case EVENT_LBUTTONUP:

		cout << "select object : " << selectObject << endl;
		cout << "width : " << selection.width << endl;
		cout << "height : " << selection.height << endl;

		//if(selectObject && selection.width > 0 && selection.height > 0 )
		//{
		frame = imread(imageName,1);
		uiUtils::displayText(frame,Point (10,10),"..space bar to save selection to file ...any key to skip to next image",1);
		uiUtils::displayText(frame,Point (10,30),"..mouse to reselect ...esc to quit",1);
		rectangle(frame,selection,uiUtils::color(uiUtils::white),1);
		imshow("Input Image", frame);
		//}

		selectObject = false;
		break;
	}

}

void saveSelection()
{
	if (selection.width > 0 && selection.height > 0)
	{
		cout << ".. saving the region of interest :  "  << imageName << " 1 " << selection.x << ' ' << selection.y << ' ' << selection.width << ' ' << selection.height << endl;

		ofstream outFile (outFileName, ios::out | ios::app );
		if (outFile.is_open())
		{
			outFile << imageName << " 1 " << selection.x << ' ' << selection.y << ' ' << selection.width << ' ' << selection.height << endl;
			outFile.close();
		}
		else cout << ".. can not open file !" << endl;
	}
}

int displayImages(char *folder)
{
	cout << folder << endl;

	vector<String> images;

	namedWindow( winName, WINDOW_AUTOSIZE );
	setMouseCallback( winName, onMouse, 0 );


	glob((string)folder + "/*.png",images, false);

	for (size_t i =0; i < images.size() ; i++)
	{
		frame = imread(images[i],1);
		insertActionText();
		imshow(winName, frame);
		imageName = images[i];

		char c = (char)waitKey(0);
		if( c == 27 ){
			break;
		}
		else if (c == ' ')
		{
			saveSelection();
		}
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
	char *imageFolder;

	if (argc ==1 )
	{
		printusage();
		return 0;
	}

	cout << argv[0] << endl;

	while ((option = getopt (argc, argv, "vi:o:")) != -1)
	{
		switch (option)
		{
		case 'v':
			verbose = true;
			break;
		case 'i':
			imageFolder = optarg;
			break;
		case 'o':
			outFileName = optarg;
			break;
		default:
			printusage();
			return 1;
		}
	}

	if (outFileName == NULL)
	{
		outFileName = (char *)"temp.txt";
	}

	return displayImages(imageFolder);
}
