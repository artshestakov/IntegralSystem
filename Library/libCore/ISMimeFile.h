#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISMimePart.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMimeFile : public ISMimePart
{
    Q_OBJECT

public:
    ISMimeFile(const QByteArray& stream, const QString& fileName);
    ISMimeFile(QFile *f);
    ~ISMimeFile();

protected:
    virtual void prepare();
    QFile* file;
};
//-----------------------------------------------------------------------------
