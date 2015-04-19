/*
 * SimpleReverbEffect.cpp
 *
 *  Created on: Apr 18, 2015
 *      Author: andrei
 */
#include "effects/native/simplereverbeffect.h"

#include<cmath>
#include "sampleutil.h"
#include<iostream>
using namespace std;

//i31    37    41    43    47    53
int SimpleReverbGroupState::early_taps_delay[] = {
        31,
        37,
        41,
        47,
        53
};

float SimpleReverbGroupState::early_taps_decay[] = {
        0.9,
        0.8,
        0.7,
        0.6,
        0.5
};



// static
QString SimpleReverbEffect::getId() {
    return "org.mixxx.effects.simplereverb";
}

// static
EffectManifest SimpleReverbEffect::getManifest() {
    EffectManifest manifest;
    manifest.setId(getId());
    manifest.setName(QObject::tr("SimpleReverb"));
    manifest.setAuthor("AndreiRO");
    manifest.setVersion("0.33");
    manifest.setDescription(QObject::tr("SimpleReverb."));

    EffectManifestParameter* time = manifest.addParameter();
    time->setId("decay_amount");
    time->setName(QObject::tr("Decay"));
    time->setDescription(
            QObject::tr("Decay for sound"));
    time->setControlHint(EffectManifestParameter::CONTROL_KNOB_LINEAR);
    time->setSemanticHint(EffectManifestParameter::SEMANTIC_UNKNOWN);
    time->setUnitsHint(EffectManifestParameter::UNITS_UNKNOWN);
    time->setMinimum(0.0);
    time->setDefault(0.2);
    time->setMaximum(0.5);

    time = manifest.addParameter();
    time->setId("delay_time");
    time->setName(QObject::tr("Delay"));
    time->setDescription(QObject::tr("Delay time (seconds)"));
    time->setControlHint(EffectManifestParameter::CONTROL_KNOB_LINEAR);
    time->setSemanticHint(EffectManifestParameter::SEMANTIC_UNKNOWN);
    time->setUnitsHint(EffectManifestParameter::UNITS_TIME);
    time->setDefaultLinkType(EffectManifestParameter::LINK_LINKED);
    time->setMinimum(0.01);
    time->setDefault(0.2);
    time->setMaximum(0.3);

    time = manifest.addParameter();
        time->setId("bandwidth");
        time->setName(QObject::tr("Reverberation"));
        time->setDescription(QObject::tr("Delay time (seconds)"));
        time->setControlHint(EffectManifestParameter::CONTROL_KNOB_LINEAR);
        time->setSemanticHint(EffectManifestParameter::SEMANTIC_UNKNOWN);
        time->setUnitsHint(EffectManifestParameter::UNITS_UNKNOWN);
        time->setDefaultLinkType(EffectManifestParameter::LINK_LINKED);
        time->setMinimum(0.001);
        time->setDefault(0.1);
        time->setMaximum(0.2);


    return manifest;
}

SimpleReverbEffect::SimpleReverbEffect(EngineEffect* pEffect, const EffectManifest& manifest)
        : m_pDelayParameter(pEffect->getParameterById("delay_time")),
          m_pDecayParameter(pEffect->getParameterById("decay_amount")),
        m_pBandwithParameter(pEffect->getParameterById("bandwidth")) {
    Q_UNUSED(manifest);
}

SimpleReverbEffect::~SimpleReverbEffect() {

}

void SimpleReverbEffect::processChannel(const ChannelHandle& handle, SimpleReverbGroupState* pGroupState,
                                const CSAMPLE* pInput,
                                CSAMPLE* pOutput, const unsigned int numSamples,
                                const unsigned int sampleRate,
                                const EffectProcessor::EnableState enableState,
                                const GroupFeatureState& groupFeatures) {
    Q_UNUSED(handle);
    Q_UNUSED(pGroupState);
    Q_UNUSED(enableState);
    Q_UNUSED(groupFeatures);

    SimpleReverbGroupState& gs = *pGroupState;
    int delay = (int)(m_pDelayParameter->value() * 1000);
    float decay = 1 - m_pDecayParameter->value();
    float freq = m_pBandwithParameter->value();
    gs.m_apf.setDecay(decay);
    gs.m_apf.setDelay(delay);
    gs.m_apf.setFreq(freq);

    unsigned int old_index = gs.index; /* gs.tail gets modified in the loop */
    unsigned int count = 17;
    for (int j = 0; j < LATE_REFLECTIONS; ++ j) {
        gs.m_cfs[EARLY_TAPS + j].setDelay(delay + count);
        gs.m_cfs[EARLY_TAPS + j].setDecay(decay);
        count += 17;
    }
    for (unsigned int i = 0; i + 1 < numSamples; i += 2) {
        for (int j = 0; j < EARLY_TAPS; ++ j) {
            gs.m_cfs[j].inc();
        }

        gs.pIn[gs.index] = pInput[i];
        gs.index = (gs.index + 1) % MAX_BUFFER_LEN;
        gs.pIn[gs.index] = pInput[i + 1];
        gs.index = (gs.index + 1) % MAX_BUFFER_LEN;

        gs.m_apf.inc();
    }

    float left, right;
    for (unsigned int i = 0; i + 1 < numSamples; i += 2) {
        pOutput[i] = pInput[i];
        pOutput[i + 1] = pInput[i + 1];
        left = right = 0;
        for (int j = 0; j < EARLY_TAPS; ++ j) {
            gs.m_cfs[j].process(sampleRate, left, right);
        }

        for (int j = 0; j < LATE_REFLECTIONS; ++ j) {
            gs.m_cfs[EARLY_TAPS + j].process(
                                            sampleRate,
                                            left,
                                            right
                                    );
        }

        left /= (EARLY_TAPS + LATE_REFLECTIONS);
        right /= (EARLY_TAPS + LATE_REFLECTIONS);

               gs.pOut[old_index] *=  freq;
        gs.pOut[old_index] = (gs.pOut[old_index] + pOutput[i]) / 2;
        old_index = (old_index + 1) % MAX_BUFFER_LEN;
        gs.pOut[old_index] *= freq;
        gs.pOut[old_index] = (gs.pOut[old_index] + pOutput[i + 1]) / 2;
        old_index = (old_index + 1) % MAX_BUFFER_LEN;


        gs.m_apf.process(sampleRate, left, right);
        pOutput[i] = (pOutput[i] * (1 - decay) + left * decay);
        pOutput[i + 1] = (pOutput[i + 1] * (1 - decay) + right * decay);

        pOutput[i + 1] *= 1.2;


    }
    //gs.m_apf.applyFilter();

}
