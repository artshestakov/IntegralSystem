#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISContextMenuImage : public QMenu
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
