/*
  ==============================================================================

    ParallelPortOutputEditor.cpp
    Created: 8 Jan 2024 11:31:50am
    Author:  Joel Nielsen

  ==============================================================================
*/

#include "ParallelPortOutputEditor.h"
#include "ParallelPortOutput.h"


bool isHexString(const String& input) {
    for (int i = 0; i < input.length(); ++i) {
        char c = input[i];
        if (!isxdigit(c)) {
            return false;
        }
    }
    return !input.isEmpty();
}

bool isInteger(const String& input) {
    // Check if the string can be converted to an integer
    return input.containsOnly("0123456789");
}

ParallelPortOutputEditor::ParallelPortOutputEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors)
    : GenericEditor(parentNode, useDefaultParameterEditors)

{
    desiredWidth = 180;

    const int TEXT_HT = 18;
    int xPos = 20;
    int yPos = 36;

    ParallelPortOutput* p = (ParallelPortOutput*) getProcessor();

    inChanLabel = new Label("InChan", "In:");
    inChanLabel->setBounds(xPos, yPos - 5, 140, 25);
    addAndMakeVisible(inChanLabel);

    inChanBox = new ComboBox("Input event channel");
    for (int chan = 1; chan <= 8; chan++)
        inChanBox->addItem(String(chan), chan);
    inChanBox->setSelectedId(p->eventChannel + 1);
    inChanBox->setBounds(xPos + 25, yPos, 40, TEXT_HT);
    inChanBox->setTooltip("Input TTL event channel");
    inChanBox->addListener(this);
    addAndMakeVisible(inChanBox);

    Label* portText = new Label("PortLabel", "Port:");
    portText->setBounds(20, 60, 140, 25);
    addAndMakeVisible(portText);

    Label* hexLabel = new Label("HexLabel", "0x");
    hexLabel->setBounds(xPos + 30, 60, 40, 25);
    hexLabel->setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(hexLabel);

    portLabel = new Label("Port", String::toHexString(p->port).toUpperCase());
    portLabel->setBounds(xPos + 50,65,60,18);
    portLabel->setFont(Font("Default", 15, Font::plain));
    portLabel->setColour(Label::textColourId, Colours::white);
    portLabel->setColour(Label::backgroundColourId, Colours::grey);
    portLabel->setEditable(true);
    portLabel->addListener(this);
    addAndMakeVisible(portLabel);

    Label* durationText = new Label("Duration", "Duration:");
    durationText->setBounds(xPos, 90, 140, 25);
    durationText->setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(durationText);

    durationLabel = new Label("Duration", String(p->duration));
    durationLabel->setBounds(xPos + 60, 94, 40, TEXT_HT);
    durationLabel->setFont(Font("Default", 15, Font::plain));
    durationLabel->setColour(Label::textColourId, Colours::white);
    durationLabel->setColour(Label::backgroundColourId, Colours::grey);
    durationLabel->setEditable(true);
    durationLabel->addListener(this);
    addAndMakeVisible(durationLabel);

    Label* durationMs = new Label("DurationMS", "ms");
    durationMs->setBounds(xPos + 100, 90, 140, 25);
    durationMs->setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(durationMs);
}

void ParallelPortOutputEditor::saveCustomParameters(XmlElement* xml)
{
    XmlElement* mainNode = xml->createNewChildElement("PARALLELPORTOUTPUT");
    mainNode->setAttribute("port", portLabel->getText());
    mainNode->setAttribute("channel", inChanBox->getSelectedId());
    mainNode->setAttribute("duration", durationLabel->getText());
}

void ParallelPortOutputEditor::loadCustomParameters(XmlElement* xml)
{
    forEachXmlChildElementWithTagName(*xml, xmlNode, "PARALLELPORTOUTPUT")
    {
        inChanBox->setSelectedId(xmlNode->getIntAttribute("channel", inChanBox->getSelectedId()), sendNotificationSync);
        portLabel->setText(xmlNode->getStringAttribute("port", portLabel->getText()), sendNotificationSync);
        durationLabel->setText(xmlNode->getStringAttribute("duration", durationLabel->getText()), sendNotificationSync);
    }


    auto processor = static_cast<ParallelPortOutput*>(getProcessor());
    XmlElement* mainNode = xml->createNewChildElement("PARALLELPORTOUTPUT");
    mainNode->setAttribute("port", portLabel->getText());
    mainNode->setAttribute("channel", inChanBox->getSelectedId());
}


void ParallelPortOutputEditor::labelTextChanged(juce::Label* label)
{
    auto processor = static_cast<ParallelPortOutput*>(getProcessor());
    if (label == portLabel)
    {
        String text = label->getText();
        int portNumber = text.getHexValue32(); // Convert text to hexadecimal

        if (isHexString(text))
        {
            processor->setParameter(ParallelPortOutput::PORT, portNumber);
            setDisplayedPort(portNumber);
        }
        else
        {
            // Handle invalid input
            setDisplayedPort(processor->port);
        }
    }
    else if (label == durationLabel)
    {
        String text = label->getText();
        if (isInteger(text))
        {
            processor->setParameter(ParallelPortOutput::DURATION, text.getIntValue());
        }
        else
        {
            durationLabel->setText(String(processor->duration), dontSendNotification);
        }
    }
}

void ParallelPortOutputEditor::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    auto processor = static_cast<ParallelPortOutput*>(getProcessor());
    if (comboBoxThatHasChanged == inChanBox)
    {
        processor->setParameter(ParallelPortOutput::OUTPUT_CHAN,
            static_cast<float>(inChanBox->getSelectedId() - 1));
    }
}

void ParallelPortOutputEditor::setDisplayedPort(int port)
{
    portLabel->setText(String::toHexString(port).toUpperCase(), dontSendNotification);
}

