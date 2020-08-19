#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISButtonDialog : public QDialogButtonBox
{
	Q_OBJECT

signals:
	void Apply();
	void Close();

public:
	ISButtonDialog(QWidget *parent, const QString &ApplyText = QString(), const QString &CloseText = QString());
	virtual ~ISButtonDialog();

	void SetApplyIcon(const QIcon &ApplyIcon);
	void SetApplyEnabled(bool Enabled);
	void SetApplyVisible(bool Visible);
	void SetApplyCursor(const QCursor &Cursor);
	void SetApplyFocus();

	void SetCloseIcon(const QIcon &Icon);
	void SetCloseEnabled(bool Enabled);
	void SetCloseCursor(const QCursor &Cursor);
};
//-----------------------------------------------------------------------------
