#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISContextMenuImage : public QMenu
{
	Q_OBJECT

signals:
	void Select();
	void Cut();
	void Copy();
	void Paste();
	void PasteFromLink();
	void Save();
	void OpenView();

public:
	ISContextMenuImage(QWidget *parent, bool PixmapIsNull);
	virtual ~ISContextMenuImage();
};
//-----------------------------------------------------------------------------
