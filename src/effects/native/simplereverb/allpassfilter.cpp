#include "effects/native/simplereverb/allpassfilter.h"

AllPassFilter::AllPassFilter(
                        CSAMPLE* in,
                        CSAMPLE* out,
                        unsigned int delay,
                        float decay) : ReverbFilter(in, out) {
    this->delay = delay;
    this->decay = decay;
}

AllPassFilter::~AllPassFilter() {
}

void AllPassFilter::setFreq(float f) {
    freq = f;
}

void AllPassFilter::applyFilter() {
}

void AllPassFilter::process(unsigned int sampleRate, CSAMPLE& left, CSAMPLE& right) {
    this->sampleRate = sampleRate;
    unsigned int skip = (int)(sampleRate * delay / 1000.0);

    if (skip < getSize()) {
        left += pIn[tail] * decay;
        left -= pOut[tail] * decay;
        tail = (tail + 1) % MAX_BUFFER_LEN;
        right += pIn[tail] * decay;
        right -= pOut[tail] * decay;
        tail = (tail + 1) % MAX_BUFFER_LEN;
    }
}
