#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISMimeContentFormatter
{
public:
    ISMimeContentFormatter (int max_length = 76);

    void setMaxLength(int l);
    int getMaxLength() const;

    QString format(const QString &content, bool quotedPrintable = false) const;

protected:
    int max_length;
};
//-----------------------------------------------------------------------------
