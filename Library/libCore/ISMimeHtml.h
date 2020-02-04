#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISMimeText.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMimeHtml : public ISMimeText
{
public:
    ISMimeHtml(const QString &html = "");
    ~ISMimeHtml();

    void setHtml(const QString & html);
    const QString& getHtml() const;

protected:
    virtual void prepare();
};
//-----------------------------------------------------------------------------
