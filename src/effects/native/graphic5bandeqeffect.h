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

    QList<EngineFilterBiquad1Peaking*> m_bands;
    QList<CSAMPLE*> m_pBufs;
    double m_gain[5];
    double m_freq[5];
    double m_q[5];

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

    QList<EngineEffectParameter*> m_pGain;
    QList<EngineEffectParameter*> m_pFreq;
    QList<EngineEffectParameter*> m_pQ;
    unsigned int m_oldSampleRate;

    DISALLOW_COPY_AND_ASSIGN(Graphic5BandEQEffect);
};

#endif //GRAFIC5BANDEQEFFECT_H
