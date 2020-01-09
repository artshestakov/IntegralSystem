#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDelegateSearchField : public QStyledItemDelegate
{
	Q_OBJECT

public:
	ISDelegateSearchField(QObject *parent = 0);
	virtual ~ISDelegateSearchField();

	QSize sizeHint(const QStyleOptionViewItem &Option, const QModelIndex &ModelIndex) const;
};
//-----------------------------------------------------------------------------
