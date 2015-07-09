/*
 * cppUtils.cpp
 *
 *  Created on: Jul 4, 2015
 *      Author: harish
 */

#include "cppUtils.h"


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "uiUtils.h"

using namespace std;


cppUtils::cppUtils() {
	// TODO Auto-generated constructor stub

}

cppUtils::~cppUtils() {
	// TODO Auto-generated destructor stub
}

string cppUtils::toString(double in)
{
	std::ostringstream strs;
	strs << in;
	return strs.str();
}

string cppUtils::toString(int in)
{
	std::ostringstream strs;
	strs << in;
	return strs.str();
}

string cppUtils::toString(size_t in)
{
	std::ostringstream strs;
	strs << in;
	return strs.str();
}

