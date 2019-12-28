#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISMimeFile.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMimeInlineFile : public ISMimeFile
{
public:
    ISMimeInlineFile(QFile *f);
    ~ISMimeInlineFile();

protected:
    virtual void prepare();
};
//-----------------------------------------------------------------------------
