#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISPatriot : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPatriot();
	virtual ~ISPatriot();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
	void InputScannerData(const QString &String); //Входные данные со сканера штрих-кодов

protected:
	bool IsActiveCard(const QString &Barcode) const; //Проверка активности карты по штрих-коду
	bool SearchCard(const QString &Barcode) const; //Поиск карты по штрих-коду
};
//-----------------------------------------------------------------------------
