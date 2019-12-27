#include "StdAfx.h"
#include "ISCalculatorForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISCalculatorForm::ISCalculatorForm(QWidget *parent) : ISInterfaceForm(parent)
{
	setWindowTitle(LOCALIZATION("Calculator"));
	setWindowIcon(BUFFER_ICONS("Calculator"));

	ForbidResize();
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_5_PX);

    SumInMemory = 0.0;
    SumSoFar = 0.0;
    FactorSoFar = 0.0;
    WaitingForOperand = true;

    DisplayEdit = new QLineEdit(this);
	DisplayEdit->setText("0");
    DisplayEdit->setReadOnly(true);
    DisplayEdit->setAlignment(Qt::AlignRight);
    DisplayEdit->setMaxLength(15);

    QFont font = DisplayEdit->font();
    font.setPointSize(font.pointSize() + 8);
    DisplayEdit->setFont(font);

	for (int i = 0; i < NumDigitButtons; ++i)
	{
        DigitButtons[i] = CreateButton(QString::number(i), &ISCalculatorForm::DigitClicked);
    }

    ISCalculatorButton *PointButton = CreateButton(".", &ISCalculatorForm::PointClicked);
    ISCalculatorButton *ChangeSignButton = CreateButton("?", &ISCalculatorForm::ChangeSignClicked);

	ISPushButton *BackspaceButton = new ISPushButton(this);
	BackspaceButton->setText("Backspace");
	BackspaceButton->setFixedHeight(40);
	connect(BackspaceButton, &ISCalculatorButton::clicked, this, &ISCalculatorForm::BackspaceClicked);

	ISPushButton *ClearButton = new ISPushButton(this);
	ClearButton->setText("Clear");
	ClearButton->setFixedHeight(40);
	connect(ClearButton, &ISCalculatorButton::clicked, this, &ISCalculatorForm::Clear);

	ISPushButton *ClearAllButton = new ISPushButton(this);
	ClearAllButton->setText("Clear All");
	ClearAllButton->setFixedHeight(40);
	connect(ClearAllButton, &ISCalculatorButton::clicked, this, &ISCalculatorForm::clearAll);

    ISCalculatorButton *ClearMemoryButton = CreateButton("MC", &ISCalculatorForm::ClearMemory);
    ISCalculatorButton *ReadMemoryButton = CreateButton("MR", &ISCalculatorForm::ReadMemory);
    ISCalculatorButton *SetMemoryButton = CreateButton("MS", &ISCalculatorForm::SetMemory);
    ISCalculatorButton *AddToMemoryButton = CreateButton("M+", &ISCalculatorForm::AddToMemory);

	ISCalculatorButton *DivisionButton = CreateButton(SYMBOL_DIVIDE, &ISCalculatorForm::MultiplicativeOperatorClicked);
	ISCalculatorButton *TimesButton = CreateButton(SYMBOL_MULTIPLY, &ISCalculatorForm::MultiplicativeOperatorClicked);
    ISCalculatorButton *MinusButton = CreateButton("-", &ISCalculatorForm::AdditiveOperatorClicked);
    ISCalculatorButton *PlusButton = CreateButton("+", &ISCalculatorForm::AdditiveOperatorClicked);

    ISCalculatorButton *SquareRootButton = CreateButton("Sqrt", &ISCalculatorForm::UnaryOperatorClicked);
    ISCalculatorButton *PowerButton = CreateButton("pow", &ISCalculatorForm::UnaryOperatorClicked);
    ISCalculatorButton *ReciprocalButton = CreateButton("1/x", &ISCalculatorForm::UnaryOperatorClicked);
    ISCalculatorButton *EqualButton = CreateButton("=", &ISCalculatorForm::EqualClicked);

    QGridLayout *GridLayout = new QGridLayout();
	GridLayout->setSizeConstraint(QLayout::SetFixedSize);
	GetMainLayout()->addLayout(GridLayout);

    GridLayout->addWidget(DisplayEdit, 0, 0, 1, 6);
	GridLayout->addWidget(BackspaceButton, 1, 0, 1, 2);
    GridLayout->addWidget(ClearButton, 1, 2, 1, 2);
    GridLayout->addWidget(ClearAllButton, 1, 4, 1, 2);

    GridLayout->addWidget(ClearMemoryButton, 2, 0);
    GridLayout->addWidget(ReadMemoryButton, 3, 0);
    GridLayout->addWidget(SetMemoryButton, 4, 0);
    GridLayout->addWidget(AddToMemoryButton, 5, 0);

    for (int i = 1; i < NumDigitButtons; ++i)
	{
        int row = ((9 - i) / 3) + 2;
        int column = ((i - 1) % 3) + 1;
		GridLayout->addWidget(DigitButtons[i], row, column);
    }

    GridLayout->addWidget(DigitButtons[0], 5, 1);
    GridLayout->addWidget(PointButton, 5, 2);
    GridLayout->addWidget(ChangeSignButton, 5, 3);

    GridLayout->addWidget(DivisionButton, 2, 4);
    GridLayout->addWidget(TimesButton, 3, 4);
    GridLayout->addWidget(MinusButton, 4, 4);
    GridLayout->addWidget(PlusButton, 5, 4);

    GridLayout->addWidget(SquareRootButton, 2, 5);
    GridLayout->addWidget(PowerButton, 3, 5);
    GridLayout->addWidget(ReciprocalButton, 4, 5);
    GridLayout->addWidget(EqualButton, 5, 5);
}
//-----------------------------------------------------------------------------
ISCalculatorForm::~ISCalculatorForm()
{

}
//-----------------------------------------------------------------------------
template<typename Function>
ISCalculatorButton *ISCalculatorForm::CreateButton(const QString &text, Function slot)
{
	ISCalculatorButton *button = new ISCalculatorButton(this);
	button->setText(text);
	connect(button, &ISCalculatorButton::clicked, this, slot);
	return button;
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::AbortOperation()
{
	clearAll();
	DisplayEdit->setText(tr("####"));
}
//-----------------------------------------------------------------------------
bool ISCalculatorForm::Calculate(double rightOperand, const QString &pendingOperator)
{
	if (pendingOperator == tr("+"))
	{
		SumSoFar += rightOperand;
	}
	else if (pendingOperator == tr("-"))
	{
		SumSoFar -= rightOperand;
	}
	else if (pendingOperator == tr("\303\227"))
	{
		FactorSoFar *= rightOperand;
	}
	else if (pendingOperator == tr("\303\267"))
	{
		if (rightOperand == 0.0)
		{
			return false;
		}

		FactorSoFar /= rightOperand;
	}

	return true;
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::DigitClicked()
{
    ISCalculatorButton *clickedButton = qobject_cast<ISCalculatorButton *>(sender());
    int digitValue = clickedButton->text().toInt();
	if (DisplayEdit->text() == SYMBOL_0 && digitValue == 0.0)
	{
		return;
	}

    if (WaitingForOperand)
	{
        DisplayEdit->clear();
        WaitingForOperand = false;
    }

    DisplayEdit->setText(DisplayEdit->text() + QString::number(digitValue));
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::UnaryOperatorClicked()
{
    ISCalculatorButton *ClickedButton = qobject_cast<ISCalculatorButton*>(sender());
    QString clickedOperator = ClickedButton->text();
    double operand = DisplayEdit->text().toDouble();
    double result = 0.0;

    if (clickedOperator == tr("Sqrt"))
	{
        if (operand < 0.0)
		{
            AbortOperation();
            return;
        }

        result = std::sqrt(operand);
    }
	else if (clickedOperator == tr("x\302\262"))
	{
		result = std::pow(operand, 2.0);
    }
	else if (clickedOperator == tr("1/x"))
	{
        if (operand == 0.0)
		{
            AbortOperation();
            return;
        }

        result = 1.0 / operand;
    }

    DisplayEdit->setText(QString::number(result));
    WaitingForOperand = true;
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::AdditiveOperatorClicked()
{
    ISCalculatorButton *clickedButton = qobject_cast<ISCalculatorButton*>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = DisplayEdit->text().toDouble();

    if (!PendingMultiplicativeOperator.isEmpty())
	{
		if (!Calculate(operand, PendingMultiplicativeOperator))
		{
			AbortOperation();
			return;
        }

		DisplayEdit->setText(QString::number(FactorSoFar));
		operand = FactorSoFar;
		FactorSoFar = 0.0;
		PendingMultiplicativeOperator.clear();
    }

    if (!PendingAdditiveOperator.isEmpty())
	{
        if (!Calculate(operand, PendingAdditiveOperator))
		{
            AbortOperation();
            return;
        }
        
		DisplayEdit->setText(QString::number(SumSoFar));
    }
	else
	{
        SumSoFar = operand;
    }

    PendingAdditiveOperator = clickedOperator;
    WaitingForOperand = true;
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::MultiplicativeOperatorClicked()
{
    ISCalculatorButton *clickedButton = qobject_cast<ISCalculatorButton*>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = DisplayEdit->text().toDouble();

    if (!PendingMultiplicativeOperator.isEmpty())
	{
        if (!Calculate(operand, PendingMultiplicativeOperator))
		{
            AbortOperation();
            return;
        }

        DisplayEdit->setText(QString::number(FactorSoFar));
    }
	else
	{
        FactorSoFar = operand;
    }

    PendingMultiplicativeOperator = clickedOperator;
    WaitingForOperand = true;
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::EqualClicked()
{
    double operand = DisplayEdit->text().toDouble();

    if (!PendingMultiplicativeOperator.isEmpty())
	{
        if (!Calculate(operand, PendingMultiplicativeOperator))
		{
            AbortOperation();
            return;
        }

        operand = FactorSoFar;
        FactorSoFar = 0.0;
        PendingMultiplicativeOperator.clear();
    }
    if (!PendingAdditiveOperator.isEmpty())
	{
        if (!Calculate(operand, PendingAdditiveOperator))
		{
            AbortOperation();
            return;
        }

        PendingAdditiveOperator.clear();
    }
	else
	{
        SumSoFar = operand;
    }

    DisplayEdit->setText(QString::number(SumSoFar));
    SumSoFar = 0.0;
    WaitingForOperand = true;
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::PointClicked()
{
	if (WaitingForOperand)
	{
		DisplayEdit->setText("0");
	}

	if (!DisplayEdit->text().contains('.'))
	{
		DisplayEdit->setText(DisplayEdit->text() + tr("."));
	}

    WaitingForOperand = false;
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::ChangeSignClicked()
{
    QString text = DisplayEdit->text();
    double value = text.toDouble();

    if (value > 0.0)
	{
        text.prepend(tr("-"));
    }
	else if (value < 0.0)
	{
        text.remove(0, 1);
    }

    DisplayEdit->setText(text);
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::BackspaceClicked()
{
	if (WaitingForOperand)
	{
		return;
	}

    QString text = DisplayEdit->text();
    text.chop(1);
    if (text.isEmpty())
	{
        text = SYMBOL_0;
        WaitingForOperand = true;
    }

    DisplayEdit->setText(text);
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::Clear()
{
	if (WaitingForOperand)
	{
		return;
	}

    DisplayEdit->setText("0");
    WaitingForOperand = true;
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::clearAll()
{
    SumSoFar = 0.0;
    FactorSoFar = 0.0;
    PendingAdditiveOperator.clear();
    PendingMultiplicativeOperator.clear();
    DisplayEdit->setText("0");
    WaitingForOperand = true;
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::ClearMemory()
{
    SumInMemory = 0.0;
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::ReadMemory()
{
    DisplayEdit->setText(QString::number(SumInMemory));
    WaitingForOperand = true;
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::SetMemory()
{
    EqualClicked();
    SumInMemory = DisplayEdit->text().toDouble();
}
//-----------------------------------------------------------------------------
void ISCalculatorForm::AddToMemory()
{
    EqualClicked();
    SumInMemory += DisplayEdit->text().toDouble();
}
//-----------------------------------------------------------------------------
