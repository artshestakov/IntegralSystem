#include "ISPriceListObjectForm.h"
//-----------------------------------------------------------------------------
ISPriceListObjectForm::ISPriceListObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	EditMount12Price = GetFieldWidget("Mount12Price");
	connect(EditMount12Price, &ISFieldEditBase::DataChanged, this, &ISPriceListObjectForm::Month12);

	EditMount12Discount = GetFieldWidget("Mount12Discount");
	connect(EditMount12Discount, &ISFieldEditBase::DataChanged, this, &ISPriceListObjectForm::Month12);

	EditMount12DiscountPrice = GetFieldWidget("Mount12DiscountPrice");
	connect(EditMount12DiscountPrice, &ISFieldEditBase::DataChanged, this, &ISPriceListObjectForm::Month12);

	EditMount12DiscountPriceMount = GetFieldWidget("Mount12DiscountPriceMount");
	connect(EditMount12DiscountPriceMount, &ISFieldEditBase::DataChanged, this, &ISPriceListObjectForm::Month12);



	EditMount6Price = GetFieldWidget("Mount6Price");
	connect(EditMount6Price, &ISFieldEditBase::DataChanged, this, &ISPriceListObjectForm::Month6);

	EditMount6Discount = GetFieldWidget("Mount6Discount");
	connect(EditMount6Discount, &ISFieldEditBase::DataChanged, this, &ISPriceListObjectForm::Month6);

	EditMount6DiscountPrice = GetFieldWidget("Mount6DiscountPrice");
	connect(EditMount6DiscountPrice, &ISFieldEditBase::DataChanged, this, &ISPriceListObjectForm::Month6);

	EditMount6DiscountPriceMount = GetFieldWidget("Mount6DiscountPriceMount");
	connect(EditMount6DiscountPriceMount, &ISFieldEditBase::DataChanged, this, &ISPriceListObjectForm::Month6);



	EditMount18Price = GetFieldWidget("Mount18Price");
	connect(EditMount18Price, &ISFieldEditBase::DataChanged, this, &ISPriceListObjectForm::Month18);

	EditMount18Discount = GetFieldWidget("Mount18Discount");
	connect(EditMount18Discount, &ISFieldEditBase::DataChanged, this, &ISPriceListObjectForm::Month18);

	EditMount18DiscountPrice = GetFieldWidget("Mount18DiscountPrice");
	connect(EditMount18DiscountPrice, &ISFieldEditBase::DataChanged, this, &ISPriceListObjectForm::Month18);

	EditMount18DiscountPriceMount = GetFieldWidget("Mount18DiscountPriceMount");
	connect(EditMount18DiscountPriceMount, &ISFieldEditBase::DataChanged, this, &ISPriceListObjectForm::Month18);
}
//-----------------------------------------------------------------------------
ISPriceListObjectForm::~ISPriceListObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISPriceListObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();
	
	Month12();
	Month6();
	Month18();
}
//-----------------------------------------------------------------------------
void ISPriceListObjectForm::Month12()
{
	double Discount = EditMount12Price->GetValue().toInt() * EditMount12Discount->GetValue().toInt() / 100;
	EditMount12DiscountPrice->SetValue(Discount);
	
	double DiscountMount = Discount * EditMount12Discount->GetValue().toInt() / 100;
	EditMount12DiscountPriceMount->SetValue(Discount - DiscountMount);
}
//-----------------------------------------------------------------------------
void ISPriceListObjectForm::Month6()
{
	double Discount = EditMount6Price->GetValue().toInt() * EditMount6Discount->GetValue().toInt() / 100;
	EditMount6DiscountPrice->SetValue(Discount);

	double DiscountMount = Discount / 6;
	EditMount6DiscountPriceMount->SetValue(DiscountMount);
}
//-----------------------------------------------------------------------------
void ISPriceListObjectForm::Month18()
{
	double Discount = EditMount18Price->GetValue().toInt() * EditMount18Discount->GetValue().toInt() / 100;
	EditMount18DiscountPrice->SetValue(Discount);

	double DiscountMount = Discount / 18;
	EditMount18DiscountPriceMount->SetValue(DiscountMount);
}
//-----------------------------------------------------------------------------
