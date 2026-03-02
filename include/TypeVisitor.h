#pragma once
#include <IteratingAction.h>

template <typename Functor, typename T, typename... Args>
inline constexpr bool HasFilterAction = requires(Functor f, Args... args)
{
	{ f.template Filter<T>(args...) } -> utils::same<IteratingAction>;
};

template <typename... Types>
class TypeVisitor
{
public:
	static void Visit(...)
	{
	}
};

template <typename FirstType, typename... Types>
class TypeVisitor<FirstType, Types...>
{
public:
	template <typename Functor, typename... Args>
	requires requires (Functor f, Args&&... args)
	{
		{ f.template operator()<FirstType>(std::forward<Args>(args)...) } -> utils::same<IteratingAction>;
	}
	static void Visit(Functor& i_functor, Args&&... args)
	{
		IteratingAction action = IteratingAction::Continue;
		if constexpr (HasFilterAction<Functor, FirstType, Args...>)
		{
			action = i_functor.template Filter<FirstType>(std::forward<Args>(args)...);
		}
		if (action == IteratingAction::Stop || (action != IteratingAction::Skip &&
			i_functor.template operator()<FirstType>(std::forward<Args>(args)...) == IteratingAction::Stop))
		{
			return;
		}
		TypeVisitor<Types...>::Visit(i_functor, std::forward<Args>(args)...);
	}
};