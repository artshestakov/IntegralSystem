#pragma once
//-----------------------------------------------------------------------------
#include "isnetwork_global.h"
//-----------------------------------------------------------------------------
class ISNETWORK_EXPORT ISQuotedPrintable : public QObject
{
    Q_OBJECT

public:
    static QString encode(const QByteArray &input);
    static QByteArray decode(const QString &input);

private:
    ISQuotedPrintable();
};
//-----------------------------------------------------------------------------
