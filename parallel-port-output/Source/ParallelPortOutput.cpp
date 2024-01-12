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

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <bitset>


short _stdcall Inp32(short PortAddress);
void _stdcall Out32(short PortAddress, short data);


ParallelPortOutput::ParallelPortOutput()
    : GenericProcessor("ParallelPortOutput")
    , port(0)
    , duration(10)
    , pinState(0x00)
{
    setProcessorType (PROCESSOR_TYPE_SINK);
}

std::string toBinaryString(uint8_t byte) {
    return std::bitset<8>(byte).to_string();
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
        case PORT:
            port = static_cast<int>(newValue);
            break;
        case DURATION:
            duration = static_cast<int>(newValue);
            break;
    }
}

void ParallelPortOutput::sendState()
{
    Out32(port, pinState);
}


void ParallelPortOutput::process(AudioSampleBuffer& continuousBuffer)
{
    std::vector<int> pinsToTurnOff;
    bool turnOff = false;
    int currentSample = getTimestamp(0) + getNumSamples(0);

    // Get pins to be removed
    for (const auto& pair : pinTurnOffSamples) {
        int pin = pair.first;
        uint64_t turnOffSample = pair.second;

        if (currentSample >= turnOffSample) {
            std::cout << "Turning off pin: " << pin << ". Current state" << toBinaryString(pinState) << "Samples:" << currentSample << ". Turn off:" << turnOffSample << std::endl;
            pinsToTurnOff.push_back(pin);
            turnOff = true;
        }
    }

    // Turn off the pins and remove them from the map
    for (int pin : pinsToTurnOff) {
        pinState &= ~(1 << (pin - 1));
        pinTurnOffSamples.erase(pin);
    }

    // Send new state
    if (turnOff)
    {
        std::cout << "Turn off state: " << toBinaryString(pinState) << std::endl;
        sendState();
    }
 
    
    checkForEvents(false);
}


void ParallelPortOutput::handleEvent(const EventChannel* channelInfo, const MidiMessage& event, int samplePosition)
{
    
    if (Event::getEventType(event) == EventChannel::TTL) 
    {
        TTLEventPtr ttl = TTLEvent::deserializeFromMessage(event, channelInfo);

        auto it = channelPinMap.find(ttl->getChannel());

        if (it != channelPinMap.end() && ttl->getState())
        {
            
            int pin = it->second;
            std::cout << "Setting pin:" << pin << ". Old pin state" << toBinaryString(pinState) << std::endl;
            pinState |= (1 << (pin - 1));
            std::cout << "New pin state" << toBinaryString(pinState) << std::endl;
            sendState();
            int sampleRate = getTotalDataChannels() > 0 ? getDataChannel(0)->getSampleRate() : 30000;
            pinTurnOffSamples[pin] = ttl->getTimestamp() + int(std::floor(duration * sampleRate / 1000.0f));
        }
    }
}


bool ParallelPortOutput::setMapPath(std::string filePath)
{
    if (filePath.empty())
    {
        return false;
    }
    channelPinMap.clear();
    std::ifstream file(filePath);

    if (!file)
    {
        std::cout << "Cannot open channel-pin config file " << filePath << std::endl;
        CoreServices::sendStatusMessage("Cannot open channel-pin config file");
        return false;
    }

    channelMapPath = filePath;

    // Perform a naive JSON parsing
    try
    {
        std::stringstream ss;
        ss << file.rdbuf();
        file.close();

        // remove curly braces
        std::string str = ss.str();
        str.erase(std::remove(str.begin(), str.end(), '{'), str.end());
        str.erase(std::remove(str.begin(), str.end(), '}'), str.end());

        // split by comma
        std::vector<std::string> pairs;
        size_t pos = 0;
        std::string token;
        while ((pos = str.find(',')) != std::string::npos)
        {
            pairs.push_back(str.substr(0, pos));
            str.erase(0, pos + 1);
        }
        pairs.push_back(str);

        for (const auto& pair : pairs)
        {
            size_t colonPos = pair.find(':');
            if (colonPos != std::string::npos)
            {
                std::string channelStr = trim(pair.substr(0, colonPos));
                int channel = std::stoi(channelStr) - 1;
                int pin = std::stoi(trim(pair.substr(colonPos + 1)));

                if (channel < 0 || channel > 7 || pin < 1 || pin > 8)
                {
                    std::cerr << "Channel or pin number out of range in mapping: " << pair << std::endl;
                    continue;
                }

                std::cout << "PP setting map entry. Channel:" << channel << ". Pin:" << pin << std::endl;
                channelPinMap[channel] = pin;
            }
        }
    }
    catch (const std::runtime_error& re)
    {
        std::cout << "Failed to read channel to pin mapping file with runtime error: " << re.what() << std::endl;
        return false;
    }
    catch (const std::exception& ex)
    {
        std::cout << "Failed to read channel to pin mapping file with exception: " << ex.what() << std::endl;
        return false;
    }

    return true;
}

bool ParallelPortOutput::disable()
{
    pinState = 0x00;
    sendState();
    pinTurnOffSamples.clear();
    return true;
}

inline std::string ParallelPortOutput::trim(const std::string const_str)
{
    std::string str = const_str;
    str.erase(str.find_last_not_of(" \n\r\t\"") + 1);
    str.erase(0, str.find_first_not_of(" \n\r\t\""));
    return str;
}