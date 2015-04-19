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

void AllPassFilter::process(unsigned int sampleRate, CSAMPLE& left, CSAMPLE& right) {
    unsigned int skip = (int)(sampleRate * delay / 1000.0);

    qDebug() << getSize() << '\n';
    if (skip < getSize()) {
        qDebug() << "Here\n";
        left += pIn[tail] * decay;
        left -= pOut[tail] * decay;
        tail = (tail + 1) % MAX_BUFFER_LEN;
        right += pIn[tail] * decay;
        right -= pOut[tail] * decay;
        tail = (tail + 1) % MAX_BUFFER_LEN;
    }
}
