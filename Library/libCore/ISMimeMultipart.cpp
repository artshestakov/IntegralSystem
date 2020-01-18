#include "ISMimeMultipart.h"
//-----------------------------------------------------------------------------
const QString MULTI_PART_NAMES[] = 
{
    "multipart/mixed",         //    Mixed
    "multipart/digest",        //    Digest
    "multipart/alternative",   //    Alternative
    "multipart/related",       //    Related
    "multipart/report",        //    Report
    "multipart/signed",        //    Signed
    "multipart/encrypted"      //    Encrypted
};
//-----------------------------------------------------------------------------
ISMimeMultiPart::ISMimeMultiPart(MultiPartType type)
{
    this->type = type;
    this->cType = MULTI_PART_NAMES[this->type];
    this->cEncoding = _8Bit;

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(QByteArray().append(qrand()));
    cBoundary = md5.result().toHex();
}
//-----------------------------------------------------------------------------
ISMimeMultiPart::~ISMimeMultiPart() 
{

}
//-----------------------------------------------------------------------------
void ISMimeMultiPart::addPart(ISMimePart *part) 
{
    parts.append(part);
}
//-----------------------------------------------------------------------------
const QList<ISMimePart*> & ISMimeMultiPart::getParts() const 
{
    return parts;
}
//-----------------------------------------------------------------------------
void ISMimeMultiPart::prepare() 
{
    QList<ISMimePart*>::iterator it;

    content = "";
    for (it = parts.begin(); it != parts.end(); it++) {
        content += "--" + cBoundary + "\r\n";
        (*it)->prepare();
        content += (*it)->toString();
    };

    content += "--" + cBoundary + "--\r\n";

    ISMimePart::prepare();
}
//-----------------------------------------------------------------------------
void ISMimeMultiPart::setMimeType(const MultiPartType type) 
{
    this->type = type;
    this->cType = MULTI_PART_NAMES[type];
}
//-----------------------------------------------------------------------------
ISMimeMultiPart::MultiPartType ISMimeMultiPart::getMimeType() const 
{
    return type;
}
//-----------------------------------------------------------------------------
