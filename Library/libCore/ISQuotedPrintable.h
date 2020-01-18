#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISQuotedPrintable : public QObject
{
    Q_OBJECT

public:
    static QString encode(const QByteArray &input);
    static QByteArray decode(const QString &input);

private:
    ISQuotedPrintable();
};
//-----------------------------------------------------------------------------
