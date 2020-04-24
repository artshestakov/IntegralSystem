#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISMimePart.h"
#include "ISEmailAddress.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISMimeMessage : public QObject
{
public:
    enum RecipientType 
	{
        To, // primary
        Cc, // carbon copy
        Bcc, // blind carbon copy
    };

public:
    ISMimeMessage(bool createAutoMimeConent = true);
    virtual ~ISMimeMessage();

    void setSender(ISEmailAddress* e);
    void addRecipient(ISEmailAddress* rcpt, RecipientType type = To);
    void addTo(ISEmailAddress* rcpt);
    void addCc(ISEmailAddress* rcpt);
    void addBcc(ISEmailAddress* rcpt);
    void setSubject(const QString & subject);
    void addPart(ISMimePart* part);

    void setHeaderEncoding(ISMimePart::Encoding);

    const ISEmailAddress & getSender() const;
    const QList<ISEmailAddress*> & getRecipients(RecipientType type = To) const;
    const QString & getSubject() const;
    const QList<ISMimePart*> & getParts() const;

    ISMimePart& getContent();
    void setContent(ISMimePart *mime_part);

    virtual QString toString();

protected:
    ISEmailAddress* sender;
    QList<ISEmailAddress*> recipientsTo, recipientsCc, recipientsBcc;
    QString Subject;
    ISMimePart *MimePart;
    bool autoMimeContentCreated;
    
    ISMimePart::Encoding hEncoding;
};
//-----------------------------------------------------------------------------
