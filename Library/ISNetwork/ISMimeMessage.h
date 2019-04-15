#pragma once
//-----------------------------------------------------------------------------
#include "isnetwork_global.h"
#include "ISMimePart.h"
#include "ISMimeMultipart.h"
#include "ISEmailAddress.h"
//-----------------------------------------------------------------------------
class ISNETWORK_EXPORT ISMimeMessage : public QObject
{
	Q_OBJECT

public:

    enum RecipientType 
	{
        To, // primary
        Cc, // carbon copy
        Bcc, // blind carbon copy
    };

    ISMimeMessage(bool createAutoMimeConent = true);
    ~ISMimeMessage();

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
    void setContent(ISMimePart *content);

    virtual QString toString();

protected:
    ISEmailAddress* sender;
    QList<ISEmailAddress*> recipientsTo, recipientsCc, recipientsBcc;
    QString subject;
    ISMimePart *content;
    bool autoMimeContentCreated;
    
    ISMimePart::Encoding hEncoding;
};
//-----------------------------------------------------------------------------
