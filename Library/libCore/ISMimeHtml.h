#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISMimeText.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMimeHtml : public ISMimeText
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
