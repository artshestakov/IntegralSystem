#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISPriceListObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPriceListObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISPriceListObjectForm();

protected:
	void AfterShowEvent() override;
	void Month12();
	void Month6();
	void Month18();

private:
	ISFieldEditBase *EditMount12Price;
	ISFieldEditBase *EditMount12Discount;
	ISFieldEditBase *EditMount12DiscountPrice;
	ISFieldEditBase *EditMount12DiscountPriceMount;

	ISFieldEditBase *EditMount6Price;
	ISFieldEditBase *EditMount6Discount;
	ISFieldEditBase *EditMount6DiscountPrice;
	ISFieldEditBase *EditMount6DiscountPriceMount;

	ISFieldEditBase *EditMount18Price;
	ISFieldEditBase *EditMount18Discount;
	ISFieldEditBase *EditMount18DiscountPrice;
	ISFieldEditBase *EditMount18DiscountPriceMount;
};
//-----------------------------------------------------------------------------
