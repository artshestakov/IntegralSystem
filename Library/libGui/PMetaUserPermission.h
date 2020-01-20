#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class PMetaUserPermission : public QObject
{
	Q_OBJECT

public:
	PMetaUserPermission(QObject *parent);
	virtual ~PMetaUserPermission();

	void SetSubSystemUID(const QString &sub_system_uid);
	QString GetSubSystemUID() const;

	void SetTableUID(const QString &table_uid);
	QString GetTableUID() const;

	void SetShow(bool show);
	bool GetShow() const;

	void SetCreate(bool create);
	bool GetCreate() const;

	void SetCreateCopy(bool create_copy);
	bool GetCreateCopy() const;

	void SetEdit(bool edit);
	bool GetEdit() const;

	void SetDelete(bool del);
	bool GetDelete() const;

	void SetDeleteCascade(bool delete_cascade);
	bool GetDeleteCascade() const;

	void SetUpdateList(bool update_list);
	bool GetUpdateList() const;

	void SetShowDeleted(bool show_deleted);
	bool GetShowDeleted() const;

	void SetShowAll(bool show_all);
	bool GetShowAll() const;

	void SetSearch(bool search);
	bool GetSearch() const;

	void SetExport(bool export_list);
	bool GetExport() const;

	void SetPrint(bool print);
	bool GetPrint() const;

	void SetSystemInformation(bool system_information);
	bool GetSystemInformation() const;

	void SetAttachTask(bool attach_task);
	bool GetAttackTask() const;

	void SetTableNavigation(bool table_navigation);
	bool GetTableNavigation() const;

private:
	QString SubSystemUID;
	QString TableUID;

	bool Show;
	bool Create;
	bool CreateCopy;
	bool Edit;
	bool Delete;
	bool DeleteCascade;
	bool UpdateList;
	bool ShowDeleted;
	bool ShowAll;
	bool Search;
	bool Export;
	bool Print;
	bool SystemInformation;
	bool AttachTask;
	bool TableNavigation;
};
//-----------------------------------------------------------------------------
