#include "LoopScheduler.hpp"

#include <LowPower.h>

namespace
{
    void deepSleepFor8s(int times)
    {
        auto oldClkPr = CLKPR;
        CLKPR = 0x80;
        CLKPR = 0x08;
        for (int i = 0; i < times; ++i)
        {
            LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
        }
        CLKPR = 0x80;
        CLKPR = oldClkPr;
    }

    constexpr LoopScheduler::TimeType estimated8sSleepDuration = 8670;
} // namespace

LoopScheduler::LoopScheduler(TimeType loopCycleDurationMs) : _loopCycleDurationMs{loopCycleDurationMs}
{}

void LoopScheduler::init()
{
    _loopCycleStartTimeMs = millis();
}

LoopScheduler::TimeType LoopScheduler::currentLoopCycleDurationMs() const
{
    return millis() - _loopCycleStartTimeMs;
}

void LoopScheduler::waitForEndOfLoopCycle()
{
    const TimeType timeToWaitMs = _loopCycleDurationMs - (millis() - _loopCycleStartTimeMs);

    if (timeToWaitMs > 0)
    {
        const int numberOf8sToWait = static_cast<int>((timeToWaitMs / estimated8sSleepDuration));
        const TimeType remainingTimeToWait = timeToWaitMs % estimated8sSleepDuration;

        if (numberOf8sToWait > 0)
        {
            deepSleepFor8s(numberOf8sToWait);
        }
        if (remainingTimeToWait > 0)
        {
            delay(remainingTimeToWait);
        }
    }

    _loopCycleStartTimeMs = millis();
}
