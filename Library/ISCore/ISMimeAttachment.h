#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMimeFile.h"
//-----------------------------------------------------------------------------
class ISMimeAttachment : public ISMimeFile
{
public:
    ISMimeAttachment(QFile* file);
    ISMimeAttachment(const QByteArray& stream, const QString& fileName);
    ~ISMimeAttachment();

protected:
    virtual void prepare();
};
//-----------------------------------------------------------------------------
