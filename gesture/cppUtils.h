/*
 * cppUtils.h
 *
 *  Created on: Jul 4, 2015
 *      Author: harish
 */

#ifndef CPPUTILS_H_
#define CPPUTILS_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

class cppUtils {
public:
	cppUtils();
	virtual ~cppUtils();

	static string toString(double in);
	static string toString(int in);
	static string toString(size_t in);

	static int toInt(char* in);
};

#endif /* CPPUTILS_H_ */
