#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMimeFile.h"
//-----------------------------------------------------------------------------
class ISMimeInlineFile : public ISMimeFile
{
public:
    ISMimeInlineFile(QFile *f);
    ~ISMimeInlineFile();

protected:
    virtual void prepare();
};
//-----------------------------------------------------------------------------
