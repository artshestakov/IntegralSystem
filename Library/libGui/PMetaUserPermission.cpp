#include "PMetaUserPermission.h"
//-----------------------------------------------------------------------------
PMetaUserPermission::PMetaUserPermission()
	: Show(false),
	CreateCopy(false),
	Edit(false),
	Delete(false),
	DeleteCascade(false),
	UpdateList(false),
	ShowDeleted(false),
	ShowAll(false),
	Search(false),
	Export(false),
	Print(false),
	SystemInformation(false),
	AttachTask(false),
	TableNavigation(false)
{
	
}
//-----------------------------------------------------------------------------
PMetaUserPermission::~PMetaUserPermission()
{

}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetSubSystemUID(const QString &sub_system_uid)
{
	SubSystemUID = sub_system_uid;
}
//-----------------------------------------------------------------------------
QString PMetaUserPermission::GetSubSystemUID() const
{
	return SubSystemUID;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetTableUID(const QString &table_uid)
{
	TableUID = table_uid;
}
//-----------------------------------------------------------------------------
QString PMetaUserPermission::GetTableUID() const
{
	return TableUID;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetShow(bool show)
{
	Show = show;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetShow() const
{
	return Show;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetCreate(bool create)
{
	Create = create;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetCreate() const
{
	return Create;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetCreateCopy(bool create_copy)
{
	CreateCopy = create_copy;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetCreateCopy() const
{
	return CreateCopy;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetEdit(bool edit)
{
	Edit = edit;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetEdit() const
{
	return Edit;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetDelete(bool del)
{
	Delete = del;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetDelete() const
{
	return Delete;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetDeleteCascade(bool delete_cascade)
{
	DeleteCascade = delete_cascade;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetDeleteCascade() const
{
	return DeleteCascade;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetUpdateList(bool update_list)
{
	UpdateList = update_list;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetUpdateList() const
{
	return UpdateList;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetShowDeleted(bool show_deleted)
{
	ShowDeleted = show_deleted;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetShowDeleted() const
{
	return ShowDeleted;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetShowAll(bool show_all)
{
	ShowAll = show_all;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetShowAll() const
{
	return ShowAll;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetSearch(bool search)
{
	Search = search;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetSearch() const
{
	return Search;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetExport(bool export_list)
{
	Export = export_list;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetExport() const
{
	return Export;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetPrint(bool print)
{
	Print = print;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetPrint() const
{
	return Print;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetSystemInformation(bool system_information)
{
	SystemInformation = system_information;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetSystemInformation() const
{
	return SystemInformation;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetAttachTask(bool attach_task)
{
	AttachTask = attach_task;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetAttackTask() const
{
	return AttachTask;
}
//-----------------------------------------------------------------------------
void PMetaUserPermission::SetTableNavigation(bool table_navigation)
{
	TableNavigation = table_navigation;
}
//-----------------------------------------------------------------------------
bool PMetaUserPermission::GetTableNavigation() const
{
	return TableNavigation;
}
//-----------------------------------------------------------------------------
