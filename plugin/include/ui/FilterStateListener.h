#pragma once
#include "CtmFilter.h"

class FilterStateListener
{
public:
    virtual void notify(CtmFilter*) = 0;
};