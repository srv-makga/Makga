#pragma once

#include <map>
#include <functional>

namespace core {
template<typename Identifier_t, typename Function_t>
class Dispatcher
{
public:
	Dispatcher()
	{
	}

	~Dispatcher()
	{
	}

public:
	bool Add(const Identifier_t& _identifier, const Function_t& _functor)
	{
		return m_functor.insert({ _identifier, _functor }).second;
	}

	const Function_t& Find(Identifier_t _identifier)
	{
		auto it = m_functor.find(_identifier);
		if (m_functor.end() == it)
		{
			return nullptr;
		}

		return it->second;
	}

	bool Remove(const Identifier_t _identifier)
	{
		auto it = m_functor.find(_identifier);
		if (m_functor.end() == it)
		{
			return false;
		}

		m_functor.erase(it);

		return true;
	}

	template<typename Actor, typename Args>
	bool Exec(Identifier_t _identifier, Actor _actor, Args _args)
	{
		auto iter = m_functor.find(_identifier);
		if (m_functor.end() == iter)
		{
			return false;
		}

		return iter->second(_actor, _args);

		/*Function_t functor;

		if (true == Find(_identifier, functor))
		{
			(_actor->*functor)(_args);
			return true;
		}

		return false;*/
	}

	//template<typename Actor, typename ...Args>
	//bool Exec(Identifier_t _identifier, Actor _actor, Args... args)
	//{
	//	auto iter = m_functor.find(_identifier);
	//	if (m_functor.end() == iter)
	//	{
	//		return false;
	//	}
	//	
	//	return iter->second(_actor, args...);
	//	/*Function_t functor;

	//	if (true == Find(_identifier, functor))
	//	{
	//		(_actor->*functor)(args...);
	//		return true;
	//	}

	//	return false;*/
	//}

private:
	bool Find(Identifier_t _identifier, Function_t& _functor) const
	{
		auto it = m_functor.find(_identifier);
		if (m_functor.end() == it)
		{
			return false;
		}

		_functor = (*it).second;

		return true;
	}

private:
	std::map<Identifier_t, Function_t> m_functor;
};
} // namespace core