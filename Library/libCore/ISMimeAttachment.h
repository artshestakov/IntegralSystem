#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMimePart.h"
#include "ISMimeFile.h"
//-----------------------------------------------------------------------------
class ISMimeAttachment : public ISMimeFile
{
    Q_OBJECT

public:
    ISMimeAttachment(QFile* file);
    ISMimeAttachment(const QByteArray& stream, const QString& fileName);
    ~ISMimeAttachment();

protected:
    virtual void prepare();
};
//-----------------------------------------------------------------------------
