
// bpmcontrol.h
// Created 7/5/2009 by RJ Ryan (rryan@mit.edu)

#ifndef BPMCONTROL_H
#define BPMCONTROL_H

#include "engine/enginecontrol.h"
#include "engine/enginesync.h"
#include "tapfilter.h"

class ControlObject;
class ControlPushButton;
class EngineBuffer;

class BpmControl : public EngineControl {
    Q_OBJECT

  public:
    BpmControl(const char* _group, ConfigObject<ConfigValue>* _config);
    virtual ~BpmControl();

    double getBpm() const;
    double getFileBpm() const { return m_dFileBpm; }
    void onEngineRateChange(double rate);
    double getSyncAdjustment() const;
    void setUserTweakingSync(bool tweakActive);
    // Get the phase offset from the specified position.
    double getPhaseOffset(double reference_position);

    void setCurrentSample(const double dCurrentSample, const double dTotalSamples);
    double process(const double dRate,
                   const double dCurrentSample,
                   const double dTotalSamples,
                   const int iBufferSize);

  public slots:
    virtual void trackLoaded(TrackPointer pTrack);
    virtual void trackUnloaded(TrackPointer pTrack);

  private slots:
    void slotSetEngineBpm(double);
    void slotFileBpmChanged(double);
    void slotControlPlay(double);
    void slotControlBeatSync(double);
    void slotControlBeatSyncPhase(double);
    void slotControlBeatSyncTempo(double);
    void slotTapFilter(double,int);
    void slotBpmTap(double);
    void slotAdjustBpm();
    void slotUpdatedTrackBeats();
    void slotBeatsTranslate(double);
    void slotMasterBeatDistanceChanged(double);

  private:
    double getBeatDistance(double dThisPosition) const;
    EngineBuffer* pickSyncTarget();
    bool syncTempo();
    bool syncPhase();

    // ControlObjects that come from PlayerManager
    ControlObject* m_pNumDecks;

    // ControlObjects that come from EngineBuffer
    ControlObject* m_pPlayButton;
    ControlObject* m_pQuantize;
    ControlObject* m_pRateSlider;
    ControlObject* m_pRateRange;
    ControlObject* m_pRateDir;

    ControlObject* m_pThisBeatDistance;
    ControlObject* m_pMasterBeatDistance;
    ControlObject* m_pSyncState;
    double m_dSyncAdjustment;
    bool m_bUserTweakingSync;
    double m_dUserOffset;

    // Is vinyl control enabled?
    ControlObject* m_pVCEnabled;

    // ControlObjects that come from LoopingControl
    ControlObject* m_pLoopEnabled;
    ControlObject* m_pLoopStartPosition;
    ControlObject* m_pLoopEndPosition;

    /** The current loaded file's detected BPM */
    ControlObject* m_pFileBpm;

    /** The current effective BPM of the engine */
    ControlObject* m_pEngineBpm;

    // Used for bpm tapping from GUI and MIDI
    ControlPushButton* m_pButtonTap;

    /** Button for sync'ing with the other EngineBuffer */
    ControlPushButton* m_pButtonSync;
    ControlPushButton* m_pButtonSyncPhase;
    ControlPushButton* m_pButtonSyncTempo;

    // Button that translates the beats so the nearest beat is on the current
    // playposition.
    ControlPushButton* m_pTranslateBeats;

    double m_dFileBpm;
    double m_dLoopSize; // Only used to see if we shouldn't quantize position.
    double m_dPreviousSample;

    TapFilter m_tapFilter;

    TrackPointer m_pTrack;
    BeatsPointer m_pBeats;

    QString m_sGroup;
};


#endif /* BPMCONTROL_H */