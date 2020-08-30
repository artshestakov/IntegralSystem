#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
#include "ISListBaseForm.h"
//#include "ISAsteriskCallsListForm.h"
//-----------------------------------------------------------------------------
namespace ISMedTech
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

	//Форма списка оценок
	class RatingListForm : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE RatingListForm(QWidget *parent = 0);
		virtual ~RatingListForm();

	protected:
		void FilterClicked();
	};

	//Виджет подсистемы оценок
	class RatingSubSystem : public ISInterfaceMetaForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE RatingSubSystem(QWidget *parent = 0);
		virtual ~RatingSubSystem();

		void LoadData() override;

	protected:
		void SelectedRating();
		void ClearCDR();

	private:
		RatingListForm *RatingListForm;
		//ISAsteriskCallsListForm *AsteriskCallsListForm;
		bool CDRLoadData;
	};
}
//-----------------------------------------------------------------------------
