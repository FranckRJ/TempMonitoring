#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <Arduino.h>

class LoopScheduler
{
public:
    using TimeType = decltype(millis());

public:
    explicit LoopScheduler(TimeType loopCycleDurationMs);

    void init();

    [[nodiscard]] TimeType currentLoopCycleDurationMs() const;

    void waitForEndOfLoopCycle();

private:
    const TimeType _loopCycleDurationMs;
    TimeType _loopCycleStartTimeMs = 0;
};

#endif // SCHEDULER_HPP
