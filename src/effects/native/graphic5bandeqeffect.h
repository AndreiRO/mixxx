#ifndef GRAFIC5BANDEQEFFECT_H
#define GRAFIC5BANDEQEFFECT_H

#include <QMap>

#include "controlobjectslave.h"
#include "effects/effect.h"
#include "effects/effectprocessor.h"
#include "engine/effects/engineeffect.h"
#include "engine/effects/engineeffectparameter.h"
#include "engine/enginefilterbiquad1.h"
#include "util.h"
#include "util/types.h"
#include "util/defs.h"
#include "sampleutil.h"

class Graphic5BandEQEffectGroupState {
  public:
    Graphic5BandEQEffectGroupState();
    virtual ~Graphic5BandEQEffectGroupState();

    void setFilters(int sampleRate);

    EngineFilterBiquad1LowShelving* m_low;
    QList<EngineFilterBiquad1Peaking*> m_bands;
    EngineFilterBiquad1HighShelving* m_high;
    QList<CSAMPLE*> m_pBufs;
    QList<double> m_oldMid;
    double m_oldLow;
    double m_oldHigh;
    float m_centerFrequencies[5];
    float m_q[5];
};

class Graphic5BandEQEffect : public PerChannelEffectProcessor<Graphic5BandEQEffectGroupState> {
  public:
    Graphic5BandEQEffect(EngineEffect* pEffect, const EffectManifest& manifest);
    virtual ~Graphic5BandEQEffect();

    static QString getId();
    static EffectManifest getManifest();

    // See effectprocessor.h
    void processChannel(const ChannelHandle& handle,
                        Graphic5BandEQEffectGroupState* pState,
                        const CSAMPLE* pInput, CSAMPLE *pOutput,
                        const unsigned int numSamples,
                        const unsigned int sampleRate,
                        const EffectProcessor::EnableState enableState,
                        const GroupFeatureState& groupFeatureState);

  private:
    QString debugString() const {
        return getId();
    }

    EngineEffectParameter* m_pPotLow;
    EngineEffectParameter* m_pCenterLow;
    EngineEffectParameter* m_pQLow;
    QList<EngineEffectParameter*> m_pPotMid;
    QList<EngineEffectParameter*> m_pCenterMid;
    QList<EngineEffectParameter*> m_pQMid;
    EngineEffectParameter* m_pPotHigh;
    EngineEffectParameter* m_pCenterHigh;
    EngineEffectParameter* m_pQHigh;
    unsigned int m_oldSampleRate;

    DISALLOW_COPY_AND_ASSIGN(Graphic5BandEQEffect);
};

#endif //GRAFIC5BANDEQEFFECT_H
