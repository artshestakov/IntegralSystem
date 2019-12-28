#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISMimePart.h"
#include "ISMimeFile.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMimeAttachment : public ISMimeFile
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
