#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class PMetaClassFunction : public PMetaClass
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
