#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISFieldEditBase.h"
#include "ISImageWidget.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISImageEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISImageEdit(QWidget *parent);
	virtual ~ISImageEdit();

	virtual void SetValue(const QVariant &value) override;
	virtual QVariant GetValue() const override;
	virtual void Clear() override;
	virtual void SetVisibleClear(bool Visible) override;

public slots:
	void SetReadOnly(bool read_only);

private:
	ISImageWidget *ImageWidget;
};
//-----------------------------------------------------------------------------
