#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISCalculatorForm : public ISInterfaceForm
{
    Q_OBJECT

public:
    ISCalculatorForm(QWidget *parent = 0);
	virtual ~ISCalculatorForm();

protected:
	template<typename Function>
	ISCalculatorButton *CreateButton(const QString &text, Function slot);

	void AbortOperation();
	bool Calculate(double rightOperand, const QString &pendingOperator);

protected:
    void DigitClicked();
    void UnaryOperatorClicked();
    void AdditiveOperatorClicked();
    void MultiplicativeOperatorClicked();
    void EqualClicked();
    void PointClicked();
    void ChangeSignClicked();
    void BackspaceClicked();
    void Clear();
    void clearAll();
    void ClearMemory();
    void ReadMemory();
    void SetMemory();
    void AddToMemory();

private:
    QLineEdit *DisplayEdit;
    double SumInMemory;
    double SumSoFar;
    double FactorSoFar;
    QString PendingAdditiveOperator;
    QString PendingMultiplicativeOperator;
    bool WaitingForOperand;

    enum { NumDigitButtons = 10 };
    ISCalculatorButton *DigitButtons[NumDigitButtons];
};
//-----------------------------------------------------------------------------
