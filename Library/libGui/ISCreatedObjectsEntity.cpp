#include "StdAfx.h"
#include "ISCreatedObjectsEntity.h"
#include "ISDefines.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISSystem.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
ISCreatedObjectsEntity::ISCreatedObjectsEntity() : QObject()
{

}
//-----------------------------------------------------------------------------
ISCreatedObjectsEntity::~ISCreatedObjectsEntity()
{

}
//-----------------------------------------------------------------------------
ISCreatedObjectsEntity& ISCreatedObjectsEntity::GetInstance()
{
	static ISCreatedObjectsEntity CreatedObjectsEntity;
	return CreatedObjectsEntity;
}
//-----------------------------------------------------------------------------
void ISCreatedObjectsEntity::RegisterForm(QWidget *ObjectForm)
{
	ObjectForms.insert(dynamic_cast<ISInterfaceForm*>(ObjectForm)->GetFormUID(), ObjectForm);
}
//-----------------------------------------------------------------------------
void ISCreatedObjectsEntity::UnregisterForm(const QString &FormUID)
{
	ObjectForms.remove(FormUID);
}
//-----------------------------------------------------------------------------
bool ISCreatedObjectsEntity::CheckExistForms()
{
	QList<QWidget*> Forms = ObjectForms.values();
	int CountNotSaved = 0;
	QString DetailedText;

	for (int i = 0; i < Forms.count(); ++i)
	{
		QWidget *ObjectFormBase = Forms.at(i);
		if (ObjectFormBase->property("ModificationFlag").toBool())
		{
			CountNotSaved++;
			DetailedText += QString::number(i + 1) + ") " + ObjectFormBase->windowTitle().remove(SYMBOL_OBJECT_CHANGED) + ";\n";
		}
	}

	if (CountNotSaved)
	{
		emit Existed();
		ISSystem::RemoveLastSymbolFromString(DetailedText, 2);
		DetailedText += ".";

		ISMessageBox MessageBox(QMessageBox::Warning, LOCALIZATION("Warning"), LOCALIZATION("Message.Information.NotSavedObjects").arg(CountNotSaved), QMessageBox::Ok);
		MessageBox.setInformativeText(DetailedText);
		MessageBox.Exec();

		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
