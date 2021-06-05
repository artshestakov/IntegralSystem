#pragma once
#ifndef _ISSTACKWALKER_H_INCLUDED
#define _ISSTACKWALKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StackWalker.h"
#include <string>
//-----------------------------------------------------------------------------
class ISStackWalker : public StackWalker
{
public:
    ISStackWalker();
    ~ISStackWalker();

    const std::string& GetStack() const;

protected:
    void OnOutput(LPCSTR szText) override;

private:
    std::string String;
};
//-----------------------------------------------------------------------------
#endif
