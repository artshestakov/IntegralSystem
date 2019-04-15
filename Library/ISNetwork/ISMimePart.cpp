#include "StdAfx.h"
#include "ISMimePart.h"
#include "ISQuotedPrintable.h"
//-----------------------------------------------------------------------------
ISMimePart::ISMimePart()
{
    cEncoding = _7Bit;
    prepared = false;
    cBoundary = "";
}
//-----------------------------------------------------------------------------
ISMimePart::~ISMimePart()
{
    return;
}
//-----------------------------------------------------------------------------
void ISMimePart::setContent(const QByteArray & content)
{
    this->content = content;
}
//-----------------------------------------------------------------------------
void ISMimePart::setHeader(const QString & header)
{
    this->header = header;
}
//-----------------------------------------------------------------------------
void ISMimePart::addHeaderLine(const QString & line)
{
    this->header += line + "\r\n";
}
//-----------------------------------------------------------------------------
const QString& ISMimePart::getHeader() const
{
    return header;
}
//-----------------------------------------------------------------------------
const QByteArray& ISMimePart::getContent() const
{
    return content;
}
//-----------------------------------------------------------------------------
void ISMimePart::setContentId(const QString & cId)
{
    this->cId = cId;
}
//-----------------------------------------------------------------------------
const QString & ISMimePart::getContentId() const
{
    return this->cId;
}
//-----------------------------------------------------------------------------
void ISMimePart::setContentName(const QString & cName)
{
    this->cName = cName;
}
//-----------------------------------------------------------------------------
const QString & ISMimePart::getContentName() const
{
    return this->cName;
}
//-----------------------------------------------------------------------------
void ISMimePart::setContentType(const QString & cType)
{
    this->cType = cType;
}
//-----------------------------------------------------------------------------
const QString & ISMimePart::getContentType() const
{
    return this->cType;
}
//-----------------------------------------------------------------------------
void ISMimePart::setCharset(const QString & charset)
{
    this->cCharset = charset;
}
//-----------------------------------------------------------------------------
const QString & ISMimePart::getCharset() const
{
    return this->cCharset;
}
//-----------------------------------------------------------------------------
void ISMimePart::setEncoding(Encoding enc)
{
    this->cEncoding = enc;
}
//-----------------------------------------------------------------------------
ISMimePart::Encoding ISMimePart::getEncoding() const
{
    return this->cEncoding;
}
//-----------------------------------------------------------------------------
ISMimeContentFormatter& ISMimePart::getContentFormatter()
{
    return this->formatter;
}
//-----------------------------------------------------------------------------
QString ISMimePart::toString()
{
    if (!prepared)
        prepare();

    return mimeString;
}
//-----------------------------------------------------------------------------
void ISMimePart::prepare()
{
    mimeString = QString();

    /* === Header Prepare === */

    /* Content-Type */
    mimeString.append("Content-Type: ").append(cType);

    if (cName != "")
        mimeString.append("; name=\"").append(cName).append("\"");

    if (cCharset != "")
        mimeString.append("; charset=").append(cCharset);

    if (cBoundary != "")
        mimeString.append("; boundary=").append(cBoundary);

    mimeString.append("\r\n");
    /* ------------ */

    /* Content-Transfer-Encoding */
    mimeString.append("Content-Transfer-Encoding: ");
    switch (cEncoding)
    {
    case _7Bit:
        mimeString.append("7bit\r\n");
        break;
    case _8Bit:
        mimeString.append("8bit\r\n");
        break;
    case Base64:
        mimeString.append("base64\r\n");
        break;
    case QuotedPrintable:
        mimeString.append("quoted-printable\r\n");
        break;
    }
    /* ------------------------ */

    /* Content-Id */
    if (cId != NULL)
        mimeString.append("Content-ID: <").append(cId).append(">\r\n");
    /* ---------- */

    /* Addition header lines */

    mimeString.append(header).append("\r\n");

    /* ------------------------- */

    /* === End of Header Prepare === */

    /* === Content === */
    switch (cEncoding)
    {
    case _7Bit:
        mimeString.append(QString(content).toLatin1());
        break;
    case _8Bit:
        mimeString.append(content);
        break;
    case Base64:
        mimeString.append(formatter.format(content.toBase64()));
        break;
    case QuotedPrintable:
        mimeString.append(formatter.format(ISQuotedPrintable::encode(content), true));
        break;
    }
    mimeString.append("\r\n");
    /* === End of Content === */

    prepared = true;
}
//-----------------------------------------------------------------------------
