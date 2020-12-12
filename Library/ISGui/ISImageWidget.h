#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISImageWidget : public QLabel
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

	QByteArray GetImage() const;
	void SetByteArray(const QByteArray &byte_array);
	void SetPixmap();
	void SetPixmap(const QPixmap &Pixmap);

	void Clear();

protected:
	void Select();
	void Cut();
	void Copy();
	void Paste();
	void Save();
	void OpenView();

private:
	bool ReadFile(const QString &FilePath); //Чтение файла в буфер
	QPixmap ByteArrayToPixmap();

private:
	QByteArray ByteArray;
};
//-----------------------------------------------------------------------------
