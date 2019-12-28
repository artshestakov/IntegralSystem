#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISQuotedPrintable : public QObject
{
    Q_OBJECT

public:
    static QString encode(const QByteArray &input);
    static QByteArray decode(const QString &input);

private:
    ISQuotedPrintable();
};
//-----------------------------------------------------------------------------
