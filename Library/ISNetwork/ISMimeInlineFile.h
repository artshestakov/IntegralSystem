#pragma once
//-----------------------------------------------------------------------------
#include "isnetwork_global.h"
#include "ISMimeFile.h"
//-----------------------------------------------------------------------------
class ISNETWORK_EXPORT ISMimeInlineFile : public ISMimeFile
{
public:
    ISMimeInlineFile(QFile *f);
    ~ISMimeInlineFile();

protected:
    virtual void prepare();
};
//-----------------------------------------------------------------------------
