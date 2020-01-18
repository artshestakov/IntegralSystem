#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMimeText.h"
//-----------------------------------------------------------------------------
class ISMimeHtml : public ISMimeText
{
    Q_OBJECT

public:
    ISMimeHtml(const QString &html = "");
    ~ISMimeHtml();

    void setHtml(const QString & html);
    const QString& getHtml() const;

protected:
    virtual void prepare();
};
//-----------------------------------------------------------------------------
