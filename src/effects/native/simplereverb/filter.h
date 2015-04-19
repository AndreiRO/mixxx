/*
 * filter.h
 *
 *  Created on: Apr 25, 2015
 *      Author: andrei
 */

#ifndef SRC_EFFECTS_NATIVE_SIMPLEREVERB_FILTER_H_
#define SRC_EFFECTS_NATIVE_SIMPLEREVERB_FILTER_H_

#include "sampleutil.h"
#include "util.h"
#include "util/defs.h"
#include "util/types.h"

class ReverbFilter {
public:
	ReverbFilter(CSAMPLE* in, CSAMPLE* out);
	virtual ~ReverbFilter();

	void inc();
	void process(unsigned int sampleRate, CSAMPLE& left, CSAMPLE& right);
    unsigned int getSize();

protected:
	CSAMPLE* pIn;
	CSAMPLE* pOut;
	unsigned int head;
    unsigned int tail;
};


#endif /* SRC_EFFECTS_NATIVE_SIMPLEREVERB_FILTER_H_ */
