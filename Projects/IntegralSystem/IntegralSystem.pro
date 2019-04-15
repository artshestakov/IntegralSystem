QT += core gui network sql widgets xml multimedia

TARGET = IntegralSystem
CONFIG += c++11 precompile_header
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG(debug, debug|release){
    CONFIGURATION = Debug
}else{
    CONFIGURATION = Release
}

PRECOMPILED_HEADER = $$PWD/../../PublicSource/System/StdAfx.h
DESTDIR = $$PWD/../../Bin/$${CONFIGURATION}-Linux64
MOC_DIR = $$PWD/../../Build/$${TARGET}/$${CONFIGURATION}_Linux64
OBJECTS_DIR = $$PWD/../../Build/$${TARGET}/$${CONFIGURATION}_Linux64
PRECOMPILED_DIR = $$PWD/../../PrecompiledHeaders/$${TARGET}_$${CONFIGURATION}_Linux64

INCLUDEPATH += $$PWD/../../Library/ISCarat
INCLUDEPATH += $$PWD/../../Library/ISConfigurator
INCLUDEPATH += $$PWD/../../Library/ISControls
INCLUDEPATH += $$PWD/../../Library/ISCore
INCLUDEPATH += $$PWD/../../Library/ISFields
INCLUDEPATH += $$PWD/../../Library/ISMetaData
INCLUDEPATH += $$PWD/../../Library/ISNetwork
INCLUDEPATH += $$PWD/../../Library/ISPlugin
INCLUDEPATH += $$PWD/../../Library/ISPostgres
INCLUDEPATH += $$PWD/../../Library/ISPrinting
INCLUDEPATH += $$PWD/../../Library/ISProcess
INCLUDEPATH += $$PWD/../../Library/ISResources
INCLUDEPATH += $$PWD/../../Library/ISSystem
INCLUDEPATH += $$PWD/../../Library/ISTelephony
INCLUDEPATH += $$PWD/!Controls
INCLUDEPATH += $$PWD/!Controls/Fields
INCLUDEPATH += $$PWD/!Controls/FieldSettings
INCLUDEPATH += $$PWD/!Controls/MainCalendar
INCLUDEPATH += $$PWD/!Forms
INCLUDEPATH += $$PWD/!StackTrace
INCLUDEPATH += $$PWD/!System
INCLUDEPATH += $$PWD/../../PublicSource/License
INCLUDEPATH += $$PWD/../../PublicSource/System
INCLUDEPATH += $$PWD/../../PublicSource/Version
INCLUDEPATH += .

LIBS += -L$$DESTDIR -Wl,-rpath="$${DESTDIR}",-rpath-link="$${DESTDIR}" \
    -lISCarat \
    -lISConfigurator \
    -lISControls \
    -lISCore \
    -lISFields \
    -lISMetaData \
    -lISNetwork \
    -lISPlugin \
    -lISPostgres \
    -lISPrinting \
    -lISProcess \
    -lISResources \
    -lISSystem \
    -lISTelephony \

SOURCES += !System/main.cpp \
    !Controls/Fields/ISComboTimeEdit.cpp \
    !Controls/Fields/ISDatabaseEdit.cpp \
    !Controls/Fields/ISTaskExecutorEdit.cpp \
    !Controls/FieldSettings/ISComboSettingCalendarNoteType.cpp \
    !Controls/FieldSettings/ISComboSettingEdit.cpp \
    !Controls/FieldSettings/ISComboSettingSelectionBehavior.cpp \
    !Controls/FieldSettings/ISComboSettingStartedParagraph.cpp \
    !Forms/ISAboutForm.cpp \
    !Forms/ISApplicationsListForm.cpp \
    !Forms/ISAttachFileForm.cpp \
    !Forms/ISAuthorizationForm.cpp \
    !Forms/ISCalculatorForm.cpp \
    !Forms/ISCalendarEventForm.cpp \
    !Forms/ISCalendarForm.cpp \
    !Forms/ISCalendarObjectForm.cpp \
    !Forms/ISChatForm.cpp \
    !Forms/ISConnectionForm.cpp \
    !Forms/ISDatabaseForm.cpp \
    !Forms/ISDesktopForm.cpp \
    !Forms/ISDiscussionListForm.cpp \
    !Forms/ISDiscussionObjectForm.cpp \
    !Forms/ISDistFileListForm.cpp \
    !Forms/ISEditObjectListForm.cpp \
    !Forms/ISEMailInfoForm.cpp \
    !Forms/ISEMailListForm.cpp \
    !Forms/ISEMailObjectForm.cpp \
    !Forms/ISEMailReadForm.cpp \
    !Forms/ISExitForm.cpp \
    !Forms/ISExternalToolsForm.cpp \
    !Forms/ISFastCreateObjectForm.cpp \
    !Forms/ISFeedbackForm.cpp \
    !Forms/ISHistoryForm.cpp \
    !Forms/ISLicenseForm.cpp \
    !Forms/ISLockForm.cpp \
    !Forms/ISMainWindow.cpp \
    !Forms/ISMissingLibraryForm.cpp \
    !Forms/ISMonitorActivityForm.cpp \
    !Forms/ISNoteForm.cpp \
    !Forms/ISNotificationsForm.cpp \
    !Forms/ISParagraphBaseForm.cpp \
    !Forms/ISProtocolEntersForm.cpp \
    !Forms/ISProtocolObjectListForm.cpp \
    !Forms/ISReconnectForm.cpp \
    !Forms/ISServiceWindow.cpp \
    !Forms/ISSettingsDatabaseForm.cpp \
    !Forms/ISSettingsDatabaseObjectForm.cpp \
    !Forms/ISSettingsForm.cpp \
    !Forms/ISStatisticsDatabaseForm.cpp \
    !Forms/ISStatisticTablesForm.cpp \
    !Forms/ISStorageFilesListForm.cpp \
    !Forms/ISTaskAttachedCardsForm.cpp \
    !Forms/ISTaskFilesListForm.cpp \
    !Forms/ISTaskObjectForm.cpp \
    !Forms/ISTasksAllListForm.cpp \
    !Forms/ISTasksBaseListForm.cpp \
    !Forms/ISTasksForm.cpp \
    !Forms/ISTasksFromMeListForm.cpp \
    !Forms/ISTasksMyListForm.cpp \
    !Forms/ISTasksStatusListForm.cpp \
    !Forms/ISUpdateDownloadForm.cpp \
    !Forms/ISUserGroupAccessObjectForm.cpp \
    !Forms/ISUserObjectForm.cpp \
    !Forms/ISUserPasswordForm.cpp \
    !Forms/ISUserListForm.cpp \
    !Forms/ISWorkspaceForm.cpp \
    !Forms/ISCaratForm.cpp \
    !System/ISAbstractMode.cpp \
    !System/ISAutoLocking.cpp \
    !System/ISCrashDumper.cpp \
    !System/ISIntegralSystem.cpp \
    !System/ISLibraryLoader.cpp \
    !System/ISModeService.cpp \
    !System/ISModeUser.cpp \
    !System/ISRegisterMetaType.cpp \
    !System/ISStartup.cpp \
    !System/ISVersionInformer.cpp \
    !Controls/ISSplashScreen.cpp \
    !Controls/ISAttachFileItem.cpp \
    !Controls/ISCalculatorButton.cpp \
    !Controls/MainCalendar/ISCalendarPanel.cpp \
    !Controls/MainCalendar/ISCalendarDayWidget.cpp \
    !Controls/MainCalendar/ISCalendarEventItem.cpp \
    !Controls/ISMenuBar.cpp \
    !Controls/ISChatMessageWidget.cpp \
    !Controls/ISStatusBar.cpp \
    !Controls/ISNotifyWidgetItem.cpp \
    !Controls/ISLicenseMessageWidget.cpp \
    !Controls/ISParagraphButton.cpp \
    !Controls/ISMenuFastAccess.cpp \
    ../../PublicSource/License/ISCrypterLicense.cpp \
    !Controls/ISSystemsPanel.cpp \
    !Controls/ISTasksSqlModel.cpp \
    !Controls/ISMainMenu.cpp \
    !Controls/ISButtonWeather.cpp \
    !Controls/ISButtonCurrency.cpp \
    !Controls/ISButtonMainMenu.cpp \
    !Forms/ISPasswordChangeQueryListForm.cpp \
    !Forms/ISAsteriskCDRListForm.cpp

HEADERS += \
    !Controls/Fields/ISComboTimeEdit.h \
    !Controls/Fields/ISDatabaseEdit.h \
    !Controls/Fields/ISTaskExecutorEdit.h \
    !Controls/FieldSettings/ISComboSettingCalendarNoteType.h \
    !Controls/FieldSettings/ISComboSettingEdit.h \
    !Controls/FieldSettings/ISComboSettingSelectionBehavior.h \
    !Controls/FieldSettings/ISComboSettingStartedParagraph.h \
    !Forms/ISAboutForm.h \
    !Forms/ISApplicationsListForm.h \
    !Forms/ISAttachFileForm.h \
    !Forms/ISAuthorizationForm.h \
    !Forms/ISCalculatorForm.h \
    !Forms/ISCalendarEventForm.h \
    !Forms/ISCalendarForm.h \
    !Forms/ISCalendarObjectForm.h \
    !Forms/ISChatForm.h \
    !Forms/ISConnectionForm.h \
    !Forms/ISDatabaseForm.h \
    !Forms/ISDesktopForm.h \
    !Forms/ISDiscussionListForm.h \
    !Forms/ISDiscussionObjectForm.h \
    !Forms/ISDistFileListForm.h \
    !Forms/ISEditObjectListForm.h \
    !Forms/ISEMailInfoForm.h \
    !Forms/ISEMailListForm.h \
    !Forms/ISEMailObjectForm.h \
    !Forms/ISEMailReadForm.h \
    !Forms/ISExitForm.h \
    !Forms/ISExternalToolsForm.h \
    !Forms/ISFastCreateObjectForm.h \
    !Forms/ISFeedbackForm.h \
    !Forms/ISHistoryForm.h \
    !Forms/ISLicenseForm.h \
    !Forms/ISLockForm.h \
    !Forms/ISMainWindow.h \
    !Forms/ISMissingLibraryForm.h \
    !Forms/ISMonitorActivityForm.h \
    !Forms/ISNoteForm.h \
    !Forms/ISNotificationsForm.h \
    !Forms/ISParagraphBaseForm.h \
    !Forms/ISProtocolEntersForm.h \
    !Forms/ISProtocolObjectListForm.h \
    !Forms/ISReconnectForm.h \
    !Forms/ISServiceWindow.h \
    !Forms/ISSettingsDatabaseForm.h \
    !Forms/ISSettingsDatabaseObjectForm.h \
    !Forms/ISSettingsForm.h \
    !Forms/ISStatisticsDatabaseForm.h \
    !Forms/ISStatisticTablesForm.h \
    !Forms/ISStorageFilesListForm.h \
    !Forms/ISTaskAttachedCardsForm.h \
    !Forms/ISTaskFilesListForm.h \
    !Forms/ISTaskObjectForm.h \
    !Forms/ISTasksAllListForm.h \
    !Forms/ISTasksBaseListForm.h \
    !Forms/ISTasksForm.h \
    !Forms/ISTasksFromMeListForm.h \
    !Forms/ISTasksMyListForm.h \
    !Forms/ISTasksStatusListForm.h \
    !Forms/ISUpdateDownloadForm.h \
    !Forms/ISUserGroupAccessObjectForm.h \
    !Forms/ISUserObjectForm.h \
    !Forms/ISUserPasswordForm.h \
    !Forms/ISUserListForm.h \
    !Forms/ISWorkspaceForm.h \
    !Forms/ISCaratForm.h \
    !System/ISAbstractMode.h \
    !System/ISAutoLocking.h \
    !System/ISCrashDumper.h \
    !System/ISIntegralSystem.h \
    !System/ISLibraryLoader.h \
    !System/ISModeService.h \
    !System/ISModeUser.h \
    !System/ISRegisterMetaType.h \
    !System/ISStartup.h \
    !System/ISVersionInformer.h \
    !Controls/ISAttachFileItem.h \
    !Controls/ISCalculatorButton.h \
    !Controls/MainCalendar/ISCalendarPanel.h \
    !Controls/MainCalendar/ISCalendarDayWidget.h \
    !Controls/MainCalendar/ISCalendarEventItem.h \
    !Controls/ISMenuBar.h \
    !Controls/ISChatMessageWidget.h \
    !Controls/ISStatusBar.h \
    !Controls/ISNotifyWidgetItem.h \
    !Controls/ISLicenseMessageWidget.h \
    !Controls/ISParagraphButton.h \
    !Controls/ISMenuFastAccess.h \
    ../../PublicSource/License/ISCrypterLicense.h \
    !Controls/ISSystemsPanel.h \
    !StackTrace/call_stack.hpp \
    !Controls/ISSplashScreen.h \
    !Controls/ISTasksSqlModel.h \
    !Controls/ISMainMenu.h \
    !Controls/ISButtonWeather.h \
    !Controls/ISButtonCurrency.h \
    !Controls/ISButtonMainMenu.h \
    !Forms/ISPasswordChangeQueryListForm.h \
    !Forms/ISAsteriskCDRListForm.h
