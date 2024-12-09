/*
  ==============================================================================

    ParallelPortOutputEditor.h
    Created: 8 Jan 2024 11:31:50am
    Author:  Joel Nielsen

  ==============================================================================
*/

#ifndef PARALLELPORTOUTPUTEDITOR_H_INCLUDED
#define PARALLELPORTOUTPUTEDITOR_H_INCLUDED

//#include <EditorHeaders.h>


/**

 User interface for the "ParallelPortOutput" node.

 @see ParallelPortOutput

 */
#include <VisualizerEditorHeaders.h>

class ParallelPortOutputEditor : public VisualizerEditor
    , public Label::Listener
    , public Button::Listener
{
public:
    ParallelPortOutputEditor(GenericProcessor* parentNode);

    /** Destructor */
    ~ParallelPortOutputEditor() { }

    /** Creates the canvas */
    Visualizer* createNewCanvas();

    void labelTextChanged(juce::Label* label) override;


    void setDisplayedPort(int port);

    void buttonClicked(Button* button) override;

    void saveVisualizerEditorParameters(XmlElement* xml) override;
    void loadVisualizerEditorParameters(XmlElement* xml) override;

private:
    ScopedPointer<Label> inChanLabel;
    ScopedPointer<ComboBox> inChanBox;

    ScopedPointer<Label> portLabel;

    ScopedPointer<Label> durationLabel;

    ScopedPointer<ComboBox> pinBox;

    ScopedPointer<UtilityButton> fileButton;
    //ScopedPointer<Label> file;
    ScopedPointer<Label> fileNameLabel;

    File lastFilePath;
};


#endif  // EVENTBROADCASTEREDITOR_H_INCLUDED
