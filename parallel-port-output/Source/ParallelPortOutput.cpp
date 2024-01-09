/*
  ==============================================================================

    ParallelPortOutput.cpp
    Created: 8 Jan 2024 11:31:50am
    Author:  Joel Nielsen

  ==============================================================================
*/

#include "ParallelPortOutput.h"
#include "ParallelPortOutputEditor.h"

#ifdef __cplusplus
extern "C" {
#endif
}
#include <iostream>
#include <thread>
#include <chrono>


short _stdcall Inp32(short PortAddress);
void _stdcall Out32(short PortAddress, short data);


ParallelPortOutput::ParallelPortOutput()
    : GenericProcessor  ("ParallelPortOutput")
    , port     (0)
    , eventChannel (0)
    , offSample(-1)
    , duration(10)
{
    setProcessorType (PROCESSOR_TYPE_SINK);
}


AudioProcessorEditor* ParallelPortOutput::createEditor()
{
    editor = new ParallelPortOutputEditor(this, true);
    return editor;
}

void ParallelPortOutput::setParameter(int parameterIndex, float newValue)
{
    switch (parameterIndex)
    {
    case OUTPUT_CHAN:
        eventChannel = static_cast<int>(newValue);
        break;
    case PORT:
        port = static_cast<int>(newValue);
        break;
    case DURATION:
        duration = static_cast<int>(newValue);
        break;
    }
}

void ParallelPortOutput::setPort(bool high)
{
    if (high)
        Out32(port, 0x01);
    else
        Out32(port, 0x00);
}


void ParallelPortOutput::process(AudioSampleBuffer& continuousBuffer)
{
    // Set low if necessary
    if (offSample > 0 && offSample <= getTimestamp(eventChannel) + getNumSamples(eventChannel))
    {
        setPort(false);
        offSample = -1;
    }
    
    checkForEvents(false);
}


void ParallelPortOutput::handleEvent(const EventChannel* channelInfo, const MidiMessage& event, int samplePosition)
{
    
    if (Event::getEventType(event) == EventChannel::TTL) 
    {
        TTLEventPtr ttl = TTLEvent::deserializeFromMessage(event, channelInfo);

        if (ttl->getChannel() == eventChannel && ttl->getState())
        {
            setPort(true);
            int timeout = 10;
            int sampleRate = getTotalDataChannels() > 0 ? getDataChannel(0)->getSampleRate() : 30000;
            offSample = ttl->getTimestamp() + int(std::floor(duration * sampleRate / 1000.0f));
        }
    }
}