/*
  ==============================================================================

    ParallelPortOutputEditor.h
    Created: 8 Jan 2024 11:31:50am
    Author:  Joel Nielsen

  ==============================================================================
*/

#ifndef PARALLELPORTOUTPUTEDITOR_H_INCLUDED
#define PARALLELPORTOUTPUTEDITOR_H_INCLUDED

#include <EditorHeaders.h>


/**

 User interface for the "ParallelPortOutput" node.

 @see ParallelPortOutput

 */

class ParallelPortOutputEditor
    : public GenericEditor
    , public Label::Listener
    , public ComboBox::Listener
    , public Button::Listener
{
public:
    ParallelPortOutputEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);

    void labelTextChanged(juce::Label* label) override;
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

    void setDisplayedPort(int port);

    void buttonClicked(Button* button) override;

    void saveCustomParameters(XmlElement* xml) override;
    void loadCustomParameters(XmlElement* xml) override;

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
