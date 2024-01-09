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

    enum Parameter
    {
        OUTPUT_CHAN,
        PORT,
        DURATION
    };

private:
    void setPort(bool high);

    int port;
    int eventChannel;
    int offSample;
    int duration;
};


#endif  // EVENTBROADCASTER_H_INCLUDED
