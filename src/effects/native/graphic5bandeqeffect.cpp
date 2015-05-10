#include "effects/native/graphic5bandeqeffect.h"
#include "util/math.h"

#define Q 1.2247449

// static
QString Graphic5BandEQEffect::getId() {
    return "org.mixxx.effects.graphiceq5band";
}

// static
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

    // Display rounded center frequencies for each filter
    float centerFrequencies[5] = {45, 500, 2500,
                                  5500, 12000};

    EffectManifestParameter* lowGain = manifest.addParameter();
    lowGain->setId(QString("low_gain"));
    lowGain->setName(QString("Low Gain"));
    lowGain->setDescription(QString("Gain for Low Filter"));
    lowGain->setControlHint(EffectManifestParameter::CONTROL_KNOB_LINEAR);
    lowGain->setSemanticHint(EffectManifestParameter::SEMANTIC_UNKNOWN);
    lowGain->setUnitsHint(EffectManifestParameter::UNITS_UNKNOWN);
    //lowGain->setNeutralPointOnScale(0.5);
    lowGain->setDefault(0);
    lowGain->setMinimum(-12);
    lowGain->setMaximum(12);

    EffectManifestParameter* lowFreq = manifest.addParameter();
    lowFreq->setId(QString("low_freq"));
    lowFreq->setName(QString("Low Freq"));
    lowFreq->setDescription(QString("Frequency corner for Low Filter"));
    lowFreq->setControlHint(EffectManifestParameter::CONTROL_KNOB_LINEAR);
    lowFreq->setSemanticHint(EffectManifestParameter::SEMANTIC_UNKNOWN);
    lowFreq->setUnitsHint(EffectManifestParameter::UNITS_UNKNOWN);
    lowFreq->setDefault(centerFrequencies[0]);
    lowFreq->setMinimum(20);
    lowFreq->setMaximum(250);

    EffectManifestParameter* lowQ = manifest.addParameter();
    lowQ->setId(QString("low_Q"));
    lowQ->setName(QString("Low q"));
    lowQ->setDescription(QString("Q factor for Low Filter"));
    lowQ->setControlHint(EffectManifestParameter::CONTROL_KNOB_LINEAR);
    lowQ->setSemanticHint(EffectManifestParameter::SEMANTIC_UNKNOWN);
    lowQ->setUnitsHint(EffectManifestParameter::UNITS_UNKNOWN);
    lowQ->setDefault(Q);
    lowQ->setMinimum(Q + 100);
    lowQ->setMaximum(Q - 100);
#if 0
    QString paramName;
    for (int i = 0; i < 3; i++) {
        if (centerFrequencies[i + 1] < 1000) {
            paramName = QString("%1 Hz").arg(centerFrequencies[i + 1]);
        } else {
            paramName = QString("%1 kHz").arg(centerFrequencies[i + 1] / 1000);
        }

        EffectManifestParameter* mid = manifest.addParameter();
        mid->setId(QString("mid%1").arg(i));
        mid->setName(paramName);
        mid->setDescription(QString("Gain for Band Filter %1").arg(i));
        mid->setControlHint(EffectManifestParameter::CONTROL_KNOB_LINEAR);
        mid->setSemanticHint(EffectManifestParameter::SEMANTIC_UNKNOWN);
        mid->setUnitsHint(EffectManifestParameter::UNITS_UNKNOWN);
        mid->setNeutralPointOnScale(0.5);
        mid->setDefault(0);
        mid->setMinimum(-12);
        mid->setMaximum(12);
    }

    EffectManifestParameter* high = manifest.addParameter();
    high->setId(QString("high"));
    high->setName(QString("%1 kHz").arg(centerFrequencies[7] / 1000));
    high->setDescription(QString("Gain for High Filter"));
    high->setControlHint(EffectManifestParameter::CONTROL_KNOB_LINEAR);
    high->setSemanticHint(EffectManifestParameter::SEMANTIC_UNKNOWN);
    high->setUnitsHint(EffectManifestParameter::UNITS_UNKNOWN);
    high->setDefault(0);
    high->setMinimum(-12);
    high->setMaximum(12);
#endif
    return manifest;
}

Graphic5BandEQEffectGroupState::Graphic5BandEQEffectGroupState() {
    m_oldLow = 0;
    for (int i = 0; i < 6; i++) {
        m_oldMid.append(1.0);
    }
    m_oldHigh = 0;

    m_pBufs.append(SampleUtil::alloc(MAX_BUFFER_LEN));
    m_pBufs.append(SampleUtil::alloc(MAX_BUFFER_LEN));

    // Initialize the default center frequencies
    m_centerFrequencies[0] = 81;
    m_centerFrequencies[1] = 494;
    m_centerFrequencies[2] = 1097;
    m_centerFrequencies[3] = 5416;
    m_centerFrequencies[4] = 9828;

    // Initialize qs
    m_q[0] = Q;
    m_q[1] = Q;
    m_q[2] = Q;
    m_q[3] = Q;
    m_q[4] = Q;

    // Initialize the filters with default parameters
    m_low = new EngineFilterBiquad1LowShelving(44100, m_centerFrequencies[0], m_q[0]);
    m_high = new EngineFilterBiquad1HighShelving(44100, m_centerFrequencies[4], m_q[4]);
    for (int i = 1; i < 4; i++) {
        m_bands.append(new EngineFilterBiquad1Peaking(44100,
                                                      m_centerFrequencies[i],
                                                      m_q[i]));
    }
}

Graphic5BandEQEffectGroupState::~Graphic5BandEQEffectGroupState() {
    foreach (EngineFilterBiquad1Peaking* filter, m_bands) {
        delete filter;
    }

    delete m_low;
    delete m_high;

    foreach(CSAMPLE* buf, m_pBufs) {
        SampleUtil::free(buf);
    }
}

void Graphic5BandEQEffectGroupState::setFilters(int sampleRate) {
    m_low->setFrequencyCorners(sampleRate, m_centerFrequencies[0], m_q[0],
                               m_oldLow);
#if 0
    m_high->setFrequencyCorners(sampleRate, m_centerFrequencies[4], m_q[4],
                                m_oldHigh);
    for (int i = 0; i < 3 i++) {
        m_bands[i]->setFrequencyCorners(sampleRate, m_centerFrequencies[i + 1],
                                        Q, m_oldMid[i]);
    }
#endif
}

Graphic5BandEQEffect::Graphic5BandEQEffect(EngineEffect* pEffect,
                                 const EffectManifest& manifest)
        : m_oldSampleRate(44100) {
    Q_UNUSED(manifest);
    m_pPotLow = pEffect->getParameterById("low_gain");
    m_pCenterLow = pEffect->getParameterById("low_freq");
    m_pQLow = pEffect->getParameterById("low_Q");
#if 0
    for (int i = 0; i < 4; i++) {
        m_pPotMid.append(pEffect->getParameterById(QString("mid%1").arg(i)));
    }
    m_pPotHigh = pEffect->getParameterById("high");
#endif
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

    float fLow;
    float fMid[3];
    float fHigh;

    if (enableState == EffectProcessor::DISABLING) {
         // Ramp to dry, when disabling, this will ramp from dry when enabling as well
        fLow = 1.0;
        pState->m_centerFrequencies[0] = 40;
        pState->m_q[0] = Q;
        fHigh = 1.0;
        for (int i = 0; i < 3; i++) {
            fMid[i] = 1.0;
        }
    } else {
        fLow = m_pPotLow->value();
        pState->m_centerFrequencies[0] = m_pCenterLow->value();
        pState->m_q[0] = m_pQLow->value();
    //    fHigh = m_pPotHigh->value();
        for (int i = 0; i < 3; i++) {
      //      fMid[i] = m_pPotMid[i]->value();
        }
    }


    if (fLow != pState->m_oldLow) {
        pState->m_low->setFrequencyCorners(sampleRate,
                                           pState->m_centerFrequencies[0], Q,
                                           fLow);
    }
#if 0
    if (fHigh != pState->m_oldHigh) {
        pState->m_high->setFrequencyCorners(sampleRate,
                                            pState->m_centerFrequencies[7], Q,
                                            fHigh);
    }
    for (int i = 0; i < 6; i++) {
        if (fMid[i] != pState->m_oldMid[i]) {
            pState->m_bands[i]->setFrequencyCorners(sampleRate,
                                                    pState->m_centerFrequencies[i + 1],
                                                    Q, fMid[i]);
        }
    }
#endif
    int bufIndex = 0;
    if (fLow) {
        pState->m_low->process(pInput, pState->m_pBufs[1 - bufIndex], numSamples);
        bufIndex = 1 - bufIndex;
    } else {
        pState->m_low->pauseFilter();
        SampleUtil::copy(pState->m_pBufs[bufIndex], pInput, numSamples);
    }
#if 0
    for (int i = 0; i < 6; i++) {
        if (fMid[i]) {
            pState->m_bands[i]->process(pState->m_pBufs[bufIndex],
                                        pState->m_pBufs[1 - bufIndex], numSamples);
            bufIndex = 1 - bufIndex;
        } else {
            pState->m_bands[i]->pauseFilter();
        }
    }

#endif
    fHigh = 0.0;
    if (fHigh) {
        pState->m_high->process(pState->m_pBufs[bufIndex],
                                pOutput, numSamples);
        bufIndex = 1 - bufIndex;
    } else {
        SampleUtil::copy(pOutput, pState->m_pBufs[bufIndex], numSamples);
        pState->m_high->pauseFilter();
    }

    pState->m_oldLow = fLow;
#if 0
    pState->m_oldHigh = fHigh;
    for (int i = 0; i < 6; i++) {
        pState->m_oldMid[i] = fMid[i];
    }
#endif
    if (enableState == EffectProcessor::DISABLING) {
        pState->m_low->pauseFilter();
#if 0
        pState->m_high->pauseFilter();
        for (int i = 0; i < 6; i++) {
            pState->m_bands[i]->pauseFilter();
        }
#endif
    }
}
