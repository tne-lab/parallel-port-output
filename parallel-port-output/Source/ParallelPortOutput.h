/*
  ==============================================================================

    ParallelPortOutput.h
    Created: 8 Jan 2024 11:31:50am
    Author:  Joel Nielsen

  ==============================================================================
*/

#ifndef EVENTBROADCASTER_H_INCLUDED
#define EVENTBROADCASTER_H_INCLUDED

#include <ProcessorHeaders.h>


class ParallelPortOutput : public GenericProcessor
{
    friend class ParallelPortOutputEditor;

public:
    ParallelPortOutput();

    AudioProcessorEditor* createEditor() override;

    void setParameter(int parameterIndex, float newValue) override;

    /*start required updated for v6*/ 
    void process(AudioBuffer<float>& continuousBuffer) override;


    /** Responds to incoming events if a stimEventChannel is selected. */
    void handleTTLEvent(TTLEventPtr event) override;
    //void handleEvent(const EventChannel* channelInfo, const MidiMessage& event, int samplePosition = 0) override;

    /*bool disable() override; is bool stopAcquisition() override; updated in v6*/
    bool stopAcquisition() override;
    /*end required updated for v6*/

    bool setMapPath(std::string filePath);

    // for consistency across, might be better to keep it out
    enum Parameter
    {
        PORT,
        DURATION
    };

    int chan;
    float sampleRate;
    const DataStream* stream;
    std::string channelMapPath;

private:
    void sendState();
    std::string trim(const std::string str);

    int port;
    int duration;

    uint8_t pinState;

    // Map of incoming TTL channels to pin numbers (range 1-8 for both values)
    std::map<int, int> channelPinMap;

    // Map of pin number to timestamp to turn off that pin
    std::unordered_map<int, uint64_t> pinTurnOffSamples;
};


#endif  // EVENTBROADCASTER_H_INCLUDED
