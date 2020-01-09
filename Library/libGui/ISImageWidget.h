#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISImageWidget : public QLabel
{
	Q_OBJECT

signals:
	void ImageChanged();

public:
	ISImageWidget(QWidget *parent = 0);
	virtual ~ISImageWidget();
	
	void contextMenuEvent(QContextMenuEvent *e);
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
	void dragMoveEvent(QDragMoveEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);
	
	void SetBackgroundImage(const QPixmap &Pixmap);

	QPixmap GetPixmap() const;
	void SetPixmap(const QPixmap &Pixmap);
	void SetPixmap(const QString &Path);

	void Clear();

protected:
	void Select();
	void Cut();
	void Copy();
	void Paste();
	void PasteFromLink();
	void Save();
	void OpenView();

private:
	QPixmap CurrentPixmap;
};
//-----------------------------------------------------------------------------
