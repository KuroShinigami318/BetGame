#pragma once

#include "Components/IContainer.h"

namespace utils
{
class any;
}
enum class IWindowErrorCode : uint8_t;
struct ClosedByExitFlowTag {};

class IWindow : virtual public IContainer
{
public:
	using IRecursiveControlError = utils::Error<utils::IRecursiveControl::ErrorCode>;
	using IWindowError = utils::Error<IWindowErrorCode, IRecursiveControlError>;
	using ResultT = utils::any;
	using OpenResultT = utils::Result<const ResultT*, IWindowError>;
	
public:
	virtual OpenResultT Open() = 0;
	virtual void Close(const ResultT&) = 0;
	virtual bool IsOpened() const = 0;
};