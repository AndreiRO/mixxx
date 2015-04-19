/*
 * filter.cpp
 *
 *  Created on: Apr 25, 2015
 *      Author: andrei
 */


#include "effects/native/simplereverb/filter.h"

ReverbFilter::ReverbFilter(CSAMPLE* in, CSAMPLE* out) {
	pIn = in;
	pOut = out;
	head = tail = 0;
}

ReverbFilter::~ReverbFilter() {

}

void ReverbFilter::inc() {
	head = (head + 2) % MAX_BUFFER_LEN;
}

unsigned int ReverbFilter::getSize() {
    return (head > tail) ? head - tail : MAX_BUFFER_LEN - tail + head;
}
