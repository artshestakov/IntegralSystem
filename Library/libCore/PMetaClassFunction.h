#pragma once
#ifndef _PMETACLASSFUNCTION_H_INCLUDED
#define _PMETACLASSFUNCTION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT PMetaClassFunction : public PMetaClass
{
	Q_OBJECT

	Q_PROPERTY(QString Text READ GetText WRITE SetText)

public:
	PMetaClassFunction(QObject *parent = 0);
	virtual ~PMetaClassFunction();

	void SetText(const QString &text);
	QString GetText() const;

private:
	QString Text;
};
//-----------------------------------------------------------------------------
#endif
