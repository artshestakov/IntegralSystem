#include "ISValidators.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISDoubleValidator::ISDoubleValidator(int Decimals, QObject *parent) : QDoubleValidator(INT_MIN, INT_MAX, Decimals, parent)
{
	setNotation(QDoubleValidator::StandardNotation);
	setLocale(QLocale::English);
}
//-----------------------------------------------------------------------------
ISDoubleValidator::~ISDoubleValidator()
{

}
//-----------------------------------------------------------------------------
QValidator::State ISDoubleValidator::validate(QString &String, int &Position) const
{
	return String.contains(SYMBOL_COMMA) ? QDoubleValidator::Invalid : QDoubleValidator::validate(String, Position);
}
//-----------------------------------------------------------------------------
