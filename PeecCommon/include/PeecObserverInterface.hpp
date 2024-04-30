#pragma once

#include <memory>

namespace Utils
{
	class Observer
	{
	public:
		virtual void update() = 0;
		virtual ~Observer() = default;
	};

	class Subject
	{
	public:
		virtual void addObserver(std::shared_ptr<Observer> _observer) = 0;
		virtual void removeObserver(std::shared_ptr<Observer> _observer) = 0;
		virtual void notifyObservers() = 0;
		virtual ~Subject() = default;
	};
}