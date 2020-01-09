#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPatriot : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPatriot(QObject *parent);
	virtual ~ISPatriot();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
	void InputScannerData(const QString &String); //������� ������ �� ������� �����-�����

protected:
	bool IsActiveCard(const QString &Barcode) const; //�������� ���������� ����� �� �����-����
	bool SearchCard(const QString &Barcode) const; //����� ����� �� �����-����
};
//-----------------------------------------------------------------------------
