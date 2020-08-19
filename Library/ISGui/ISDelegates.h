#pragma once
#ifndef _ISDELEGATES_H_INCLUDED
#define _ISDELEGATES_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISDelegateBoolean : public QStyledItemDelegate
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDelegateBoolean(QObject *parent);
	virtual ~ISDelegateBoolean();

protected:
	void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	void initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const;

private:
	QImage ImageNotSelected;
	QImage ImageSelected;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDelegateCallDetails : public QStyledItemDelegate
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDelegateCallDetails(QObject *parent);
	virtual ~ISDelegateCallDetails();

protected:
	void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	void initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const;

private:
	QIcon IconDetailsNotNull;
	QIcon IconDetailsNull;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDelegateColor : public QStyledItemDelegate
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDelegateColor(QObject *parent);
	virtual ~ISDelegateColor();

protected:
	void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDelegateFile : public QStyledItemDelegate
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDelegateFile(QWidget *parent = 0);
	virtual ~ISDelegateFile();

protected:
	void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	void initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const;

private:
	QImage ImageNotSelected;
	QImage ImageSelected;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDelegateImage : public QStyledItemDelegate
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDelegateImage(QObject *parent);
	virtual ~ISDelegateImage();

protected:
	QSize sizeHint(const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	void initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDelegateSearchField : public QStyledItemDelegate
{
	Q_OBJECT

public:
	ISDelegateSearchField(QObject *parent = 0);
	virtual ~ISDelegateSearchField();

	QSize sizeHint(const QStyleOptionViewItem &Option, const QModelIndex &ModelIndex) const;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISPopupDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	ISPopupDelegate(QObject *parent = 0);
	virtual ~ISPopupDelegate();

protected:
	virtual void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
};
//-----------------------------------------------------------------------------
#endif
