#pragma once
//-----------------------------------------------------------------------------
#include "isnetwork_global.h"
#include "ISMimePart.h"
#include "ISMimeFile.h"
//-----------------------------------------------------------------------------
class ISNETWORK_EXPORT ISMimeAttachment : public ISMimeFile
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
