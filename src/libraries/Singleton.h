#pragma once

#include <type_traits>

// based on https://stackoverflow.com/a/34519373


// Seems unable to use it with QWidget-based classes, don't understand why exactly.
// Conflict is between Qt's ownership model (parent automatically destroys childs in dtor)
// and static allocation from this singleton.
template<typename T>
class Singleton
{
  protected:
    Singleton() noexcept = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    virtual ~Singleton() = default; // to silence base class Singleton<T> has a non-virtual destructor [-Weffc++]

  public:
    static T& get() noexcept(std::is_nothrow_constructible<T>::value) {
        static T instance{};
        return instance;
    }
};
