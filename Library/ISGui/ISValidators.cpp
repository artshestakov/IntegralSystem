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
	for (int i = 0, c = String.size(); i < c; ++i) //Ищём индексы НЕ цифр
	{
		if (!String[i].isDigit())
		{
			VectorInt.emplace_back(i);
		}
	}
	Pos -= VectorInt.size(); //Отнимаем у позиции размер найденных НЕ цифр
	while (!VectorInt.empty()) //Удаляем не цифры
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
	if (String.isEmpty() && !Pos) //Если поле было очищено
	{
		return QDoubleValidator::Acceptable;
	}

	//Если встретилась запятая - меняем на точку
	if (String.contains(SYMBOL_COMMA))
	{
		String.replace(SYMBOL_COMMA, SYMBOL_POINT);
	}

	ISVectorInt VectorInt;
	for (int i = 0, c = String.size(); i < c; ++i) //Ищём индексы НЕ цифр
	{
		QChar Symbol = String[i];
		if (Symbol.isDigit() || Symbol == SYMBOL_POINT) //Если текущий символ является цифрой и настроенным разделителем - пропускаем
		{
			continue;
		}
		VectorInt.emplace_back(i); //Иначе запоминаем индекс НЕ цифры и НЕ разделителя
	}
	Pos -= VectorInt.size(); //Отнимаем у позиции размер найденных НЕ цифр

	//Удаляем не цифры
	while (!VectorInt.empty())
	{
		String.remove(ISAlgorithm::VectorTakeBack(VectorInt), 1);
	}

	//Ищем индекс первого попавшегося разделителя
	int SeparatorFirstIndex = String.indexOf(SYMBOL_POINT);
	if (SeparatorFirstIndex != -1) //Нашли разделитель - с этого индекса ищем и удаляем разделители (в итоге должен остаться только один разделитель)
	{
		while ((SeparatorFirstIndex = String.indexOf(SYMBOL_POINT, ++SeparatorFirstIndex)) != -1)
		{
			String.remove(SeparatorFirstIndex, 1);
		}
	}

	//Разбиваем строку на целое и дробное число
	QStringList StringList = String.split(SYMBOL_POINT);
	QString IntPart = StringList.front(); //Целая часть
	QString DoublePart = StringList.back(); //Дробная часть

	if (IntPart == DoublePart) //Если целая и дробная части равны - это целое число
	{
		String.setNum(IntPart.toInt()); //Преобразовываем стандартным способом
	}
	else //Иначе дробное
	{
		IntPart.setNum(IntPart.toInt()); //Преобразовываем стандартным способом

		//Учитываем свойство Decimal
		if (DoublePart.size() > Decimal)
		{
			DoublePart.chop(DoublePart.size() - Decimal);
		}

		//Собираем итоговое дробное число
		String = IntPart + SYMBOL_POINT + DoublePart;
	}
	return QDoubleValidator::Acceptable;
}
//-----------------------------------------------------------------------------
