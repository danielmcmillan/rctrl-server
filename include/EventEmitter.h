#pragma once
#include <functional>
#include <map>
#include <vector>
#include <mutex>
#include <algorithm>

namespace Rctrl
{
	/// Template class for an event emitter.
	///
	/// EventType is the type used to separate events into categories that can be listened to individually.
	///		It must be comparable with <, and be copyable and assignable.
	/// Event is the type for the payload of an event.
	template <typename EventType, typename Event>
	class EventEmitter
	{
	public:
		using CallbackFunction = std::function<void(EventType, const Event&)>;
		using ListenerID = unsigned int;

		EventEmitter() {}

		/// Add a listener for the specified event type.
		///
		/// @param eventType
		/// @param callback Function to be called when any event of type eventType occurs, passed the event type and payload as an argument.
		/// @return An identifier for the listener, passed to removeListener to unsubscribe from these events.
		ListenerID addListener(EventType eventType, CallbackFunction callback)
		{
			if (!callback)
			{
				throw std::invalid_argument("EventEmitter::addListener: The callback function provided is not valid.");
			}

			int listenerID = lastListenerID++;
			std::lock_guard<std::mutex> lock(listenersMutex);
			Listener listener = { listenerID, callback };
			listeners.emplace(eventType, listener);

			return listenerID;
		}

		/// Unsubscribes a listener from an event it is subscribed to.
		///
		/// @param id The ID for the listener that was returned by the addListener function.
		void removeListener(ListenerID id)
		{
			std::lock_guard<std::mutex> lock(listenersMutex);
			for (auto it = listeners.begin(); it != listeners.end();)
			{
				if (it->second.id == id)
				{
					it = listeners.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

	protected:
		~EventEmitter() {}

		/// Send an event to all of its listeners.
		///
		/// @param The type for this event.
		/// @param event The payload of the event.
		void emit(EventType eventType, const Event &event) const
		{
			std::vector<std::pair<EventType, Listener>> matchingListeners;

			// Copy the matching listeners into vector while listeners is locked
			{
				std::lock_guard<std::mutex> lock(listenersMutex);

				auto listenerRange = listeners.equal_range(eventType);
				matchingListeners.insert(matchingListeners.begin(), listenerRange.first, listenerRange.second);
			}

			// Call callback on matching listeners
			for (const auto &listener : matchingListeners)
			{
				listener.second.callback(eventType, event);
			}
		}

	private:
		struct Listener
		{
			int id;
			CallbackFunction callback;
		};

		mutable std::mutex listenersMutex;
		std::multimap<EventType, Listener> listeners;
		ListenerID lastListenerID = 0;
	};
}
