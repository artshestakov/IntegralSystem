#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISPathEditFile : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPathEditFile(QWidget *parent);
	virtual ~ISPathEditFile();

	void SetValue(const QVariant &value) override;
	void SetFilterFile(const QString &filter_file);

protected:
	void SelectFile();

private:
	QString FilterFile;
};
//-----------------------------------------------------------------------------
