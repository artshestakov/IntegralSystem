#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISAttachFileItem : public QWidget
{
	Q_OBJECT

signals:
	void Delete();

public:
	ISAttachFileItem(QWidget *parent = 0);
	virtual ~ISAttachFileItem();

	void SetFilePath(const QString &Text);
	QString GetFilePath() const;

	void SetItemIndex(int Index);
	int GetItemIndex() const;

	void SetFont(const QFont &Font);

private:
	QLabel *Label;
	int ItemIndex;
};
//-----------------------------------------------------------------------------
