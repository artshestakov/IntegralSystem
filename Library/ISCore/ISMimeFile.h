#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMimePart.h"
//-----------------------------------------------------------------------------
class ISMimeFile : public ISMimePart
{
public:
    ISMimeFile(const QByteArray& stream, const QString& fileName);
    ISMimeFile(QFile *f);
    ~ISMimeFile();

protected:
    virtual void prepare();
    QFile* file;
};
//-----------------------------------------------------------------------------
