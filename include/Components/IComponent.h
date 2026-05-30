#pragma once

class IComponent
{
protected:
	struct SignalKey;

public:
	virtual ~IComponent()
	{
		utils::Access<SignalKey>(sig_onComponentDestroyed).Emit();
	}

	virtual utils::unique_ref<IComponent> Clone() = 0;

public:
	utils::Signal_public<void(), SignalKey> sig_onComponentDestroyed;
};