#include "ISCreatedObjectsEntity.h"
#include "ISDialogsCommon.h"
#include "ISLocalization.h"
#include "ISInterfaceForm.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISGui.h"
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
void ISCreatedObjectsEntity::RegisterForm(ISObjectFormBase *ObjectForm)
{
	ObjectForms.emplace(dynamic_cast<ISInterfaceForm*>(ObjectForm)->GetFormUID(), ObjectForm);
}
//-----------------------------------------------------------------------------
void ISCreatedObjectsEntity::UnregisterForm(const QString &FormUID)
{
	auto It = ObjectForms.find(FormUID);
	if (It != ObjectForms.end())
	{
		ObjectForms.erase(It);
	}
}
//-----------------------------------------------------------------------------
bool ISCreatedObjectsEntity::CheckExistForms()
{
	std::vector<ISObjectFormBase*> Forms = ISAlgorithm::ConvertMapToValues<ISUuid, ISObjectFormBase*>(ObjectForms);
	int CountNotSaved = 0;
	QString DetailedText;

	for (size_t i = 0; i < Forms.size(); ++i)
	{
		ISObjectFormBase *ObjectFormBase = Forms[i];
		if (ObjectFormBase->GetModificationFlag())
		{
			++CountNotSaved;
			DetailedText += QString::number(i + 1) + ") " + ObjectFormBase->windowTitle().remove(SYMBOL_OBJECT_CHANGED) + ";\n";
			if (!ObjectFormBase->parentWidget()) //Если форма не является вкладкой, а отдельным окном
			{
				//Если форма свернута - активируем её, иначе - переводим на передний план
				ObjectFormBase->isMinimized() ?
					ObjectFormBase->setWindowState(ObjectFormBase->windowState() & ~Qt::WindowMinimized | Qt::WindowActive) :
					ObjectFormBase->activateWindow();
			}
		}
	}

	bool Result = !CountNotSaved;
	if (!Result)
	{
		emit Existed();
		DetailedText.chop(2);
		DetailedText += SYMBOL_POINT;

		ISMessageBox MessageBox(QMessageBox::Warning, LANG("Warning"), LANG("Message.Information.NotSavedObjects").arg(CountNotSaved), QString(), { { 1, "OK" } });
		MessageBox.setInformativeText(DetailedText);
		MessageBox.Exec();
	}
	return Result;
}
//-----------------------------------------------------------------------------
