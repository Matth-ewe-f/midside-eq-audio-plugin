#pragma once

// forward declaration
class CtmFilter;

class FilterStateListener
{
public:
    virtual void notify(CtmFilter*) = 0;
};