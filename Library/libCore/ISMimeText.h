#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISMimePart.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMimeText : public ISMimePart
{
	Q_OBJECT

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
