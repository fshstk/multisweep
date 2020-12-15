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
#include "../../resources/Conversions.h"
#include "../../resources/Quaternion.h"
#include <JuceHeader.h>

class SpherePannerBackground : public Component
{

public:
  SpherePannerBackground()
    : Component()
  {
    setBufferedToImage(true);
  };

  ~SpherePannerBackground(){};

  void resized() override
  {
    const Rectangle<float> sphere(getLocalBounds().reduced(10, 10).toFloat());

    radius = 0.5f * jmin(sphere.getWidth(), sphere.getHeight());
    centre = getLocalBounds().getCentre();
    sphereArea.setBounds(0, 0, 2 * radius, 2 * radius);
    sphereArea.setCentre(centre.toFloat());
  };

  void paint(Graphics& g) override
  {
    const Rectangle<float> bounds = getLocalBounds().toFloat();
    const float centreX = bounds.getCentreX();
    const float centreY = bounds.getCentreY();

    g.setColour(Colours::white);
    g.drawEllipse(
      centreX - radius, centreY - radius, 2.0f * radius, 2.0f * radius, 1.0f);

    g.setFont(getLookAndFeel().getTypefaceForFont(Font(12.0f, 1)));
    g.setFont(12.0f);
    g.drawText("FRONT",
               centreX - 15,
               centreY - radius - 12,
               30,
               12,
               Justification::centred);
    g.drawText(
      "BACK", centreX - 15, centreY + radius, 30, 12, Justification::centred);
    g.drawFittedText("L\nE\nF\nT",
                     sphereArea.getX() - 10,
                     centreY - 40,
                     10,
                     80,
                     Justification::centred,
                     4);
    g.drawFittedText("R\nI\nG\nH\nT",
                     sphereArea.getRight(),
                     centreY - 40,
                     10,
                     80,
                     Justification::centred,
                     5);

    g.setColour(Colours::steelblue.withMultipliedAlpha(0.2f));
    Path circles;

    for (int deg = 75; deg >= 0; deg -= 15) {
      float rCirc;
      if (!linearElevation)
        rCirc = radius * std::cos(Conversions<float>::degreesToRadians(deg));
      else
        rCirc = radius * (90 - deg) / 90;
      circles.addEllipse(
        centreX - rCirc, centreY - rCirc, 2.0f * rCirc, 2.0f * rCirc);
      g.fillPath(circles);
    }

    g.setColour(Colours::steelblue.withMultipliedAlpha(0.7f));
    g.strokePath(circles, PathStrokeType(.5f));

    ColourGradient gradient(Colours::black.withMultipliedAlpha(0.7f),
                            centreX,
                            centreY,
                            Colours::black.withMultipliedAlpha(0.1f),
                            0,
                            0,
                            true);
    g.setGradientFill(gradient);

    Path line;
    line.startNewSubPath(centreX, centreY - radius);
    line.lineTo(centreX, centreY + radius);

    Path path;
    path.addPath(line);
    path.addPath(line,
                 AffineTransform().rotation(
                   0.25f * MathConstants<float>::pi, centreX, centreY));
    path.addPath(line,
                 AffineTransform().rotation(
                   0.5f * MathConstants<float>::pi, centreX, centreY));
    path.addPath(line,
                 AffineTransform().rotation(
                   0.75f * MathConstants<float>::pi, centreX, centreY));

    g.strokePath(path, PathStrokeType(0.5f));
  }
  void setElevationStyle(bool linear) { linearElevation = linear; };

private:
  float radius = 1.0f;
  Rectangle<float> sphereArea;
  Point<int> centre;
  bool linearElevation = false;
};

class SpherePanner : public Component
{
public:
  SpherePanner()
    : Component()
  {
    setBufferedToImage(true);

    addAndMakeVisible(background);
    background.addMouseListener(this, false); // could this be risky?
  };

  ~SpherePanner(){};

  class Listener
  {
  public:
    virtual ~Listener() {}
    virtual void mouseWheelOnSpherePannerMoved(
      SpherePanner* sphere,
      const MouseEvent& event,
      const MouseWheelDetails& wheel){};
  };

  class Element
  {
  public:
    Element() {}
    virtual ~Element() {}

    virtual void startMovement(){};
    virtual void moveElement(const MouseEvent& event,
                             Point<int> centre,
                             float radius,
                             bool upBeforeDrag,
                             bool linearElevation,
                             bool rightClick = false) = 0;
    virtual void stopMovement(){};

    /**
     Get cartesian coordinates
     */
    virtual Vector3D<float> getCoordinates() = 0;

    void setActive(bool isActive) { active = isActive; }
    bool isActive() { return active; }

    void setColour(Colour newColour) { colour = newColour; }
    void setTextColour(Colour newColour) { textColour = newColour; }
    Colour getColour() { return colour; }
    Colour getTextColour() { return textColour; }

    void setLabel(String newLabel) { label = newLabel; }

    void setGrabPriority(int newPriority) { grabPriority = newPriority; }
    int getGrabPriority() { return grabPriority; }
    void setGrabRadius(float newRadius) { grabRadius = newRadius; }
    float getGrabRadius() { return grabRadius; }

    String getLabel() { return label; };

  private:
    bool active = true;

    float grabRadius = 0.123f;
    int grabPriority = 0;

    Colour colour = Colours::white;
    Colour textColour = Colours::black;
    String label = "";
  };

  class StandardElement : public Element
  {
  public:
    StandardElement()
      : Element()
    {}

    void moveElement(const MouseEvent& event,
                     Point<int> centre,
                     float radius,
                     bool upBeforeDrag,
                     bool linearElevation,
                     bool rightClick) override
    {
      Point<int> pos = event.getPosition();
      const float azimuth = -1.0f * centre.getAngleToPoint(pos);
      float r = centre.getDistanceFrom(pos) / radius;
      if (r > 1.0f) {
        r = 1.0f / r;
        upBeforeDrag = !upBeforeDrag;
      }

      if (linearElevation)
        r = std::sin(r * MathConstants<float>::halfPi);

      float elevation = std::acos(r);
      if (!upBeforeDrag)
        elevation *= -1.0f;

      position = Conversions<float>::sphericalToCartesian(azimuth, elevation);
    }

    /*
     Get cartesian coordinates
     */
    Vector3D<float> getCoordinates() override { return position; };

    bool setCoordinates(
      Vector3D<float>
        newPosition) // is true when position is updated (use it for repainting)
    {
      if (position.x != newPosition.x || position.y != newPosition.y ||
          position.z != newPosition.z) {
        position = newPosition;
        return true;
      }
      return false;
    }

  private:
    Vector3D<float> position;
  };

  class AzimuthElevationParameterElement : public Element
  {
  public:
    AzimuthElevationParameterElement(
      AudioProcessorParameter& azimuthParameter,
      NormalisableRange<float> azimuthParameterRange,
      AudioProcessorParameter& elevationParameter,
      NormalisableRange<float> elevationParameterRange)
      : Element()
      , azimuth(azimuthParameter)
      , azimuthRange(azimuthParameterRange)
      , elevation(elevationParameter)
      , elevationRange(elevationParameterRange)
    {}

    void startMovement() override
    {
      azimuth.beginChangeGesture();
      elevation.beginChangeGesture();
    };

    void moveElement(const MouseEvent& event,
                     Point<int> centre,
                     float radius,
                     bool upBeforeDrag,
                     bool linearElevation,
                     bool rightClick) override
    {
      Point<int> pos = event.getPosition();
      const float azi = -1.0f * centre.getAngleToPoint(pos);
      const float azimuthInDegrees{ Conversions<float>::radiansToDegrees(azi) };

      if (!rightClick) {
        float r = centre.getDistanceFrom(pos) / radius;

        if (r > 1.0f) {
          r = 1.0f / r;
          upBeforeDrag = !upBeforeDrag;
        }

        if (linearElevation)
          r = std::sin(r * MathConstants<float>::halfPi);
        float ele = std::acos(r);
        if (!upBeforeDrag)
          ele *= -1.0f;

        const float elevationInDegrees{ Conversions<float>::radiansToDegrees(
          ele) };

        elevation.setValueNotifyingHost(
          elevationRange.convertTo0to1(elevationInDegrees));
      }

      azimuth.setValueNotifyingHost(
        azimuthRange.convertTo0to1(azimuthInDegrees));
    }

    void stopMovement() override
    {
      azimuth.endChangeGesture();
      elevation.endChangeGesture();
    };

    const float getAzimuthInRadians()
    {
      const float azimuthInDegrees{ azimuthRange.convertFrom0to1(
        azimuth.getValue()) };
      return Conversions<float>::degreesToRadians(azimuthInDegrees);
    }

    const float getElevationInRadians()
    {
      const float elevationInDegrees{ elevationRange.convertFrom0to1(
        elevation.getValue()) };
      return Conversions<float>::degreesToRadians(elevationInDegrees);
    }

    /*
     Get cartesian coordinates
     */
    Vector3D<float> getCoordinates() override
    {
      return Conversions<float>::sphericalToCartesian(getAzimuthInRadians(),
                                                      getElevationInRadians());
    };

  private:
    AudioProcessorParameter& azimuth;
    NormalisableRange<float> azimuthRange;
    AudioProcessorParameter& elevation;
    NormalisableRange<float> elevationRange;
  };

  class RollWidthParameterElement : public Element
  {
  public:
    RollWidthParameterElement(AzimuthElevationParameterElement& center,
                              AudioProcessorParameter& rollParameter,
                              NormalisableRange<float> rollParameterRange,
                              AudioProcessorParameter& widthParameter,
                              NormalisableRange<float> widthParameterRange)
      : Element()
      , centerElement(center)
      , roll(rollParameter)
      , rollRange(rollParameterRange)
      , width(widthParameter)
      , widthRange(widthParameterRange)
    {}

    void startMovement() override
    {
      roll.beginChangeGesture();
      width.beginChangeGesture();
    };

    void moveElement(const MouseEvent& event,
                     Point<int> centre,
                     float radius,
                     bool upBeforeDrag,
                     bool linearElevation,
                     bool rightClick) override
    {
      Point<int> pos = event.getPosition();
      const float azi = -1.0f * centre.getAngleToPoint(pos);
      float r = centre.getDistanceFrom(pos) / radius;
      if (r > 1.0f) {
        r = 1.0f / r;
        upBeforeDrag = !upBeforeDrag;
      }

      if (linearElevation)
        r = std::sin(r * MathConstants<float>::halfPi);

      float ele = std::acos(r);
      if (!upBeforeDrag)
        ele *= -1.0f;

      Vector3D<float> posXYZ(
        Conversions<float>::sphericalToCartesian(azi, ele));

      // ==== calculate width
      Vector3D<float> dPos = posXYZ - centerElement.getCoordinates();
      const float alpha = 4.0f * std::asin(dPos.length() / 2.0f);
      width.setValueNotifyingHost(
        widthRange.convertTo0to1(Conversions<float>::radiansToDegrees(alpha)));

      // ==== calculate roll
      iem::Quaternion<float> quat;
      float ypr[3];
      ypr[0] = centerElement.getAzimuthInRadians();
      ypr[1] = -centerElement.getElevationInRadians(); // pitch
      ypr[2] = 0.0f;

      quat.fromYPR(ypr);
      quat.conjugate();

      const auto rotated = quat.rotateVector(posXYZ);

      float rollInRadians = atan2(rotated.z, rotated.y);
      if (isMirrored)
        rollInRadians = atan2(-rotated.z, -rotated.y);

      roll.setValueNotifyingHost(rollRange.convertTo0to1(
        Conversions<float>::radiansToDegrees(rollInRadians)));
    }

    void stopMovement() override
    {
      roll.endChangeGesture();
      width.endChangeGesture();
    };

    /*
     Get cartesian coordinates
     */
    Vector3D<float> getCoordinates() override
    {
      float ypr[3];
      ypr[0] = centerElement.getAzimuthInRadians();
      ypr[1] = -centerElement.getElevationInRadians(); // pitch
      ypr[2] = Conversions<float>::degreesToRadians(
        rollRange.convertFrom0to1(roll.getValue()));

      // updating not active params
      iem::Quaternion<float> quat;
      quat.fromYPR(ypr);

      const float widthInRadiansQuarter(
        Conversions<float>::degreesToRadians(
          widthRange.convertFrom0to1(width.getValue())) /
        4.0f);

      iem::Quaternion<float> quatLRot{ iem::Quaternion<float>(
        cos(widthInRadiansQuarter), 0.0f, 0.0f, sin(widthInRadiansQuarter)) };
      if (isMirrored)
        quatLRot.conjugate();

      iem::Quaternion<float> quatL = quat * quatLRot;

      return quatL.getCartesian();
    };

    void setMirrored(bool mirrored) { isMirrored = mirrored; }

  private:
    AzimuthElevationParameterElement& centerElement;
    AudioProcessorParameter& roll;
    NormalisableRange<float> rollRange;
    AudioProcessorParameter& width;
    NormalisableRange<float> widthRange;
    bool isMirrored = false;
  };

  void resized() override
  {
    background.setBounds(getLocalBounds());
    const Rectangle<float> sphere(getLocalBounds().reduced(10, 10).toFloat());

    radius = 0.5f * jmin(sphere.getWidth(), sphere.getHeight());
    centre = getLocalBounds().getCentre();
    sphereArea.setBounds(0, 0, 2 * radius, 2 * radius);
    sphereArea.setCentre(centre.toFloat());
  }

  void paintOverChildren(Graphics& g) override
  {
    const Rectangle<float> bounds = getLocalBounds().toFloat();
    const float centreX = bounds.getCentreX();
    const float centreY = bounds.getCentreY();

    g.setFont(getLookAndFeel().getTypefaceForFont(Font(12.0f, 1)));

    const int size = elements.size();
    for (int i = 0; i < size; ++i) {
      SpherePanner::Element* handle = elements.getUnchecked(i);

      Vector3D<float> pos = handle->getCoordinates();
      const bool isUp = pos.z >= -0.0f;

      const float diam = 15.0f + 4.0f * pos.z;
      const Colour colour =
        handle->isActive() ? handle->getColour() : Colours::grey;
      g.setColour(colour);

      if (linearElevation) {
        const float r = sqrt(pos.y * pos.y + pos.x * pos.x);
        const float factor = std::asin(r) / r / MathConstants<float>::halfPi;
        pos *= factor;
      }

      const Rectangle<float> circleArea(centreX - pos.y * radius - diam / 2,
                                        centreY - pos.x * radius - diam / 2,
                                        diam,
                                        diam);
      Path panPos;

      panPos.addEllipse(circleArea);
      g.strokePath(panPos, PathStrokeType(1.0f));

      if (i == activeElem) {
        g.setColour(colour.withAlpha(0.8f));
        g.drawEllipse(
          circleArea.withSizeKeepingCentre(1.3f * diam, 1.3f * diam), 0.9f);
      }

      g.setColour(colour.withAlpha(isUp ? 1.0f : 0.3f));
      g.fillPath(panPos);
      g.setColour(isUp ? handle->getTextColour() : colour);

      g.setFont(isUp ? 15.0f : 10.0f);
      g.drawText(handle->getLabel(),
                 circleArea.toNearestInt(),
                 Justification::centred,
                 false);
    }
  };

  void mouseWheelMove(const MouseEvent& event,
                      const MouseWheelDetails& wheel) override
  {
    for (int i = listeners.size(); --i >= 0;)
      listeners.getUnchecked(i)->mouseWheelOnSpherePannerMoved(
        this, event, wheel);
  }

  void mouseMove(const MouseEvent& event) override
  {
    const int oldActiveElem = activeElem;
    activeElem = -1;

    const int nElem = elements.size();

    if (nElem > 0) {
      const auto centre = getLocalBounds().getCentre();
      const Point<int> dif = centre - event.getPosition();
      const Point<float> mousePos(dif.y / radius,
                                  dif.x / radius); // scale and swap xy

      int highestPriority = -1;
      int smallestDist = 123456789; // basically infinity

      for (int i = 0; i < nElem; ++i) {
        Element* handle(elements.getUnchecked(i));
        auto elementPosition = handle->getCoordinates();

        if (linearElevation) {
          const float r = sqrt(elementPosition.y * elementPosition.y +
                               elementPosition.x * elementPosition.x);
          const float factor = std::asin(r) / r / MathConstants<float>::halfPi;
          elementPosition *= factor;
        }

        const Point<float> connection(mousePos.x - elementPosition.x,
                                      mousePos.y - elementPosition.y);
        const auto distance = connection.getDistanceFromOrigin();

        if (distance <= handle->getGrabRadius()) {
          if (handle->getGrabPriority() > highestPriority) {
            activeElem = i;
            highestPriority = handle->getGrabPriority();
            smallestDist = distance;
          } else if (handle->getGrabPriority() == highestPriority &&
                     distance < smallestDist) {
            activeElem = i;
            smallestDist = distance;
          }
        }
      }
    }

    if (activeElem != -1)
      activeElemWasUpBeforeDrag =
        elements.getUnchecked(activeElem)->getCoordinates().z >= 0.0f;
    if (oldActiveElem != activeElem)
      repaint();
  }

  void mouseDrag(const MouseEvent& event) override
  {
    const bool rightClick = event.mods.isRightButtonDown();
    if (activeElem != -1) {
      elements.getUnchecked(activeElem)
        ->moveElement(event,
                      centre,
                      radius,
                      activeElemWasUpBeforeDrag,
                      linearElevation,
                      rightClick);
      repaint();
    }
  }

  void mouseDown(const MouseEvent& event) override
  {
    if (activeElem != -1)
      elements.getUnchecked(activeElem)->startMovement();
  }

  void mouseUp(const MouseEvent& event) override
  {
    if (activeElem != -1)
      elements.getUnchecked(activeElem)->stopMovement();
  }

  void mouseDoubleClick(const MouseEvent& event) override
  {
    setElevationStyle(!linearElevation);
    background.repaint();
    repaint();
  }

  void addListener(Listener* const listener)
  {
    jassert(listener != nullptr);
    if (listener != nullptr)
      listeners.add(listener);
  };

  void removeListener(Listener* const listener)
  {
    listeners.removeFirstMatchingValue(listener);
  };

  void addElement(Element* const element)
  {
    jassert(element != nullptr);
    if (element != nullptr)
      elements.addIfNotAlreadyThere(element);
  };

  void removeElement(Element* const element)
  {
    elements.removeFirstMatchingValue(element);
  };

  void setElevationStyle(bool linear)
  {
    linearElevation = linear;
    background.setElevationStyle(linear);
  };

private:
  float radius = 1.0f;
  Rectangle<float> sphereArea;
  Point<int> centre;
  int activeElem = -1;
  bool activeElemWasUpBeforeDrag;
  Array<Listener*> listeners;
  Array<Element*> elements;
  bool linearElevation = false;
  SpherePannerBackground background;
};
