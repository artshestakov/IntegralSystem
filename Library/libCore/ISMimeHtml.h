#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMimeText.h"
//-----------------------------------------------------------------------------
class ISMimeHtml : public ISMimeText
{
public:
    ISMimeHtml(const QString &html = "");
    ~ISMimeHtml();

    void setHtml(const QString & html);
    const QString& getHtml() const;

protected:
    virtual void prepare();
};
//-----------------------------------------------------------------------------
