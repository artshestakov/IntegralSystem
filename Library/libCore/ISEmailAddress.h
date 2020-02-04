#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISEmailAddress
{
public:
    ISEmailAddress();
    ISEmailAddress(const QString &address, const QString &name = QString());
    ~ISEmailAddress();

	void setName(const QString &name);
    void setAddress(const QString &address);

    const QString& getName() const;
    const QString& getAddress() const;

private:
	QString Name;
    QString Address;
};
//-----------------------------------------------------------------------------
