#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMimeContentFormatter : public QObject
{
    Q_OBJECT

public:
    ISMimeContentFormatter (int max_length = 76);

    void setMaxLength(int l);
    int getMaxLength() const;

    QString format(const QString &content, bool quotedPrintable = false) const;

protected:
    int max_length;
};
//-----------------------------------------------------------------------------
