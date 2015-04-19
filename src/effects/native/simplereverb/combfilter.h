/*
 * filter.h
 *
 *  Created on: Apr 25, 2015
 *      Author: andrei
 */

#ifndef SRC_EFFECTS_NATIVE_SIMPLEREVERB_COMBFILTER_H_
#define SRC_EFFECTS_NATIVE_SIMPLEREVERB_COMBFILTER_H_

#include "filter.h"
#include "sampleutil.h"
#include "util.h"
#include "util/defs.h"
#include "util/types.h"


class CombFilter : public ReverbFilter {
public:

	CombFilter(CSAMPLE* in = NULL, CSAMPLE* out = NULL, unsigned int delay = 100, float decay = 0.5);

	virtual ~CombFilter();

    void setDelay(unsigned int delay) { this->delay = delay; }
    void setDecay(float decay) { this->decay = decay; }

    void process(unsigned int sampleRate, CSAMPLE& left, CSAMPLE& right);
protected:
    unsigned int delay;
    float decay;
};


#endif /* SRC_EFFECTS_NATIVE_SIMPLEREVERB_COMBFILTER_H_ */
