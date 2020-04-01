#include "ISMimeMessage.h"
#include "ISQuotedPrintable.h"
#include "ISConstants.h"
#include "ISMimeMultipart.h"
//-----------------------------------------------------------------------------
ISMimeMessage::ISMimeMessage(bool createAutoMimeContent) : hEncoding(ISMimePart::_8Bit)
{
	if (createAutoMimeContent)
	{
		this->content = new ISMimeMultiPart();
	}
    
    autoMimeContentCreated = createAutoMimeContent;
}
//-----------------------------------------------------------------------------
ISMimeMessage::~ISMimeMessage()
{
    if (this->autoMimeContentCreated)
    {
      this->autoMimeContentCreated = false;
      delete (this->content);
    }
}
//-----------------------------------------------------------------------------
ISMimePart& ISMimeMessage::getContent() 
{
    return *content;
}
//-----------------------------------------------------------------------------
void ISMimeMessage::setContent(ISMimePart *content) 
{
    if (this->autoMimeContentCreated)
    {
      this->autoMimeContentCreated = false;
      delete (this->content);
    }

    this->content = content;
}
//-----------------------------------------------------------------------------
void ISMimeMessage::setSender(ISEmailAddress* e)
{
    this->sender = e;
}
//-----------------------------------------------------------------------------
void ISMimeMessage::addRecipient(ISEmailAddress* rcpt, RecipientType type)
{
    switch (type)
    {
    case To:
        recipientsTo << rcpt;
        break;
    case Cc:
        recipientsCc << rcpt;
        break;
    case Bcc:
        recipientsBcc << rcpt;
        break;
    }
}
//-----------------------------------------------------------------------------
void ISMimeMessage::addTo(ISEmailAddress* rcpt) 
{
    this->recipientsTo << rcpt;
}
//-----------------------------------------------------------------------------
void ISMimeMessage::addCc(ISEmailAddress* rcpt) 
{
    this->recipientsCc << rcpt;
}
//-----------------------------------------------------------------------------
void ISMimeMessage::addBcc(ISEmailAddress* rcpt) 
{
    this->recipientsBcc << rcpt;
}
//-----------------------------------------------------------------------------
void ISMimeMessage::setSubject(const QString & subject)
{
    this->subject = subject;
}
//-----------------------------------------------------------------------------
void ISMimeMessage::addPart(ISMimePart *part)
{
    if (typeid(*content) == typeid(ISMimeMultiPart)) 
	{
        ((ISMimeMultiPart*) content)->addPart(part);
    };
}
//-----------------------------------------------------------------------------
void ISMimeMessage::setHeaderEncoding(ISMimePart::Encoding hEnc)
{
    this->hEncoding = hEnc;
}
//-----------------------------------------------------------------------------
const ISEmailAddress & ISMimeMessage::getSender() const
{
    return *sender;
}
//-----------------------------------------------------------------------------
const QList<ISEmailAddress*> & ISMimeMessage::getRecipients(RecipientType type) const
{
    switch (type)
    {
    default:
    case To:
        return recipientsTo;
    case Cc:
        return recipientsCc;
    case Bcc:
        return recipientsBcc;
    }
}
//-----------------------------------------------------------------------------
const QString & ISMimeMessage::getSubject() const
{
    return subject;
}
//-----------------------------------------------------------------------------
const QList<ISMimePart*> & ISMimeMessage::getParts() const
{
    if (typeid(*content) == typeid(ISMimeMultiPart)) {
        return ((ISMimeMultiPart*) content)->getParts();
    }
    else {
        QList<ISMimePart*> *res = new QList<ISMimePart*>();
        res->append(content);
        return *res;
    }
}
//-----------------------------------------------------------------------------
QString ISMimeMessage::toString()
{
    QString mime;

    mime = "From:";
    if (sender->getName() != "")
    {
        switch (hEncoding)
        {
        case ISMimePart::Base64:
            mime += " =?utf-8?B?" + QByteArray().append(sender->getName()).toBase64() + "?=";
            break;
        case ISMimePart::QuotedPrintable:
            mime += " =?utf-8?Q?" + ISQuotedPrintable::encode(QByteArray().append(sender->getName())).replace(' ', '_').replace(':',"=3A") + "?=";
            break;
        default:
            mime += SYMBOL_SPACE + sender->getName();
        }
    }
    mime += " <" + sender->getAddress() + ">\r\n";

	mime += "To:";
    QList<ISEmailAddress*>::iterator it;  int i;
    for (i = 0, it = recipientsTo.begin(); it != recipientsTo.end(); ++it, ++i)
    {
        if (i != 0) { mime += ','; }

        if ((*it)->getName() != "")
        {
            switch (hEncoding)
            {
            case ISMimePart::Base64:
                mime += " =?utf-8?B?" + QByteArray().append((*it)->getName()).toBase64() + "?=";
                break;
            case ISMimePart::QuotedPrintable:
                mime += " =?utf-8?Q?" + ISQuotedPrintable::encode(QByteArray().append((*it)->getName())).replace(' ', '_').replace(':',"=3A") + "?=";
                break;
            default:
                mime += SYMBOL_SPACE + (*it)->getName();
            }
        }
        mime += " <" + (*it)->getAddress() + '>';
    }
    mime += "\r\n";

	if (recipientsCc.size() != 0) {
        mime += "Cc:";
    }
    for (i = 0, it = recipientsCc.begin(); it != recipientsCc.end(); ++it, ++i)
    {
        if (i != 0) { mime += ','; }

        if ((*it)->getName() != "")
        {
            switch (hEncoding)
            {
            case ISMimePart::Base64:
                mime += " =?utf-8?B?" + QByteArray().append((*it)->getName()).toBase64() + "?=";
                break;
            case ISMimePart::QuotedPrintable:
                mime += " =?utf-8?Q?" + ISQuotedPrintable::encode(QByteArray().append((*it)->getName())).replace(' ', '_').replace(':',"=3A") + "?=";
                break;
            default:
                mime += SYMBOL_SPACE + (*it)->getName();
            }
        }
        mime += " <" + (*it)->getAddress() + '>';
    }
    if (recipientsCc.size() != 0) {
        mime += "\r\n";
    }

	mime += "Subject: ";

    switch (hEncoding)
    {
    case ISMimePart::Base64:
        mime += "=?utf-8?B?" + QByteArray().append(subject).toBase64() + "?=";
        break;
    case ISMimePart::QuotedPrintable:
        mime += "=?utf-8?Q?" + ISQuotedPrintable::encode(QByteArray().append(subject)).replace(' ', '_').replace(':',"=3A") + "?=";
        break;
    default:
        mime += subject;
    }

    mime += "\r\n";
    mime += "MIME-Version: 1.0\r\n";
    mime += QString("Date: %1\r\n").arg(QDateTime::currentDateTime().toString(Qt::RFC2822Date));

    mime += content->toString();
    return mime;
}
//-----------------------------------------------------------------------------
