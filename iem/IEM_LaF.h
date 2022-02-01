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
using namespace juce;

class LaF : public LookAndFeel_V4
{
public:
  const Colour ClBackground = Colour(0xFF2D2D2D);
  const Colour ClFace = Colour(0xFFD8D8D8);
  const Colour ClFaceShadow = Colour(0XFF272727);
  const Colour ClFaceShadowOutline = Colour(0xFF212121);
  const Colour ClFaceShadowOutlineActive = Colour(0xFF7C7C7C);
  const Colour ClRotSliderArrow = Colour(0xFF4A4A4A);
  const Colour ClRotSliderArrowShadow = Colour(0x445D5D5D);
  const Colour ClSliderFace = Colour(0xFF191919);
  const Colour ClText = Colour(0xFFFFFFFF);
  const Colour ClTextTextboxbg = Colour(0xFF000000);
  const Colour ClSeperator = Colour(0xFF979797);
  const Colour ClWidgetColours[4] = { Colour(0xFF00CAFF),
                                      Colour(0xFF4FFF00),
                                      Colour(0xFFFF9F00),
                                      Colour(0xFFD0011B) };

  Typeface::Ptr robotoLight, robotoRegular, robotoMedium, robotoBold;

  float sliderBarSize = 8.0f;

  LaF();

  Typeface::Ptr getTypefaceForFont(const Font& f) override;

  Font getLabelFont(Label& label) override;

  Font getPopupMenuFont() override;

  Font getTextButtonFont(TextButton& button, int height) override;

  Font getAlertWindowMessageFont() override;

  Slider::SliderLayout getSliderLayout(Slider& slider) override;

  void drawLabel(Graphics& g, Label& label) override;

  void drawCornerResizer(Graphics& g,
                         int w,
                         int h,
                         bool isMouseOver,
                         bool isMouseDragging) override;

  void fillTextEditorBackground(Graphics& g,
                                int width,
                                int height,
                                TextEditor& textEditor) override;

  void drawTextEditorOutline(Graphics& g,
                             int width,
                             int height,
                             TextEditor& textEditor) override;

  void drawTableHeaderBackground(Graphics& g,
                                 TableHeaderComponent& header) override;

  void drawTableHeaderColumn(Graphics& g,
                             TableHeaderComponent& header,
                             const String& columnName,
                             int /*columnId*/,
                             int width,
                             int height,
                             bool isMouseOver,
                             bool isMouseDown,
                             int columnFlags) override;

  void drawLinearSlider(Graphics& g,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPos,
                        float minSliderPos,
                        float maxSliderPos,
                        const Slider::SliderStyle style,
                        Slider& slider) override;

  void drawLinearSliderBackground(Graphics& g,
                                  int x,
                                  int y,
                                  int width,
                                  int height,
                                  float sliderPos,
                                  float minSliderPos,
                                  float maxSliderPos,
                                  const Slider::SliderStyle style,
                                  Slider& slider) override;

  void drawRotarySlider(Graphics& g,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPos,
                        float rotaryStartAngle,
                        float rotaryEndAngle,
                        Slider& slider) override;

  void drawRotarySliderDual(Graphics& g,
                            int x,
                            int y,
                            int width,
                            int height,
                            float sliderPos,
                            float rotaryStartAngle,
                            float rotaryEndAngle,
                            Slider& slider,
                            bool isDual);

  void drawLinearSliderThumb(Graphics& g,
                             int x,
                             int y,
                             int width,
                             int height,
                             float sliderPos,
                             float minSliderPos,
                             float maxSliderPos,
                             const Slider::SliderStyle style,
                             Slider& slider) override;

  void drawRoundThumb(Graphics& g,
                      const float centreX,
                      const float centreY,
                      const float diameter,
                      const Colour& colour,
                      float outlineThickness);

  Button* createSliderButton(Slider&, const bool isIncrement) override;

  void drawButtonBackground(Graphics& g,
                            Button& button,
                            const Colour& backgroundColour,
                            bool isMouseOverButton,
                            bool isButtonDown) override;

  void drawButtonText(Graphics& g,
                      TextButton& button,
                      bool isMouseOverButton,
                      bool isButtonDown) override;

  void drawToggleButton(Graphics& g,
                        ToggleButton& button,
                        bool isMouseOverButton,
                        bool isButtonDown) override;

  void drawTickBox(Graphics& g,
                   Component& component,
                   float x,
                   float y,
                   float w,
                   float h,
                   bool ticked,
                   bool isEnabled,
                   bool isMouseOverButton,
                   bool isButtonDown) override;

  Path getTickShape(const float height) override;

  void drawGroupComponentOutline(Graphics& g,
                                 int width,
                                 int height,
                                 const String& text,
                                 const Justification& position,
                                 GroupComponent& group) override;

  void positionComboBoxText(ComboBox& box, Label& label) override;

  void drawComboBox(Graphics& g,
                    int width,
                    int height,
                    bool isButtonDown,
                    int buttonX,
                    int buttonY,
                    int buttonW,
                    int buttonH,
                    ComboBox& box) override;

  void drawPopupMenuSectionHeader(Graphics& g,
                                  const Rectangle<int>& area,
                                  const String& sectionName) override;

  void drawPopupMenuItem(Graphics& g,
                         const Rectangle<int>& area,
                         const bool isSeparator,
                         const bool isActive,
                         const bool isHighlighted,
                         const bool isTicked,
                         const bool hasSubMenu,
                         const String& text,
                         const String& shortcutKeyText,
                         const Drawable* icon,
                         const Colour* const textColourToUse) override;

  void drawCallOutBoxBackground(CallOutBox& box,
                                Graphics& g,
                                const Path& path,
                                Image& cachedImage) override;
};
