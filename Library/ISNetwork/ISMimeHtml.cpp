#include "StdAfx.h"
#include "ISMimeHtml.h"
//-----------------------------------------------------------------------------
ISMimeHtml::ISMimeHtml(const QString &html) : ISMimeText(html)
{
    this->cType = "text/html";
}
//-----------------------------------------------------------------------------
ISMimeHtml::~ISMimeHtml() 
{

}
//-----------------------------------------------------------------------------
void ISMimeHtml::setHtml(const QString & html)
{
    this->text = html;
}
//-----------------------------------------------------------------------------
const QString & ISMimeHtml::getHtml() const
{
    return text;
}
//-----------------------------------------------------------------------------
void ISMimeHtml::prepare()
{
    ISMimeText::prepare();
}
//-----------------------------------------------------------------------------
