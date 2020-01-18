#include "ISMimeText.h"
//-----------------------------------------------------------------------------
ISMimeText::ISMimeText(const QString &txt)
{
    this->text = txt;
    this->cType = "text/plain";
    this->cCharset = "utf-8";
    this->cEncoding = _8Bit;
}
//-----------------------------------------------------------------------------
ISMimeText::~ISMimeText() 
{

}
//-----------------------------------------------------------------------------
void ISMimeText::setText(const QString & text)
{
    this->text = text;
}
//-----------------------------------------------------------------------------
const QString & ISMimeText::getText() const
{
    return text;
}
//-----------------------------------------------------------------------------
void ISMimeText::prepare()
{
    this->content.clear();
    this->content.append(text);

    ISMimePart::prepare();
}
//-----------------------------------------------------------------------------
