#include "ISMimeMessage.h"
#include "ISQuotedPrintable.h"
#include "ISConstants.h"
#include "ISMimeMultipart.h"
//-----------------------------------------------------------------------------
ISMimeMessage::ISMimeMessage(bool createAutoMimeContent)
    : QObject(),
    hEncoding(ISMimePart::_8Bit)
{
	if (createAutoMimeContent)
	{
        MimePart = new ISMimeMultiPart();
	}
    autoMimeContentCreated = createAutoMimeContent;
}
//-----------------------------------------------------------------------------
ISMimeMessage::~ISMimeMessage()
{
    if (autoMimeContentCreated)
    {
      autoMimeContentCreated = false;
      delete MimePart;
    }
}
//-----------------------------------------------------------------------------
ISMimePart& ISMimeMessage::getContent() 
{
    return *MimePart;
}
//-----------------------------------------------------------------------------
void ISMimeMessage::setContent(ISMimePart *mime_part)
{
    if (autoMimeContentCreated)
    {
      autoMimeContentCreated = false;
      delete MimePart;
    }
    MimePart = mime_part;
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
    recipientsTo << rcpt;
}
//-----------------------------------------------------------------------------
void ISMimeMessage::addCc(ISEmailAddress* rcpt) 
{
    recipientsCc << rcpt;
}
//-----------------------------------------------------------------------------
void ISMimeMessage::addBcc(ISEmailAddress* rcpt) 
{
    recipientsBcc << rcpt;
}
//-----------------------------------------------------------------------------
void ISMimeMessage::setSubject(const QString &subject)
{
    Subject = subject;
}
//-----------------------------------------------------------------------------
void ISMimeMessage::addPart(ISMimePart *part)
{
    if (typeid(*MimePart) == typeid(ISMimeMultiPart))
	{
        ((ISMimeMultiPart*)MimePart)->addPart(part);
    };
}
//-----------------------------------------------------------------------------
void ISMimeMessage::setHeaderEncoding(ISMimePart::Encoding hEnc)
{
    hEncoding = hEnc;
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
    return Subject;
}
//-----------------------------------------------------------------------------
const QList<ISMimePart*> & ISMimeMessage::getParts() const
{
    if (typeid(*MimePart) == typeid(ISMimeMultiPart))
    {
        return ((ISMimeMultiPart*)MimePart)->getParts();
    }
    else
    {
        QList<ISMimePart*> *res = new QList<ISMimePart*>();
        res->append(MimePart);
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

    if (recipientsCc.size() != 0)
    {
        mime += "Cc:";
    }

    for (i = 0, it = recipientsCc.begin(); it != recipientsCc.end(); ++it, ++i)
    {
        if (i != 0)
        {
            mime += ',';
        }

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

    if (recipientsCc.size() != 0)
    {
        mime += "\r\n";
    }

	mime += "Subject: ";

    switch (hEncoding)
    {
    case ISMimePart::Base64:
        mime += "=?utf-8?B?" + QByteArray().append(Subject).toBase64() + "?=";
        break;
    case ISMimePart::QuotedPrintable:
        mime += "=?utf-8?Q?" + ISQuotedPrintable::encode(QByteArray().append(Subject)).replace(' ', '_').replace(':',"=3A") + "?=";
        break;
    default:
        mime += Subject;
    }

    mime += "\r\n";
    mime += "MIME-Version: 1.0\r\n";
    mime += QString("Date: %1\r\n").arg(QDateTime::currentDateTime().toString(Qt::RFC2822Date));
    mime += MimePart->toString();
    return mime;
}
//-----------------------------------------------------------------------------
