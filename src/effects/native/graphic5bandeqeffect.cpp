#include "effects/native/graphic5bandeqeffect.h"
#include<iostream>
#include "util/math.h"

#define Q 1.2247449

// static
QString Graphic5BandEQEffect::getId() {
    return "org.mixxx.effects.graphiceq5band";
}

void initialize_values(double gains[], double freq[], double q[]) {
    for (int i = 0; i < 5; ++ i) {
        gains[i] = 1;
        q[i] = 0.5;
        freq[i] = 1 + i * 400 + 200;
    }
}

EffectManifest Graphic5BandEQEffect::getManifest() {
    EffectManifest manifest;
    manifest.setId(getId());
    manifest.setName(QObject::tr("Graphic 5 band EQ"));
    manifest.setAuthor("The Mixxx Team");
    manifest.setVersion("1.0");
    manifest.setDescription(QObject::tr(
        "A 5 band Graphic EQ based on Biquad Filters"));
    manifest.setEffectRampsFromDry(true);
    manifest.setIsMasterEQ(true);

    QString paramName;
    for (int i = 0; i < 5; i++) {
        paramName = QString("Gain: %1").arg(i + 1);
        EffectManifestParameter* gain = manifest.addParameter();
        gain->setId(QString("gain_%1").arg(i));
        gain->setName(paramName);
        gain->setDescription(QString("Gain for Band Filter %1").arg(i));
        gain->setControlHint(EffectManifestParameter::CONTROL_KNOB_LINEAR);
        gain->setSemanticHint(EffectManifestParameter::SEMANTIC_UNKNOWN);
        gain->setUnitsHint(EffectManifestParameter::UNITS_UNKNOWN);
        gain->setDefault(0.5);
        gain->setMinimum(0.1);
        gain->setMaximum(1);

        paramName = QString("Freq: %1").arg(i + 1);
        EffectManifestParameter* freq = manifest.addParameter();
        freq->setId(QString("freq_%1").arg(i));
        freq->setName(paramName);
        freq->setDescription(QString("Freq for Band Filter %1").arg(i));
        freq->setControlHint(EffectManifestParameter::CONTROL_KNOB_LINEAR);
        freq->setSemanticHint(EffectManifestParameter::SEMANTIC_UNKNOWN);
        freq->setUnitsHint(EffectManifestParameter::UNITS_UNKNOWN);
        freq->setDefault((i) * 400 + 200);
        freq->setMinimum(1 + i * 400);
        freq->setMaximum((i + 1) * 400);

        paramName = QString("Q: %1").arg(i + 1);
        EffectManifestParameter* q = manifest.addParameter();
        q->setId(QString("q_%1").arg(i));
        q->setName(paramName);
        q->setDescription(QString("Q Factor for Band Filter %1").arg(i));
        q->setControlHint(EffectManifestParameter::CONTROL_KNOB_LINEAR);
        q->setSemanticHint(EffectManifestParameter::SEMANTIC_UNKNOWN);
        q->setUnitsHint(EffectManifestParameter::UNITS_UNKNOWN);
        q->setDefault(0.5);
        q->setMinimum(0);
        q->setMaximum(1);
    }

    return manifest;
}

Graphic5BandEQEffectGroupState::Graphic5BandEQEffectGroupState() {
    // Initialize the filters with default parameters
    initialize_values(m_gain, m_freq, m_q);

    for (int i = 0; i < 5; i++) {
        m_bands.append(new EngineFilterBiquad1Peaking(44100,
                                                      i * 400 + 200,
                                                      0.5));
        m_pBufs.append(SampleUtil::alloc(MAX_BUFFER_LEN));
        m_pBufs.append(SampleUtil::alloc(MAX_BUFFER_LEN));
    }

}

Graphic5BandEQEffectGroupState::~Graphic5BandEQEffectGroupState() {
    foreach (EngineFilterBiquad1Peaking* filter, m_bands) {
        delete filter;
    }

    foreach(CSAMPLE* buf, m_pBufs) {
        SampleUtil::free(buf);
    }
}

void Graphic5BandEQEffectGroupState::setFilters(int sampleRate) {
    for (int i = 0; i < 5; i++) {
        m_bands[i]->setFrequencyCorners(sampleRate, m_freq[i], m_q[i], m_gain[i]);
    }
}

Graphic5BandEQEffect::Graphic5BandEQEffect(EngineEffect* pEffect,
                                 const EffectManifest& manifest)
        : m_oldSampleRate(44100) {
    Q_UNUSED(manifest);
    for (int i = 0; i < 5; i++) {
        m_pGain.append(pEffect->getParameterById(QString("gain_%1").arg(i)));
        m_pFreq.append(pEffect->getParameterById(QString("freq_%1").arg(i)));
        m_pQ.append(pEffect->getParameterById(QString("q_%1").arg(i)));
    }
}

Graphic5BandEQEffect::~Graphic5BandEQEffect() {
}

void Graphic5BandEQEffect::processChannel(const ChannelHandle& handle,
                                     Graphic5BandEQEffectGroupState* pState,
                                     const CSAMPLE* pInput, CSAMPLE* pOutput,
                                     const unsigned int numSamples,
                                     const unsigned int sampleRate,
                                     const EffectProcessor::EnableState enableState,
                                     const GroupFeatureState& groupFeatures) {
    Q_UNUSED(handle);
    Q_UNUSED(groupFeatures);

    // If the sample rate has changed, initialize the filters using the new
    // sample rate
    if (m_oldSampleRate != sampleRate) {
        m_oldSampleRate = sampleRate;
        pState->setFilters(sampleRate);
    }

    double newGain[5];
    double newFreq[5];
    double newQ[5];
    bool toProcess[] = {false, false, false, false, false};

    if (enableState == EffectProcessor::DISABLING) {
         // Ramp to dry, when disabling, this will ramp from dry when enabling as well
        initialize_values(newGain, newFreq, newQ);
    } else {
        for (int i = 0; i < 5; i++) {
            qDebug() << newGain[i] <<' ' << pState->m_gain[i] << '\n';
            newGain[i] = m_pGain[i]->value();
            newFreq[i] = m_pFreq[i]->value();
            newQ[i] = m_pQ[i]->value();
        }
    }


    for (int i = 0; i < 5; ++ i) {
        if (newGain[i] != pState->m_gain[i] ||
            newFreq[i] != pState->m_freq[i] ||
            newQ[i] != pState->m_q[i]) {
                toProcess[i] = true;
                pState->m_bands[i]->setFrequencyCorners(
                                                    sampleRate,
                                                    newGain[i],
                                                    newFreq[i],
                                                    newQ[i]
            );
        }
    }
    SampleUtil::copy(pState->m_pBufs[0], pInput, numSamples);

    int bufIndex = 0;
    for (int i = 0; i < 4; ++ i) {
       if (toProcess[i]) {
            pState->m_bands[i]->process(pState->m_pBufs[bufIndex], pState->m_pBufs[1 - bufIndex], numSamples);
            bufIndex = 1 - bufIndex;
        } else {
            pState->m_bands[i]->pauseFilter();
        }
    }

    if (toProcess[4]) {
        pState->m_bands[4]->process(pState->m_pBufs[bufIndex], pOutput, numSamples);
    } else {
        pState->m_bands[4]->pauseFilter();
        SampleUtil::copy(pOutput, pState->m_pBufs[bufIndex], numSamples);
    }

    /* copy old values */
    for (int i = 0; i < 5; ++ i) {
        qDebug() << "Copying\n";
        pState->m_gain[i] = newGain[i];
        pState->m_freq[i] = newFreq[i];
        pState->m_q[i] = newQ[i];
        qDebug() << pState->m_gain[i] << ' ' << newGain[i] << "test\n";
    }
}
