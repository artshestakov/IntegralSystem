#include "ISValidators.h"
#include "ISTypedefs.h"
#include "ISAlgorithm.h"
#include "ISConstants.h"
#include "ISSettingsDatabase.h"
#include "ISSettings.h"
//-----------------------------------------------------------------------------
ISIntValidator::ISIntValidator(int Bottom, int Top, QObject *parent) : QIntValidator(Bottom, Top, parent)
{

}
//-----------------------------------------------------------------------------
ISIntValidator::ISIntValidator(QObject *parent) : QIntValidator(parent)
{

}
//-----------------------------------------------------------------------------
ISIntValidator::~ISIntValidator()
{

}
//-----------------------------------------------------------------------------
QIntValidator::State ISIntValidator::validate(QString &String, int &Pos) const
{
	ISVectorInt VectorInt;
	for (int i = 0, c = String.size(); i < c; ++i) //���� ������� �� ����
	{
		if (!String[i].isDigit())
		{
			VectorInt.emplace_back(i);
		}
	}
	Pos -= VectorInt.size(); //�������� � ������� ������ ��������� �� ����
	while (!VectorInt.empty()) //������� �� �����
	{
		String.remove(ISAlgorithm::VectorTakeBack(VectorInt), 1);
	}
	return ISIntValidator::Acceptable;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDoubleValidator::ISDoubleValidator(double Bottom, double Top, int Decimals, QObject *parent) : QDoubleValidator(Bottom, Top, Decimals, parent)
{

}
//-----------------------------------------------------------------------------
ISDoubleValidator::ISDoubleValidator(QObject *parent)
	: QDoubleValidator(parent),
	Decimal(SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA))
{

}
//-----------------------------------------------------------------------------
ISDoubleValidator::~ISDoubleValidator()
{

}
//-----------------------------------------------------------------------------
QDoubleValidator::State ISDoubleValidator::validate(QString &String, int &Pos) const
{
	if (String.isEmpty() && !Pos) //���� ���� ���� �������
	{
		return QDoubleValidator::Acceptable;
	}

	//���� ����������� ������� - ������ �� �����
	if (String.contains(SYMBOL_COMMA))
	{
		String.replace(SYMBOL_COMMA, SYMBOL_POINT);
	}

	ISVectorInt VectorInt;
	for (int i = 0, c = String.size(); i < c; ++i) //���� ������� �� ����
	{
		QChar Symbol = String[i];
		if (Symbol.isDigit() || Symbol == SYMBOL_POINT) //���� ������� ������ �������� ������ � ����������� ������������ - ����������
		{
			continue;
		}
		VectorInt.emplace_back(i); //����� ���������� ������ �� ����� � �� �����������
	}
	Pos -= VectorInt.size(); //�������� � ������� ������ ��������� �� ����

	//������� �� �����
	while (!VectorInt.empty())
	{
		String.remove(ISAlgorithm::VectorTakeBack(VectorInt), 1);
	}

	//���� ������ ������� ����������� �����������
	int SeparatorFirstIndex = String.indexOf(SYMBOL_POINT);
	if (SeparatorFirstIndex != -1) //����� ����������� - � ����� ������� ���� � ������� ����������� (� ����� ������ �������� ������ ���� �����������)
	{
		while ((SeparatorFirstIndex = String.indexOf(SYMBOL_POINT, ++SeparatorFirstIndex)) != -1)
		{
			String.remove(SeparatorFirstIndex, 1);
		}
	}

	//��������� ������ �� ����� � ������� �����
	QStringList StringList = String.split(SYMBOL_POINT);
	QString IntPart = StringList.front(); //����� �����
	QString DoublePart = StringList.back(); //������� �����

	if (IntPart == DoublePart) //���� ����� � ������� ����� ����� - ��� ����� �����
	{
		String.setNum(IntPart.toInt()); //��������������� ����������� ��������
	}
	else //����� �������
	{
		IntPart.setNum(IntPart.toInt()); //��������������� ����������� ��������

		//��������� �������� Decimal
		if (DoublePart.size() > Decimal)
		{
			DoublePart.chop(DoublePart.size() - Decimal);
		}

		//�������� �������� ������� �����
		String = IntPart + SYMBOL_POINT + DoublePart;
	}
	return QDoubleValidator::Acceptable;
}
//-----------------------------------------------------------------------------
