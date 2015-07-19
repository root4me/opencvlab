/*
 * haarMark.cpp
 *
 *  Created on: Jul 17, 2015
 *      Author: harish
 *
 *  mark object and generate positive list file for training
 */



#include <opencv2/highgui/highgui.hpp>


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
char* outFileName = NULL;
char* inputFolder = NULL;
char* negativeFolder = NULL;
char* positiveFolder = NULL;

const string winName = "Review Image";

Mat frame;
string imageName;

bool selectObject = false;
Point origin;
Rect selection;


string getFileName(string filePath)
{
	return filePath.substr(filePath.find_last_of("/") + 1, (filePath.size() - filePath.find_last_of("/")));
}

/*
 * inputFile : file path and name
 * outputFolder : folder to which the file need to be moved
 * return the file path and name of output file
 */
string moveFile(string inputFile , string outputFolder)
{
	string fileName = getFileName(inputFile);
	string outputFile = outputFolder + "/" + fileName;

	if (verbose) cout << ".. moving " << inputFile << " -> " << outputFile << endl;
	rename(inputFile.c_str() , outputFile.c_str());

	return outputFile;
}


void displayControls()
{
	uiUtils::displayText(frame,Point (10,10),"Select ROI : mouse click and drag",1);
	uiUtils::displayText(frame,Point (10,30),"Positive sample : 'p' key (only after selecting an ROI)",1);
	uiUtils::displayText(frame,Point (10,50),"negative sample : 'n' key",1);
	uiUtils::displayText(frame,Point (10,70),"Skip to next image : -> arrow key",1);
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

		if (verbose) cout << "select object : " << selectObject << endl;
		if (verbose) cout << "width : " << selection.width << endl;
		if (verbose) cout << "height : " << selection.height << endl;

		frame = imread(imageName,1);
		displayControls();
		rectangle(frame,selection,uiUtils::color(uiUtils::lime),2);
		imshow(winName, frame);

		selectObject = false;
		break;
	}

}

bool saveSelection()
{
	if (selection.width > 0 && selection.height > 0)
	{
		cout << ".. saving region of interest from  "  << imageName << " 1 " << selection.x << ' ' << selection.y << ' ' << selection.width << ' ' << selection.height << endl;

		ofstream outFile (outFileName, ios::out | ios::app );
		if (outFile.is_open())
		{
			outFile << moveFile(imageName, (string)positiveFolder) << " 1 " << selection.x << ' ' << selection.y << ' ' << selection.width << ' ' << selection.height << endl;
			outFile.close();
		}
		else
		{
			cout << ".. can not open file !" << endl;
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

int processImages()
{
	vector<String> images;
	string fileName , markedFile;

	namedWindow( winName, WINDOW_AUTOSIZE );
	setMouseCallback( winName, onMouse, 0 );

	glob((string)inputFolder + "/*.png",images, false);

	for (size_t i =0; i < images.size() ; i++)
	{
		frame = imread(images[i],1);
		cout << ".. processing " << i + 1 << " / " << images.size() << endl;
		displayControls();
		imshow(winName, frame);
		imageName = images[i];

		int c = waitKey(0);
		if( c == 27 ){
			break;
		}

		switch (c)
		{
		case 112 : // 'p'
			if (verbose) cout << ".. positive" << endl;
			// mark positive ROI and save
			if (!(saveSelection()))
			{
				cout << ".. no ROI selected to save" << endl;
				i = i -1;
			}
			break;
		case 110 : // 'n'
			if (verbose) cout << ".. negative" << endl;
			// move image to negative image folder
			cout << ".. moving " << images[i] << " to negatives folder" << endl;
			moveFile(images[i],negativeFolder);
			break;
		case 65363 : // right arrow
			if (verbose) cout << ".. next" << endl;
			// skip to next image (do nothing for now)
			break;
			//case 65361 : // left arrow
			//	if (verbose) cout << ".. previous" << endl;
			//	if (i == 0) { i = -1; } else {
			//		i = i - 2;
			//	}
			//	if (verbose) cout << i << endl;
			//	break;
		default : // stay where you are. i need to be decremented
			i = i -1;
			if (verbose) cout << ".. pressed " << c << endl;
			break;
		}
	}

	return 0;
}

void printusage()
{
	cout << "usage : haarmark [-v] {-i <input folder>} " << endl;
	cout << "\t-v	Display verbose output. Used to output debug values to console" << endl;
	cout << "\t-i	input file folder (this is where the images to be processed must reside)" << endl;
	cout << "\t-p	folder were positive files are stored" << endl;
	cout << "\t-n	folder were negative files are stored" << endl;
	cout << "\t-o	output info file name (this file will contain the positive file name with the coordinates of the selected region on interest)" << endl;
	cout << "example : haarmark -i data/sample -o data/positive.info  -n data/negative" << endl;
}

int main(int argc, char **argv) {

	int option;

	if (argc ==1 )
	{
		printusage();
		return 0;
	}

	while ((option = getopt (argc, argv, "vi:o:n:p:")) != -1)
	{
		switch (option)
		{
		case 'v':
			verbose = true;
			break;
		case 'i':
			inputFolder = optarg;
			break;
		case 'o':
			outFileName = optarg;
			break;
		case 'p':
			positiveFolder = optarg;
			break;
		case 'n':
			negativeFolder = optarg;
			break;
		default:
			printusage();
			return 1;
		}
	}

	if (outFileName == NULL || inputFolder == NULL || negativeFolder == NULL || positiveFolder == NULL)
	{
		printusage();
		return 1;
	}

	return processImages();
}
