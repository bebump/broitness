#pragma once

#include "Observable.h"

#include <chrono>
#include <deque>
#include <format>

namespace bbmp
{

struct LogMessage
{
    std::chrono::time_point<std::chrono::system_clock> time;
    juce::String message;
};

class Logger : public bbmp::Observable<Logger>
{
public:
    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    juce::String getText() const
    {
        juce::StringArray arr;

        for (const auto& m : messages)
            arr.add (juce::String { std::format ("{:%H:%M:%S}", m.time) } + ": " + m.message);

        return arr.joinIntoString ("\n");
    }

    void writeLine (juce::StringRef message)
    {
        messages.push_back ({ std::chrono::system_clock::now(), message });
        callObservers();
    }

private:
    Logger() = default;

    void eraseOldMessages()
    {
        while (messages.size() > keptMessages)
            messages.pop_front();
    }

    std::deque<LogMessage> messages;
    size_t keptMessages = 100;
};

} // namespace bbmp
