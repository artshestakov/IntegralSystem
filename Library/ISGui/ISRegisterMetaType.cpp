#include "ISRegisterMetaType.h"
//-----------------------------------------------------------------------------
#include "ISUserListForm.h"
#include "ISUserObjectForm.h"
#include "ISProtocolObjectListForm.h"
#include "ISDiscussionListForm.h"
#include "ISControlDatabaseForm.h"
#include "ISStorageFilesListForm.h"
#include "ISCalendarObjectForm.h"
#include "ISDesktopForm.h"
#include "ISWorkspaceForm.h"
#include "ISCalendarForm.h"
#include "ISTaskForm.h"
#include "ISProtocolListForm.h"
#include "ISInternalDirectoriesForm.h"
#include "ISTaskListForm.h"
#include "ISTaskObjectForm.h"
#include "ISUserGroupListForm.h"
#include "ISMonitorActivityForm.h"
#include "ISFullTextSearchForm.h"
#include "ISKnowledgeBaseSubSystem.h"
#include "ISDelegates.h"
//-----------------------------------------------------------------------------
#include "ISFieldEdits.h"
#include "ISComboSearchWidgets.h"
#include "ISSettingFieldWidgets.h"
//-----------------------------------------------------------------------------
void ISRegisterMetaType::RegisterMetaType()
{
	qRegisterMetaType<std::vector<QSqlRecord>>("std::vector<QSqlRecord>");
	qRegisterMetaType<QSqlError>("QSqlError");
	qRegisterMetaType<ISUuid>("ISUuid");

	qRegisterMetaType<ISUserListForm*>("ISUserListForm");
	qRegisterMetaType<ISUserObjectForm*>("ISUserObjectForm");
	qRegisterMetaType<ISProtocolObjectListForm*>("ISProtocolObjectListForm");
	qRegisterMetaType<ISControlDatabaseForm*>("ISControlDatabaseForm");
	qRegisterMetaType<ISDiscussionListForm*>("ISDiscussionListForm");
	qRegisterMetaType<ISStorageFilesListForm*>("ISStorageFilesListForm");
	qRegisterMetaType<ISCalendarObjectForm*>("ISCalendarObjectForm");
	qRegisterMetaType<ISDesktopForm*>("ISDesktopForm");
	qRegisterMetaType<ISWorkspaceForm*>("ISWorkspaceForm");
	qRegisterMetaType<ISCalendarForm*>("ISCalendarForm");
	qRegisterMetaType<ISTaskForm*>("ISTaskForm");
	qRegisterMetaType<ISProtocolListForm*>("ISProtocolListForm");
	qRegisterMetaType<ISInternalDirectoriesForm*>("ISInternalDirectoriesForm");
	qRegisterMetaType<ISTaskListForm*>("ISTaskListForm");
	qRegisterMetaType<ISTaskObjectForm*>("ISTaskObjectForm");
	qRegisterMetaType<ISUserGroupListForm*>("ISUserGroupListForm");
	qRegisterMetaType<ISMonitorActivityForm*>("ISMonitorActivityForm");
	qRegisterMetaType<ISFullTextSearchForm*>("ISFullTextSearchForm");
	qRegisterMetaType<ISKnowledgeBaseSubSystem*>("ISKnowledgeBaseSubSystem");

	qRegisterMetaType<ISDelegateBoolean*>("ISDelegateBoolean");
	qRegisterMetaType<ISDelegateImage*>("ISDelegateImage");
	qRegisterMetaType<ISDelegateColor*>("ISDelegateColor");

	qRegisterMetaType<ISLineEdit*>("ISLineEdit");
	qRegisterMetaType<ISCheckEdit*>("ISCheckEdit"); 
	qRegisterMetaType<ISIntegerEdit*>("ISIntegerEdit");
	qRegisterMetaType<ISDoubleEdit*>("ISDoubleEdit");
	qRegisterMetaType<ISTextEdit*>("ISTextEdit");
	qRegisterMetaType<ISUuidEdit*>("ISUuidEdit");
	qRegisterMetaType<ISDateTimeEdit*>("ISDateTimeEdit");
	qRegisterMetaType<ISDateEdit*>("ISDateEdit");
	qRegisterMetaType<ISTimeEdit*>("ISTimeEdit");
	qRegisterMetaType<ISImageEdit*>("ISImageEdit");
	qRegisterMetaType<ISListEdit*>("ISListEdit");
	qRegisterMetaType<ISPathEditFile*>("ISPathEditFile");
	qRegisterMetaType<ISPathEditDir*>("ISPathEditDir"); 
	qRegisterMetaType<ISPasswordEdit*>("ISPasswordEdit");
	qRegisterMetaType<ISColorEdit*>("ISColorEdit");
	qRegisterMetaType<ISPathEditDir*>("ISPathEditDir");
	qRegisterMetaType<ISWeekDayEdit*>("ISWeekDayEdit");
	qRegisterMetaType<ISINNEdit*>("ISINNEdit");
	qRegisterMetaType<ISOKPOEdit*>("ISOKPOEdit");
	qRegisterMetaType<ISKPPEdit*>("ISKPPEdit");
	qRegisterMetaType<ISVINEdit*>("ISVINEdit");
	qRegisterMetaType<ISOGRNEdit*>("ISOGRNEdit");
	qRegisterMetaType<ISYearEdit*>("ISYearEdit");
	qRegisterMetaType<ISComboTimeEdit*>("ISComboTimeEdit");
	qRegisterMetaType<ISPhoneEdit*>("ISPhoneEdit");
	qRegisterMetaType<ISBIKEdit*>("ISBIKEdit");
	qRegisterMetaType<ISEMailEdit*>("ISEMailEdit");
	qRegisterMetaType<ISDivisionCodeEdit*>("ISDivisionCodeEdit");
	qRegisterMetaType<ISVolumeEdit*>("ISVolumeEdit");
	qRegisterMetaType<ISBirthdayEdit*>("ISBirthdayEdit");
	qRegisterMetaType<ISMonthEdit*>("ISMonthEdit");
	qRegisterMetaType<ISTaskPriorityEdit*>("ISTaskPriorityEdit");
	qRegisterMetaType<ISTaskImportantEdit*>("ISTaskImportantEdit");
	qRegisterMetaType<ISTaskDescriptionEdit*>("ISTaskDescriptionEdit");
	qRegisterMetaType<ISExecutorEdit*>("ISExecutorEdit");
	qRegisterMetaType<ISUrlEdit*>("ISUrlEdit");
	qRegisterMetaType<ISFileEdit*>("ISFileEdit");
	qRegisterMetaType<ISSexEdit*>("ISSexEdit");

	qRegisterMetaType<ISComboSearchBase*>("ISComboSearchBase");
	qRegisterMetaType<ISComboSearchString*>("ISComboSearchString");
	qRegisterMetaType<ISComboSearchNumber*>("ISComboSearchNumber");

	qRegisterMetaType<ISComboTableDateFormat*>("ISComboTableDateFormat");
	qRegisterMetaType<ISComboTableTimeFormat*>("ISComboTableTimeFormat");
	qRegisterMetaType<ISComboTableDoubleClickEvent*>("ISComboTableDoubleClickEvent");
	qRegisterMetaType<ISComboCalendarNoteType*>("ISComboCalendarNoteType");
	qRegisterMetaType<ISComboStartedParagraph*>("ISComboStartedParagraph");
	qRegisterMetaType<ISComboTabBarSelectionBehavior*>("ISComboTabBarSelectionBehavior");
	qRegisterMetaType<ISCheckViewParagraph*>("ISCheckViewParagraph");
}
//-----------------------------------------------------------------------------
