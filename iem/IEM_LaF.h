/*
 ==============================================================================
 This file is part of the IEM plug-in suite.
 Author: Daniel Rudrich
 Copyright (c) 2017 - Institute of Electronic Music and Acoustics (IEM)
 https://iem.at

 The IEM plug-in suite is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 The IEM plug-in suite is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this software.  If not, see <https://www.gnu.org/licenses/>.
 ==============================================================================
 */

#pragma once
#include <RobotoFont.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace iem {

class LaF : public juce::LookAndFeel_V4
{
public:
  const juce::Colour ClBackground = juce::Colour(0xFF2D2D2D);
  const juce::Colour ClFace = juce::Colour(0xFFD8D8D8);
  const juce::Colour ClFaceShadow = juce::Colour(0xFF272727);
  const juce::Colour ClFaceShadowOutline = juce::Colour(0xFF212121);
  const juce::Colour ClFaceShadowOutlineActive = juce::Colour(0xFF7C7C7C);
  const juce::Colour ClRotSliderArrow = juce::Colour(0xFF4A4A4A);
  const juce::Colour ClRotSliderArrowShadow = juce::Colour(0x445D5D5D);
  const juce::Colour ClSliderFace = juce::Colour(0xFF191919);
  const juce::Colour ClText = juce::Colour(0xFFFFFFFF);
  const juce::Colour ClTextTextboxbg = juce::Colour(0xFF000000);
  const juce::Colour ClSeperator = juce::Colour(0xFF979797);
  const juce::Colour ClWidgetColours[4] = { juce::Colour(0xFF00CAFF),
                                            juce::Colour(0xFF4FFF00),
                                            juce::Colour(0xFFFF9F00),
                                            juce::Colour(0xFFD0011B) };

  juce::Typeface::Ptr robotoLight, robotoRegular, robotoMedium, robotoBold;

  float sliderBarSize = 8.0f;

  LaF();

  juce::Typeface::Ptr getTypefaceForFont(const juce::Font& f) override;

  juce::Font getLabelFont(juce::Label& label) override;

  juce::Font getPopupMenuFont() override;

  juce::Font getTextButtonFont(juce::TextButton& button, int height) override;

  juce::Font getAlertWindowMessageFont() override;

  juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override;

  void drawLabel(juce::Graphics& g, juce::Label& label) override;

  void drawCornerResizer(juce::Graphics& g,
                         int w,
                         int h,
                         bool isMouseOver,
                         bool isMouseDragging) override;

  void fillTextEditorBackground(juce::Graphics& g,
                                int width,
                                int height,
                                juce::TextEditor& textEditor) override;

  void drawTextEditorOutline(juce::Graphics& g,
                             int width,
                             int height,
                             juce::TextEditor& textEditor) override;

  void drawTableHeaderBackground(juce::Graphics& g,
                                 juce::TableHeaderComponent& header) override;

  void drawTableHeaderColumn(juce::Graphics& g,
                             juce::TableHeaderComponent& header,
                             const juce::String& columnName,
                             int columnId,
                             int width,
                             int height,
                             bool isMouseOver,
                             bool isMouseDown,
                             int columnFlags) override;

  void drawLinearSlider(juce::Graphics& g,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPos,
                        float minSliderPos,
                        float maxSliderPos,
                        const juce::Slider::SliderStyle style,
                        juce::Slider& slider) override;

  void drawLinearSliderBackground(juce::Graphics& g,
                                  int x,
                                  int y,
                                  int width,
                                  int height,
                                  float sliderPos,
                                  float minSliderPos,
                                  float maxSliderPos,
                                  const juce::Slider::SliderStyle style,
                                  juce::Slider& slider) override;

  void drawRotarySlider(juce::Graphics& g,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPos,
                        float rotaryStartAngle,
                        float rotaryEndAngle,
                        juce::Slider& slider) override;

  void drawRotarySliderDual(juce::Graphics& g,
                            int x,
                            int y,
                            int width,
                            int height,
                            float sliderPos,
                            float rotaryStartAngle,
                            float rotaryEndAngle,
                            juce::Slider& slider,
                            bool isDual);

  void drawLinearSliderThumb(juce::Graphics& g,
                             int x,
                             int y,
                             int width,
                             int height,
                             float sliderPos,
                             float minSliderPos,
                             float maxSliderPos,
                             const juce::Slider::SliderStyle style,
                             juce::Slider& slider) override;

  void drawRoundThumb(juce::Graphics& g,
                      const float centreX,
                      const float centreY,
                      const float diameter,
                      const juce::Colour& colour,
                      float outlineThickness);

  juce::Button* createSliderButton(juce::Slider&,
                                   const bool isIncrement) override;

  void drawButtonBackground(juce::Graphics& g,
                            juce::Button& button,
                            const juce::Colour& backgroundColour,
                            bool isMouseOverButton,
                            bool isButtonDown) override;

  void drawButtonText(juce::Graphics& g,
                      juce::TextButton& button,
                      bool isMouseOverButton,
                      bool isButtonDown) override;

  void drawToggleButton(juce::Graphics& g,
                        juce::ToggleButton& button,
                        bool isMouseOverButton,
                        bool isButtonDown) override;

  void drawTickBox(juce::Graphics& g,
                   juce::Component& component,
                   float x,
                   float y,
                   float w,
                   float h,
                   bool ticked,
                   bool isEnabled,
                   bool isMouseOverButton,
                   bool isButtonDown) override;

  juce::Path getTickShape(const float height) override;

  void drawGroupComponentOutline(juce::Graphics& g,
                                 int width,
                                 int height,
                                 const juce::String& text,
                                 const juce::Justification& position,
                                 juce::GroupComponent& group) override;

  void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;

  void drawComboBox(juce::Graphics& g,
                    int width,
                    int height,
                    bool isButtonDown,
                    int buttonX,
                    int buttonY,
                    int buttonW,
                    int buttonH,
                    juce::ComboBox& box) override;

  void drawPopupMenuSectionHeader(juce::Graphics& g,
                                  const juce::Rectangle<int>& area,
                                  const juce::String& sectionName) override;

  void drawPopupMenuItem(juce::Graphics& g,
                         const juce::Rectangle<int>& area,
                         const bool isSeparator,
                         const bool isActive,
                         const bool isHighlighted,
                         const bool isTicked,
                         const bool hasSubMenu,
                         const juce::String& text,
                         const juce::String& shortcutKeyText,
                         const juce::Drawable* icon,
                         const juce::Colour* const textColourToUse) override;

  void drawCallOutBoxBackground(juce::CallOutBox& box,
                                juce::Graphics& g,
                                const juce::Path& path,
                                juce::Image& cachedImage) override;
};

} // namespace iem
