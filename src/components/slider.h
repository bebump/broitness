#include <juce_gui_basics/juce_gui_basics.h>

class SliderWithFeedbackLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider& slider) override {
    auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);

    auto bounds =
        juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle =
        rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto toFeedbackAngle =
        rotaryStartAngle + static_cast<float>(feedback_value_) *
                               (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin(8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    // >>> DRAWING BACKGROUND ARC =============================================
    {
      juce::Path backgroundArc;
      backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                  arcRadius, arcRadius, 0.0f, rotaryStartAngle,
                                  rotaryEndAngle, true);

      g.setColour(outline);
      g.strokePath(backgroundArc,
                   juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));
    }
    // <<< DRAWING BACKGROUND ARC ---------------------------------------------

    // >>> DRAWING VALUE ARC ==================================================
    if (slider.isEnabled()) {
      juce::Path valueArc;
      valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                             arcRadius, arcRadius, 0.0f, rotaryStartAngle,
                             toAngle, true);

      g.setColour(fill);
      g.strokePath(valueArc,
                   juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));
    }
    // <<< DRAWING VALUE ARC --------------------------------------------------

    // >>> DRAWING FEEDBACK ARC ===============================================
    {
      juce::Path backgroundArc;
      backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                  arcRadius, arcRadius, 0.0f, rotaryStartAngle,
                                  toFeedbackAngle, true);

      g.setColour(juce::Colours::yellow);
      g.strokePath(
          backgroundArc,
          juce::PathStrokeType(lineW * 1.4f, juce::PathStrokeType::curved,
                               juce::PathStrokeType::rounded));

      g.setColour(juce::Colours::orange);
      g.strokePath(
          backgroundArc,
          juce::PathStrokeType(lineW * 1.1f, juce::PathStrokeType::curved,
                               juce::PathStrokeType::rounded));
    }
    // <<< DRAWING FEEDBACK ARC -----------------------------------------------

    auto thumbWidth = lineW * 2.0f;
    juce::Point<float> thumbPoint(
        bounds.getCentreX() +
            arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() +
            arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));

    g.setColour(slider.findColour(juce::Slider::thumbColourId));
    g.fillEllipse(
        juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
  }

  void SetFeedbackValue(double value) { feedback_value_ = value; }

 private:
  double feedback_value_ = 0.0f;
};

// Going with composition over inheritance, because the LookAndFeel object must
// outlive the slider object. Hence it can't be a member of the slider.
class SliderWithFeedback {
 public:
  SliderWithFeedback()
      : slider_{juce::Slider::SliderStyle::RotaryVerticalDrag,
                juce::Slider::TextEntryBoxPosition::TextBoxBelow} {
    slider_.setLookAndFeel(&look_and_feel_);
  }

  juce::Slider& slider() { return slider_; }

  void SetFeedbackValue(double value) {
    look_and_feel_.SetFeedbackValue(value / slider_.getRange().getLength());
    slider_.repaint();
  }

 private:
  SliderWithFeedbackLookAndFeel look_and_feel_;
  juce::Slider slider_;

  friend class SliderWithFeedbackLookAndFeel;
};