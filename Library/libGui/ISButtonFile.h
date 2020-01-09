#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISButtonFile : public ISPushButton
{
	Q_OBJECT

signals:
	void DragAndDropFile(const QString &FilePath);

public:
	ISButtonFile(QWidget *parent = 0);
	virtual ~ISButtonFile();

protected:
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
	void dragMoveEvent(QDragMoveEvent *e);
};
//-----------------------------------------------------------------------------
