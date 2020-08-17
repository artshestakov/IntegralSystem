#pragma once
#ifndef _ISLABELS_H_INCLUDED
#define _ISLABELS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISQLabel : public QLabel
{
	Q_OBJECT

signals:
	void Clicked();
	void DoubleClicked();

public:
	ISQLabel(const QString &Text, QWidget *parent = 0);
	ISQLabel(QWidget *parent = 0);
	virtual ~ISQLabel();

	void SetIsLinked(bool is_linked);
	void SetElided(bool elided);

protected:
	void mouseReleaseEvent(QMouseEvent *MouseEvent);
	void mouseDoubleClickEvent(QMouseEvent *MouseEvent);
	void keyPressEvent(QKeyEvent *KeyEvent);
	void enterEvent(QEvent *Event);
	void leaveEvent(QEvent *Event);
	void resizeEvent(QResizeEvent *ResizeEvent);

private:
	bool IsLinked; //Label будет отображаться как кликабельная ссылка
	Qt::FocusPolicy FocusPolicyDefault; //Шрифт по умолчанию
	bool Elided; //Текст, которые не помещается, будет укорачиваться за счёт троеточия
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISLabelSelectionText : public ISQLabel
{
	Q_OBJECT

public:
	ISLabelSelectionText(const QString &Text, QWidget *parent = 0);
	ISLabelSelectionText(QWidget *parent = 0);
	virtual ~ISLabelSelectionText();

protected:
	void mouseReleaseEvent(QMouseEvent *MouseEvent) override;

private:
	void CopySelectedText();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//!Виджет представляющий из себя картинку и текст одновременно
class ISLabelPixmapText : public QWidget
{
	Q_OBJECT

public:
	ISLabelPixmapText(const QPixmap &Pixmap, const QString &Text, QWidget *parent = 0);
	ISLabelPixmapText(const QPixmap &Pixmap, QWidget *parent = 0);
	ISLabelPixmapText(const QString &Text, QWidget *parent = 0);
	ISLabelPixmapText(QWidget *parent = 0);
	virtual ~ISLabelPixmapText();

	QLabel* GetLabelPixmap() const;
	ISQLabel *GetLabelText() const;

	void SetDirection(QBoxLayout::Direction Direction);
	void SetPixmapText(const QPixmap &Pixmap, const QString &Text);
	void SetPixmap(const QPixmap &Pixmap);
	void SetText(const QString &Text);

	QFont GetFont() const;
	void SetFont(const QFont &Font);

	void Clear();
	void ClearPixmap();
	void ClearText();

private:
	QBoxLayout *Layout;
	QLabel *LabelPixmap;
	ISQLabel *LabelText;
};
//-----------------------------------------------------------------------------
#endif
