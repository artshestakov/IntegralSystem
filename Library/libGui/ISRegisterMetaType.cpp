#include "ISRegisterMetaType.h"
//-----------------------------------------------------------------------------
#include "ISListObjectForm.h"
#include "ISUserListForm.h"
#include "ISUserObjectForm.h"
#include "ISProtocolObjectListForm.h"
#include "ISEditObjectListForm.h"
#include "ISDiscussionListForm.h"
#include "ISControlDatabaseForm.h"
#include "ISStorageFilesListForm.h"
#include "ISCalendarObjectForm.h"
#include "ISDesktopForm.h"
#include "ISWorkspaceForm.h"
#include "ISCalendarForm.h"
#include "ISChatForm.h"
#include "ISTaskForm.h"
#include "ISProtocolListForm.h"
#include "ISSettingsDatabaseObjectForm.h"
#include "ISCaratForm.h"
#include "ISInternalDirectoriesForm.h"
#include "ISAsteriskCallsListForm.h"
#include "ISTaskAllListForm.h"
#include "ISTaskMyListForm.h"
#include "ISTaskFromListForm.h"
#include "ISTaskFavoriteListForm.h"
#include "ISTaskTodayListForm.h"
#include "ISTaskOverdueListForm.h"
#include "ISTaskObjectForm.h"
#include "ISTelephonyForm.h"
#include "ISDeviceUserObjectForm.h"
#include "ISUserGroupListForm.h"
#include "ISMonitorActivityForm.h"
#include "ISFullTextSearchForm.h"
#include "ISKnowledgeBaseSubSystem.h"
#include "ISDelegates.h"
//-----------------------------------------------------------------------------
#include "ISLineEdit.h"
#include "ISCheckEdit.h"
#include "ISIntegerEdit.h"
#include "ISDoubleEdit.h"
#include "ISTextEdit.h"
#include "ISUuidEdit.h"
#include "ISDateTimeEdit.h"
#include "ISDateEdit.h"
#include "ISTimeEdit.h"
#include "ISImageEdit.h"
#include "ISImageEdit64.h"
#include "ISListEdit.h"
#include "ISIPAddressEdit.h"
#include "ISPathEditFile.h"
#include "ISPathEditDir.h"
#include "ISPasswordEdit.h"
#include "ISColorEdit.h"
#include "ISWeekDayEdit.h"
#include "ISINNEdit.h"
#include "ISOKPOEdit.h"
#include "ISKPPEdit.h"
#include "ISOGRNEdit.h"
#include "ISYearEdit.h"
#include "ISComboTimeEdit.h"
#include "ISPhoneBaseEdit.h"
#include "ISPhoneEdit.h"
#include "ISUserEdit.h"
#include "ISBIKEdit.h"
#include "ISICQEdit.h"
#include "ISEMailEdit.h"
#include "ISPassportEdit.h"
#include "ISDivisionCodeEdit.h"
#include "ISVolumeEdit.h"
#include "ISBirthdayEdit.h"
#include "ISMonthEdit.h"
#include "ISTaskNameEdit.h"
#include "ISTaskPriorityEdit.h"
#include "ISTaskImportantEdit.h"
#include "ISTaskDescriptionEdit.h"
#include "ISExecutorEdit.h"
#include "ISLoginEdit.h"
#include "ISPercentEdit.h"
#include "ISAddressEdit.h"
#include "ISUrlEdit.h"
#include "ISFileEdit.h"
#include "ISComboSearchWidgets.h"
#include "ISSettingFieldWidgets.h"
//-----------------------------------------------------------------------------
#include "ISDevice_Atol1101.h"
#include "ISDevice_Atol1101Plus.h"
#include "ISDevice_Atol30F.h"
//-----------------------------------------------------------------------------
void ISRegisterMetaType::RegisterMetaType()
{
	qRegisterMetaType<std::vector<QSqlRecord>>("std::vector<QSqlRecord>");
	qRegisterMetaType<QSqlError>("QSqlError");
	qRegisterMetaType<ISUuid>("ISUuid");

	qRegisterMetaType<ISListObjectForm*>("ISListObjectForm");
	qRegisterMetaType<ISUserListForm*>("ISUserListForm");
	qRegisterMetaType<ISUserObjectForm*>("ISUserObjectForm");
	qRegisterMetaType<ISProtocolObjectListForm*>("ISProtocolObjectListForm");
	qRegisterMetaType<ISEditObjectListForm*>("ISEditObjectListForm");
	qRegisterMetaType<ISControlDatabaseForm*>("ISControlDatabaseForm");
	qRegisterMetaType<ISDiscussionListForm*>("ISDiscussionListForm");
	qRegisterMetaType<ISStorageFilesListForm*>("ISStorageFilesListForm");
	qRegisterMetaType<ISCalendarObjectForm*>("ISCalendarObjectForm");
	qRegisterMetaType<ISDesktopForm*>("ISDesktopForm");
	qRegisterMetaType<ISWorkspaceForm*>("ISWorkspaceForm");
	qRegisterMetaType<ISCalendarForm*>("ISCalendarForm");
	qRegisterMetaType<ISChatForm*>("ISChatForm");
	qRegisterMetaType<ISTaskForm*>("ISTaskForm");
	qRegisterMetaType<ISProtocolListForm*>("ISProtocolListForm");
	qRegisterMetaType<ISSettingsDatabaseObjectForm*>("ISSettingsDatabaseObjectForm");
	qRegisterMetaType<ISCaratForm*>("ISCaratForm");
	qRegisterMetaType<ISInternalDirectoriesForm*>("ISInternalDirectoriesForm");
	qRegisterMetaType<ISAsteriskCallsListForm*>("ISAsteriskCallsListForm");
	qRegisterMetaType<ISTaskAllListForm*>("ISTaskAllListForm");
	qRegisterMetaType<ISTaskMyListForm*>("ISTaskMyListForm");
	qRegisterMetaType<ISTaskFromListForm*>("ISTaskFromListForm");
	qRegisterMetaType<ISTaskFavoriteListForm*>("ISTaskFavoriteListForm");
	qRegisterMetaType<ISTaskTodayListForm*>("ISTaskTodayListForm");
	qRegisterMetaType<ISTaskOverdueListForm*>("ISTaskOverdueListForm");
	qRegisterMetaType<ISTaskObjectForm*>("ISTaskObjectForm");
	qRegisterMetaType<ISTelephonyForm*>("ISTelephonyForm");
	qRegisterMetaType<ISDeviceUserObjectForm*>("ISDeviceUserObjectForm");
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
	qRegisterMetaType<ISImageEdit64*>("ISImageEdit64");
	qRegisterMetaType<ISListEdit*>("ISListEdit");
	qRegisterMetaType<ISIPAddressEdit*>("ISIPAddressEdit");
	qRegisterMetaType<ISPathEditFile*>("ISPathEditFile");
	qRegisterMetaType<ISPathEditDir*>("ISPathEditDir"); 
	qRegisterMetaType<ISPasswordEdit*>("ISPasswordEdit");
	qRegisterMetaType<ISColorEdit*>("ISColorEdit");
	qRegisterMetaType<ISPathEditDir*>("ISPathEditDir");
	qRegisterMetaType<ISWeekDayEdit*>("ISWeekDayEdit");
	qRegisterMetaType<ISINNEdit*>("ISINNEdit");
	qRegisterMetaType<ISOKPOEdit*>("ISOKPOEdit");
	qRegisterMetaType<ISKPPEdit*>("ISKPPEdit");
	qRegisterMetaType<ISOGRNEdit*>("ISOGRNEdit");
	qRegisterMetaType<ISYearEdit*>("ISYearEdit");
	qRegisterMetaType<ISComboTimeEdit*>("ISComboTimeEdit");
	qRegisterMetaType<ISPhoneBaseEdit*>("ISPhoneBaseEdit");
	qRegisterMetaType<ISPhoneEdit*>("ISPhoneEdit");
	qRegisterMetaType<ISUserEdit*>("ISUserEdit");
	qRegisterMetaType<ISBIKEdit*>("ISBIKEdit");
	qRegisterMetaType<ISICQEdit*>("ISICQEdit");
	qRegisterMetaType<ISEMailEdit*>("ISEMailEdit");
	qRegisterMetaType<ISPassportEdit*>("ISPassportEdit");
	qRegisterMetaType<ISDivisionCodeEdit*>("ISDivisionCodeEdit");
	qRegisterMetaType<ISVolumeEdit*>("ISVolumeEdit");
	qRegisterMetaType<ISBirthdayEdit*>("ISBirthdayEdit");
	qRegisterMetaType<ISMonthEdit*>("ISMonthEdit");
	qRegisterMetaType<ISTaskNameEdit*>("ISTaskNameEdit");
	qRegisterMetaType<ISTaskPriorityEdit*>("ISTaskPriorityEdit");
	qRegisterMetaType<ISTaskImportantEdit*>("ISTaskImportantEdit");
	qRegisterMetaType<ISTaskDescriptionEdit*>("ISTaskDescriptionEdit");
	qRegisterMetaType<ISExecutorEdit*>("ISExecutorEdit");
	qRegisterMetaType<ISLoginEdit*>("ISLoginEdit");
	qRegisterMetaType<ISPercentEdit*>("ISPercentEdit");
	qRegisterMetaType<ISAddressEdit*>("ISAddressEdit");
	qRegisterMetaType<ISUrlEdit*>("ISUrlEdit");
	qRegisterMetaType<ISFileEdit*>("ISFileEdit");

	qRegisterMetaType<ISComboSearchBase*>("ISComboSearchBase");
	qRegisterMetaType<ISComboSearchString*>("ISComboSearchString");
	qRegisterMetaType<ISComboSearchNumber*>("ISComboSearchNumber");

	qRegisterMetaType<ISComboTableSelectionBehavior*>("ISComboTableSelectionBehavior");
	qRegisterMetaType<ISComboTableDateFormat*>("ISComboTableDateFormat");
	qRegisterMetaType<ISComboTableDoubleClickEvent*>("ISComboTableDoubleClickEvent");
	qRegisterMetaType<ISComboCalendarNoteType*>("ISComboCalendarNoteType");
	qRegisterMetaType<ISComboStartedParagraph*>("ISComboStartedParagraph");
	qRegisterMetaType<ISComboTabBarSelectionBehavior*>("ISComboTabBarSelectionBehavior");
	qRegisterMetaType<ISComboIncomingFormPosition*>("ISComboIncomingFormPosition");
	qRegisterMetaType<ISCheckViewParagraph*>("ISCheckViewParagraph");

	qRegisterMetaType<ISDevice_Atol1101*>("ISDevice_Atol1101");
	qRegisterMetaType<ISDevice_Atol1101Plus*>("ISDevice_Atol1101Plus");
	qRegisterMetaType<ISDevice_Atol30F*>("ISDevice_Atol30F");
}
//-----------------------------------------------------------------------------
