#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISColumnSizeItem : public QObject
{
	Q_OBJECT

public:
	ISColumnSizeItem(QObject *parent = 0);
	virtual ~ISColumnSizeItem();

	void SetFieldSize(const QString &FieldName, int Size);
	int GetFieldSize(const QString &FieldName);

	void SetModificationFlag(bool modification_flag);
	bool GetModificationFlag() const;

	QMap<QString, int> GetFields();

private:
	QMap<QString, int> Fields;
	bool ModificationFlag;
};
//-----------------------------------------------------------------------------
