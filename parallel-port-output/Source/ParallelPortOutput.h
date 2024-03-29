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

    void process(AudioSampleBuffer& continuousBuffer) override;
    void handleEvent(const EventChannel* channelInfo, const MidiMessage& event, int samplePosition = 0) override;

    bool disable() override;

    bool setMapPath(std::string filePath);

    enum Parameter
    {
        PORT,
        DURATION
    };

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
