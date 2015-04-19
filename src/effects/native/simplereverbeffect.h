/*
 * SimpleReverbEffect.h
 *
 *  Created on: Apr 18, 2015
 *      Author: andrei
 *      Email: andysstef@yahoo.com
 */


#ifndef SRC_EFFECTS_NATIVE_SIMPLEREVERBEFFECT_H_
#define SRC_EFFECTS_NATIVE_SIMPLEREVERBEFFECT_H_

#include "util.h"
#include "util/defs.h"
#include "util/types.h"
#include "engine/effects/engineeffect.h"
#include "engine/effects/engineeffectparameter.h"
#include "effects/effectprocessor.h"
#include "sampleutil.h"
#include "engine/enginefilterbutterworth4.h"
#include "effects/native/simplereverb/combfilter.h"
#include "effects/native/simplereverb/allpassfilter.h"

#define EARLY_TAPS 5
#define LATE_REFLECTIONS 1

struct SimpleReverbGroupState {

    SimpleReverbGroupState() {
        pOut = SampleUtil::alloc(MAX_BUFFER_LEN);
        pIn = SampleUtil::alloc(MAX_BUFFER_LEN);
        SampleUtil::applyGain(pIn, 0, MAX_BUFFER_LEN);
        SampleUtil::applyGain(pOut, 0, MAX_BUFFER_LEN);

        for (int i = 0; i < EARLY_TAPS; ++ i) {
            m_cfs[i] = CombFilter(
                                pIn,
                                pOut,
                                early_taps_delay[i],
                                early_taps_decay[i]
                    );
        }

        for (int i = 0; i < LATE_REFLECTIONS; ++ i) {
            m_cfs[i + EARLY_TAPS] = CombFilter(
                                                pIn,
                                                pOut,
                                                -1,
                                                -1
                                    );
        }


        m_apf = AllPassFilter(pIn, pOut, 700, 0.5);

        index = 0;
    }

    ~SimpleReverbGroupState() {
        SampleUtil::free(pIn);
        SampleUtil::free(pOut);
    }

    CombFilter m_cfs[EARLY_TAPS + LATE_REFLECTIONS];
    AllPassFilter m_apf;
    CSAMPLE* pIn;
    CSAMPLE* pOut;
    unsigned int index;
    static int early_taps_delay[EARLY_TAPS];
    static int late_reflections_delay[EARLY_TAPS];
    static float early_taps_decay[EARLY_TAPS];
    static float late_reflections_decay[EARLY_TAPS];
};

class SimpleReverbEffect : public PerChannelEffectProcessor<SimpleReverbGroupState> {
  public:
    SimpleReverbEffect(EngineEffect* pEffect, const EffectManifest& manifest);
    virtual ~SimpleReverbEffect();

    static QString getId();
    static EffectManifest getManifest();

    // See effectprocessor.h
    void processChannel(const ChannelHandle& handle,
                        SimpleReverbGroupState* pState,
                        const CSAMPLE* pInput, CSAMPLE* pOutput,
                        const unsigned int numSamples,
                        const unsigned int sampleRate,
                        const EffectProcessor::EnableState enableState,
                        const GroupFeatureState& groupFeatures);

  private:

    QString debugString() const {
        return getId();
    }

    EngineEffectParameter* m_pDelayParameter;
    EngineEffectParameter* m_pDecayParameter;
    EngineEffectParameter* m_pBandwithParameter;


    DISALLOW_COPY_AND_ASSIGN(SimpleReverbEffect);
};



#endif /* SRC_EFFECTS_NATIVE_SIMPLEREVERBEFFECT_H_ */
