#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMimePart.h"
//-----------------------------------------------------------------------------
class ISMimeText : public ISMimePart
{
public:
    ISMimeText(const QString &text = "");
    ~ISMimeText();

    void setText(const QString & text);
    const QString & getText() const;

protected:
    QString text;
    void prepare();
};
//-----------------------------------------------------------------------------
