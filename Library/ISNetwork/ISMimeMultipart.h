#pragma once
//-----------------------------------------------------------------------------
#include "isnetwork_global.h"
#include "ISMimePart.h"
//-----------------------------------------------------------------------------
class ISNETWORK_EXPORT ISMimeMultiPart : public ISMimePart
{
    Q_OBJECT

public:
    enum MultiPartType 
	{
        Mixed = 0, // RFC 2046, section 5.1.3
        Digest = 1, // RFC 2046, section 5.1.5
        Alternative = 2, // RFC 2046, section 5.1.4
        Related = 3, // RFC 2387
        Report = 4, // RFC 6522
        Signed = 5, // RFC 1847, section 2.1
        Encrypted = 6  // RFC 1847, section 2.2
    };

    ISMimeMultiPart(const MultiPartType type = Related);
    ~ISMimeMultiPart();

    void setMimeType(const MultiPartType type);
    MultiPartType getMimeType() const;
    const QList<ISMimePart*> & getParts() const;
    void addPart(ISMimePart *part);
    virtual void prepare();

protected:
    QList<ISMimePart*> parts;
    MultiPartType type;
};
//-----------------------------------------------------------------------------
