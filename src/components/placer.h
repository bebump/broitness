#include <juce_gui_basics/juce_gui_basics.h>

#include <algorithm>

struct Placer {
 public:
  Placer(const juce::Rectangle<int>& bounds) : bounds_(bounds) {}

  void Place(juce::Component& component, int width, int height) {
    if (bounds_.getWidth() - cursor_x_ < width) {
      cursor_y_ += max_height_;
      max_height_ = 0;
    }
    component.setBounds(bounds_.reduced(cursor_x_, cursor_y_)
                            .withWidth(width)
                            .withHeight(height));
    cursor_x_ += width;
    max_height_ = std::max(max_height_, height);
  }

 private:
  const juce::Rectangle<int>& bounds_;
  int max_height_ = 0;
  int cursor_x_ = 0, cursor_y_ = 0;
};

namespace Placement {

}
