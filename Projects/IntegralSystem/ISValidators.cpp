#include "ISValidators.h"
#include "ISTypedefsOld.h"
#include "ISAlgorithmOld.h"
#include "ISConstantsOld.h"
//-----------------------------------------------------------------------------
ISIntValidator::ISIntValidator(int Bottom, int Top, QObject *parent) : QIntValidator(Bottom, Top, parent)
{

}
//-----------------------------------------------------------------------------
ISIntValidator::ISIntValidator(QObject *parent) : ISIntValidator(0, 0, parent)
{

}
//-----------------------------------------------------------------------------
ISIntValidator::~ISIntValidator()
{

}
//-----------------------------------------------------------------------------
QIntValidator::State ISIntValidator::validate(QString &String, int &Pos) const
{
    if (String.isEmpty() && !Pos)
    {
        return ISIntValidator::Acceptable;
    }

    //���� ������ ������ �����, ���������� ��������������� ����
    bool IsMinus = String.front() == '-';

    ISVectorInt VectorInt;
    for (int i = 0, c = String.size(); i < c; ++i) //���� ������� �� ����
    {
        QChar Symbol = String[i];
        if (!Symbol.isDigit())
        {
            VectorInt.emplace_back(i);
        }
    }
    Pos -= (int)VectorInt.size(); //�������� � ������� ������ ��������� �� ����
    while (!VectorInt.empty()) //������� �� �����
    {
        String.remove(ISAlgorithmOld::VectorTakeBack(VectorInt), 1);
    }

    //��������� "�����" �����: ��� �� ������ ���� ������ MAX_INTEGER_LEN
    String.chop(String.size() - MAX_INTEGER_LEN);

    if (IsMinus) //���� ����� �������������
    {
        if (String != '0') //� ��� ���� ��� ���� - ������ �� ������ (������ ��� ���� �� ����� ���� �������������), ����� - ��������� ����� � �������������� �������
        {
            String.insert(0, '-');
            ++Pos;
        }
    }

    //���� ������ ��� ������� ������� �� ���� - ��������� ��
    if (bottom() != 0 || top() != 0)
    {
        bool Ok = true;
        int Value = String.toInt(&Ok); //�������� ��������� ������ � �����
        if (Ok) //���� ��� �� - ���������� ��������
        {
            if (Value < bottom()) //���� �������� �������� ������ ������� ������� - ������������ �������� � ������� �������
            {
                Value = bottom();
            }
            else if (Value > top()) //���� �������� �������� ������ �������� ������� - ������������ �������� � �������� �������
            {
                Value = top();
            }
            String.setNum(Value); //������������� �������� � ������
        }
    }
    return ISIntValidator::Acceptable;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDoubleValidator::ISDoubleValidator(double Bottom, double Top, int Decimals, QObject *parent)
//���� ���������� ������ ������ ����, ��� ������ �����������, ��� ����� ���� - ������������ ��� � �����������
    : QDoubleValidator(Bottom, Top, Decimals < 0 || Decimals > MAX_DECIMAL_LEN || !Decimals ? MAX_DECIMAL_LEN : Decimals, parent)
{

}
//-----------------------------------------------------------------------------
ISDoubleValidator::ISDoubleValidator(int Decimals, QObject *parent) : ISDoubleValidator(0, 0, Decimals, parent)
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

    //���� ������ ������ �����, ���������� ��������������� ����
    bool IsMinus = String.front() == '-';

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
    Pos -= (int)VectorInt.size(); //�������� � ������� ������ ��������� �� ����

    //������� �� �����
    while (!VectorInt.empty())
    {
        String.remove(ISAlgorithmOld::VectorTakeBack(VectorInt), 1);
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
        IntPart.setNum(IntPart.toLongLong()); //��������������� ����������� ��������
        String.chop(String.size() - MAX_INTEGER_LEN); //�� ��� ����� ����� ����� ����� �� �����������
    }
    else //����� �������
    {
        IntPart.setNum(IntPart.toLongLong()); //��������������� ����������� ��������
        String.chop(String.size() - MAX_INTEGER_LEN); //�� ��� ����� ����� ����� ����� �� �����������

        //��������� �������� Decimal � ����������� MAX_DECIMAL_LEN
        if (DoublePart.size() > decimals())
        {
            DoublePart.chop(DoublePart.size() - decimals());
        }

        //�������� �������� ������� �����
        String = IntPart + SYMBOL_POINT + DoublePart;
    }

    if (IsMinus) //���� ����� ������������� - ��������� ����� � �������������� �������
    {
        String.insert(0, '-');
        ++Pos;
    }

    //���� ������ ��� ������� ������� �� ���� - ��������� ��
    if (bottom() != 0 || top() != 0)
    {
        bool Ok = true;
        double Value = String.toDouble(&Ok); //�������� ��������� ������ � �����
        if (Ok) //���� ��� �� - ���������� ��������
        {
            if (Value < bottom()) //���� �������� �������� ������ ������� ������� - ������������ �������� � ������� �������
            {
                String.setNum(bottom());
                return ISDoubleValidator::Acceptable;
            }
            else if (Value > top()) //���� �������� �������� ������ �������� ������� - ������������ �������� � �������� �������
            {
                String.setNum(top());
                return ISDoubleValidator::Acceptable;
            }

            if (String.back() != SYMBOL_POINT && String.back() != '0') //�������� setNum ������ ����� ��������� ������ ������ �� ����� ����������� � �� ����� ����
            {
                String.setNum(Value); //������������� �������� � ������
            }
        }
    }
    return ISDoubleValidator::Acceptable;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISMoneyValidator::ISMoneyValidator(QObject *parent) : ISDoubleValidator(2, parent)
{

}
//-----------------------------------------------------------------------------
ISMoneyValidator::~ISMoneyValidator()
{

}
//-----------------------------------------------------------------------------
