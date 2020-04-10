#include "ISCreatedObjectsEntity.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISSystem.h"
#include "ISInterfaceForm.h"
#include "ISConstants.h"
#include "ISDefinesCore.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISCreatedObjectsEntity::ISCreatedObjectsEntity() : QObject()
{

}
//-----------------------------------------------------------------------------
ISCreatedObjectsEntity::~ISCreatedObjectsEntity()
{

}
//-----------------------------------------------------------------------------
ISCreatedObjectsEntity& ISCreatedObjectsEntity::Instance()
{
	static ISCreatedObjectsEntity CreatedObjectsEntity;
	return CreatedObjectsEntity;
}
//-----------------------------------------------------------------------------
void ISCreatedObjectsEntity::RegisterForm(QWidget *ObjectForm)
{
	ObjectForms.emplace(dynamic_cast<ISInterfaceForm*>(ObjectForm)->GetFormUID(), ObjectForm);
}
//-----------------------------------------------------------------------------
void ISCreatedObjectsEntity::UnregisterForm(const QString &FormUID)
{
	ObjectForms.erase(FormUID);
}
//-----------------------------------------------------------------------------
bool ISCreatedObjectsEntity::CheckExistForms()
{
	std::vector<QWidget *> Forms = ISAlgorithm::ConvertMapToValues<ISUuid, QWidget *>(ObjectForms);
	int CountNotSaved = 0;
	QString DetailedText;

	for (int i = 0; i < Forms.size(); ++i)
	{
		QWidget *ObjectFormBase = Forms[i];
		if (ObjectFormBase->property("ModificationFlag").toBool())
		{
			++CountNotSaved;
			DetailedText += QString::number(i + 1) + ") " + ObjectFormBase->windowTitle().remove(ISDefines::Core::SYMBOL_OBJECT_CHANGED) + ";\n";
		}
	}

	if (CountNotSaved)
	{
		emit Existed();
		ISSystem::RemoveLastSymbolFromString(DetailedText, 2);
		DetailedText += SYMBOL_POINT;

		ISMessageBox MessageBox(QMessageBox::Warning, LANG("Warning"), LANG("Message.Information.NotSavedObjects").arg(CountNotSaved), QMessageBox::Ok);
		MessageBox.setInformativeText(DetailedText);
		MessageBox.Exec();

		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
