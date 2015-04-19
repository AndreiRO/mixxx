/*
 * filter.h
 *
 *  Created on: Apr 25, 2015
 *      Author: andrei
 */

#ifndef SRC_EFFECTS_NATIVE_SIMPLEREVERB_ALLPASSFILTER_H_
#define SRC_EFFECTS_NATIVE_SIMPLEREVERB_ALLPASSFILTER_H_

#include "filter.h"
#include "sampleutil.h"
#include "util.h"
#include "util/defs.h"
#include "util/types.h"
#include "engine/enginefilterbutterworth4.h"

class AllPassFilter : public ReverbFilter {
public:

	AllPassFilter(CSAMPLE* in = NULL, CSAMPLE* out = NULL, unsigned int delay = 100, float decay = 0.5);

	virtual ~AllPassFilter();

    void setDelay(unsigned int delay) { this->delay = delay; }
    void setDecay(float decay) { this->decay = decay; }
    void setFreq(float f);
    void applyFilter();
    void process(unsigned int sampleRate, CSAMPLE& left, CSAMPLE& right);

protected:
    unsigned int delay;
    float decay;
    EngineFilterButterworth4Low* lpf;
    float freq;
    unsigned int sampleRate;
};


#endif /* SRC_EFFECTS_NATIVE_SIMPLEREVERB_ALLPASSFILTER_H_ */

