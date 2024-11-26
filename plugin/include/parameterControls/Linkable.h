#pragma once

template <typename T>
class Linkable
{
public:
    virtual void link(const T*) = 0;
    virtual void unlink(const T*) = 0;
};

template<typename T>
concept linkable = std::derived_from<T, Linkable<T>>;