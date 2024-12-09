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

ParallelPortOutputEditor::ParallelPortOutputEditor(GenericProcessor* parentNode)
    : VisualizerEditor(parentNode, "Parallel Port Output")

{
    desiredWidth = 225;

    const int TEXT_HT = 18;
    int xPos = 10;
    int yPos = 36;

    ParallelPortOutput* p = (ParallelPortOutput*) getProcessor();

    Label* fileDisplayText = new Label("Channel-pin mapping file", "Channel-pin mapping file");
    fileDisplayText->setFont(Font("Small Text", 10, Font::plain));
    fileDisplayText->setBounds(xPos, 30, 150, 8); // (110, 95, 100, 8)
    fileDisplayText->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(fileDisplayText);

    fileButton = new UtilityButton("F:", Font("Small Text", 12, Font::bold));
    fileButton->setRadius(3.0f);
    fileButton->setBounds(xPos + 5, 40, 20, 20); // (115, 105, 20, 20);
    fileButton->addListener(this);
    addAndMakeVisible(fileButton);

    fileNameLabel = new Label("Selected file", "No file selected");
    fileNameLabel->setFont(Font("Small Text", 10, Font::plain));
    fileNameLabel->setBounds(xPos + 30, 40, 100, 20); // (140, 105, 70, 20);
    fileNameLabel->setEditable(true);
    fileNameLabel->setEnabled(false);
    fileNameLabel->setColour(Label::backgroundColourId, Colours::lightgrey);
    fileNameLabel->addListener(this);
    addAndMakeVisible(fileNameLabel);


    Label* portText = new Label("PortLabel", "Port:   ");
    portText->setBounds(xPos, 70, 140, 25);
    addAndMakeVisible(portText);

    Label* hexLabel = new Label("HexLabel", "0x");
    hexLabel->setBounds(xPos + 45, 70, 40, 25);
    hexLabel->setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(hexLabel);

    portLabel = new Label("Port", String::toHexString(p->port).toUpperCase());
    portLabel->setBounds(xPos + 125,75,60,18);
    portLabel->setFont(Font("Default", 15, Font::plain));
    portLabel->setColour(Label::textColourId, Colours::white);
    portLabel->setColour(Label::backgroundColourId, Colours::grey);
    portLabel->setEditable(true);
    portLabel->addListener(this);
    addAndMakeVisible(portLabel);

    Label* durationText = new Label("Duration", "Duration (ms):");
    durationText->setBounds(xPos, 100, 140, 25);
    durationText->setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(durationText);

    durationLabel = new Label("Duration", String(p->duration));
    durationLabel->setBounds(xPos + 125, 104, 60, TEXT_HT);
    durationLabel->setFont(Font("Default", 15, Font::plain));
    durationLabel->setColour(Label::textColourId, Colours::white);
    durationLabel->setColour(Label::backgroundColourId, Colours::grey);
    durationLabel->setEditable(true);
    durationLabel->addListener(this);
    addAndMakeVisible(durationLabel);

    //Label* durationMs = new Label("DurationMS", "ms");
    //durationMs->setBounds(xPos + 100, 100, 140, 25);
    //durationMs->setColour(Label::textColourId, Colours::black);
    //addAndMakeVisible(durationMs);
}

void ParallelPortOutputEditor::buttonClicked(Button* button)
{
    auto processor = static_cast<ParallelPortOutput*>(getProcessor());
    if (button == fileButton)
    {
        String supportedFormats = "*.json";

        FileChooser chooseFileReaderFile("Please select a json file containing the markers mapping...",
            lastFilePath,
            supportedFormats);

        if (chooseFileReaderFile.browseForFileToOpen())
        {
            if (processor->setMapPath(chooseFileReaderFile.getResult().getFullPathName().toStdString()))
            {
                fileNameLabel->setText(chooseFileReaderFile.getResult().getFileName(), dontSendNotification);
            }
        }
    }
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


void ParallelPortOutputEditor::setDisplayedPort(int port)
{
    portLabel->setText(String::toHexString(port).toUpperCase(), dontSendNotification);
}


void ParallelPortOutputEditor::saveVisualizerEditorParameters(XmlElement* xml)
{
    auto processor = static_cast<ParallelPortOutput*>(getProcessor());
    XmlElement* mainNode = xml->createNewChildElement("PARALLELPORTOUTPUT");
    mainNode->setAttribute("port", portLabel->getText());
    mainNode->setAttribute("duration", durationLabel->getText());
    mainNode->setAttribute("map_path", processor->channelMapPath);
}

void ParallelPortOutputEditor::loadVisualizerEditorParameters(XmlElement* xml)
{
    forEachXmlChildElementWithTagName(*xml, xmlNode, "PARALLELPORTOUTPUT")
    {
        portLabel->setText(xmlNode->getStringAttribute("port", portLabel->getText()), sendNotificationSync);
        durationLabel->setText(xmlNode->getStringAttribute("duration", durationLabel->getText()), sendNotificationSync);
        auto processor = static_cast<ParallelPortOutput*>(getProcessor());

        if (xmlNode->hasAttribute("map_path"))
        {
            std::string path = xmlNode->getStringAttribute("map_path", "").toStdString();
            if (processor->setMapPath(path))
            {
                fileNameLabel->setText(path.substr(path.find_last_of("/\\") + 1), dontSendNotification);
            }
        }
    }
}

Visualizer* ParallelPortOutputEditor::createNewCanvas()
{
    return canvas.get();
}
