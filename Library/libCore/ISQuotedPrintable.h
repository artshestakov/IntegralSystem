#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISQuotedPrintable
{
public:
    static QString encode(const QByteArray &input);
    static QByteArray decode(const QString &input);
};
//-----------------------------------------------------------------------------
