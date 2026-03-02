#pragma once

class IAnimable
{
protected:
	struct SignalKey;

public:
	virtual ~IAnimable() = default;
	utils::Signal_public<void(), SignalKey> sig_onAnimationFinished;
	utils::Signal_public<void(), SignalKey> sig_onAnimationStarted;
};