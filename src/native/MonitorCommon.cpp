PhysicalMonitor::PhysicalMonitor (PhysicalMonitor&&) = default;

PhysicalMonitor::PhysicalMonitor (std::unique_ptr<PhysicalMonitor::Impl>&& impl) : impl (std::move (impl))
{
}

PhysicalMonitor::~PhysicalMonitor() = default;

const juce::String& PhysicalMonitor::getName()
{
    return impl->GetName();
}

int PhysicalMonitor::getBrightness()
{
    return impl->GetBrightness();
}

bool PhysicalMonitor::setBrightness (int brightness, bool force)
{
    return impl->SetBrightness (brightness, force);
}

bool PhysicalMonitor::setInput (Source source)
{
    if (auto currentInput = impl->getInput(); currentInput == source)
        return true;

    return impl->setInput ((int) source);
}

std::optional<PhysicalMonitor::Source> PhysicalMonitor::getInput()
{
    return impl->getInput();
}
