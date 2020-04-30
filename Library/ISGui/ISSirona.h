#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
#include "ISObjectFormBase.h"
#include "ISListBaseForm.h"
#include "ISRangeDateEdit.h"
//-----------------------------------------------------------------------------
namespace ISSirona
{
	class Object : public ISObjectInterface
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Object();
		virtual ~Object();

		void RegisterMetaTypes() const override;
		void BeforeShowMainWindow() const override;
		void InitializePlugin() const override;
	};

	//Форма списка записи на приём
	class AppointmentListForm : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE AppointmentListForm(QWidget *parent = 0);
		virtual ~AppointmentListForm();

	protected:
		void AdmissionFinished(); //Приём окончен
		void FilterChange(QAbstractButton *AbstractButton); //Фильтр

	private:
		QRadioButton *RadioAll;
		QRadioButton *RadioActual;
		QRadioButton *RadioFinished;
	};

	//Форма списка расходов
	class CostListForm : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE CostListForm(QWidget *parent = 0);
		virtual ~CostListForm();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *LabelSum;
	};

	//Форма объекта расходов
	class CostObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE CostObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~CostObjectForm();

	protected:
		void AfterShowEvent() override;
		bool Save() override;
	};

	//Форма списка исследований
	class ResearchListForm : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ResearchListForm(QWidget *parent = 0);
		virtual ~ResearchListForm();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *LabelSummary;
	};

	//Форма объекта исследований
	class ResearchObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ResearchObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~ResearchObjectForm();

		bool Save() override;

	protected:
		void AfterShowEvent() override;
	};

	//Форма объекта типов исследований
	class ResearchTypeObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ResearchTypeObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~ResearchTypeObjectForm();

	protected:
		void BonusChanged(const QVariant &value);
	};

	class ReportForm : public ISInterfaceMetaForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ReportForm(QWidget *parent = 0);
		virtual ~ReportForm();

		void LoadData() override;

	protected:
		void OutputList();
		double GetDoctorBonus(int DoctorID) const;

	private:
		ISRangeDateEdit *RangeDateEdit;
		QTreeWidget *TreeWidget;
	};
}
//-----------------------------------------------------------------------------
