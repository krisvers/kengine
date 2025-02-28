#ifndef KENGINE_CORE_EVENT_HPP
#define KENGINE_CORE_EVENT_HPP

#include <kengine/types.hpp>
#include <kengine/singleton.hpp>
#include <kengine/core/logging.hpp>
#include <kengine/core/uuid.hpp>

#include <unordered_map>
#include <string>

namespace kengine::core {

struct IEventData {
	IEventData() = default;
	virtual ~IEventData() = default;
};

template<typename T>
struct EventData {
	EventData() = default;
	EventData(const T& data) : data(data) {}

	T data;
};

using EventCallback = bool(*)(IEventData*);

struct ITagData {
	ITagData() = default;
	virtual ~ITagData() = default;

	virtual const UUID& getUUID() const {
		static UUID uuid = UUID(0);
		return uuid;
	}
};

template<typename T>
struct UUIDTagData : public ITagData {
	const UUID& getUUID() const override { return getUUIDStatic(); }

	static const UUID& getUUIDStatic() {
		static UUID uuid = UUID();
		return uuid;
	}
};

template<typename T>
struct TagData : public UUIDTagData<TagData> {
	TagData() = default;
	TagData(const T& data) : data(data) {}

	operator T&() { return data; }
	operator const T&() const { return data; }

	T& operator=(const T& data) {
		this->data = data;
		return *this;
	}

	T data;
};

class EventSystem : public Singleton<EventSystem> {
public:
	void createEventCallback(std::string eventName, EventCallback callback) {
		if (m_eventCallbacks.find(eventName) != m_eventCallbacks.end()) {
			Logger::get().logf(LogSeverity::Warning, "Event {} already exists", eventName);
			return;
		}

		m_eventCallbacks[eventName] = callback;
	}

	void triggerEvent(std::string eventName, IEventData* data) {
		if (m_eventCallbacks.find(eventName) == m_eventCallbacks.end()) {
			Logger::get().logf(LogSeverity::Warning, "Event {} does not exist", eventName);
			return;
		}
		
		m_eventCallbacks[eventName](data);
	}

	template<typename T>
	void createTag(std::string tagName) {
		static_assert(std::is_base_of_v<UUIDTagData<T>, T>, "T must inherit from UUIDTagData");

		if (m_tagData.find(tagName) != m_tagData.end()) {
			Logger::get().logf(LogSeverity::Warning, "Tag {} already exists", tagName);
			return;
		}

		m_tagData[tagName] = new T();
	}

	template<typename T>
	void writeTag(std::string tagName, T const& data) {
		static_assert(std::is_base_of_v<UUIDTagData<T>, T>, "T must inherit from UUIDTagData");

		if (m_tagData.find(tagName) == m_tagData.end()) {
			Logger::get().logf(LogSeverity::Warning, "Tag {} does not exist", tagName);
			return;
		}

		*(static_cast<T*>(m_tagData[tagName])->data) = data;
	}

	template<typename T>
	T const& readTag(std::string tagName) {
		static_assert(std::is_base_of_v<UUIDTagData<T>, T>, "T must inherit from UUIDTagData");

		if (m_tagData.find(tagName) == m_tagData.end()) {
			Logger::get().logf(LogSeverity::Warning, "Tag {} does not exist", tagName);
			return T();
		}

		return *(static_cast<T*>(m_tagData[tagName])->data);
	}

private:
	std::unordered_map<std::string, EventCallback> m_eventCallbacks;
	std::unordered_map<std::string, IEventData*> m_eventData;
	std::unordered_map<std::string, ITagData*> m_tagData;
};

} // namespace kengine::core