#ifndef KENGINE_SINGLETON_HPP
#define KENGINE_SINGLETON_HPP

namespace kengine {

template<typename T>
class Singleton {
public:
	static T& get() {
		static T instance;
		return instance;
	}

	Singleton(Singleton const&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton const&) = delete;
	Singleton& operator=(Singleton&&) = delete;

	Singleton() = default;
	virtual ~Singleton() = default;
};

} // namespace kengine

#endif