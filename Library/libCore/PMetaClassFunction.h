#pragma once
#ifndef _PMETACLASSFUNCTION_H_INCLUDED
#define _PMETACLASSFUNCTION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT PMetaClassFunction : public PMetaClass
{
public:
	PMetaClassFunction();
	~PMetaClassFunction();

	void SetText(const QString &text);
	QString GetText() const;

private:
	QString Text;
};
//-----------------------------------------------------------------------------
#endif
