#include "effects/native/simplereverb/combfilter.h"

CombFilter::CombFilter(
                        CSAMPLE* in,
                        CSAMPLE* out,
                        unsigned int delay,
                        float decay) : ReverbFilter(in, out) {
    this->delay = delay;
    this->decay = decay;
}

CombFilter::~CombFilter() {

}

void CombFilter::process(unsigned int sampleRate, CSAMPLE& left, CSAMPLE& right) {
    unsigned int skip = (int)(sampleRate * delay / 1000.0);

    if (skip < getSize()) {
        left += pIn[tail] * decay;
        tail = (tail + 1) % MAX_BUFFER_LEN;
        right += pIn[tail] * decay;
        tail = (tail + 1) % MAX_BUFFER_LEN;
    }
}
