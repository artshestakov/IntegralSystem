#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
#include "ISListBaseForm.h"
#include "ISAsteriskCallsListForm.h"
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
	class ISRatingListForm : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ISRatingListForm(QWidget *parent = 0);
		virtual ~ISRatingListForm();

	protected:
		void FilterClicked();
	};

	//Виджет подсистемы оценок
	class ISRatingSubSystem : public ISInterfaceMetaForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ISRatingSubSystem(QWidget *parent = 0);
		virtual ~ISRatingSubSystem();

		void LoadData() override;

	protected:
		void SelectedRating();
		void ClearCDR();

	private:
		ISRatingListForm *RatingListForm;
		ISAsteriskCallsListForm *AsteriskCallsListForm;
		bool CDRLoadData;
	};
}
//-----------------------------------------------------------------------------
