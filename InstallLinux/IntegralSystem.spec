Name: IntegralSystem
Version: %{VERSION}
Release: 1
Group: Utilities/System
Group: Applications/Communications
Vendor: Shestakov Artem
License: GPL
Summary(ru): Сервер системы управления данными IntegralSystem

%description
Not specified

%description -l ru
Not specified

%clean
rm -rf $RPM_BUILD_ROOT

%install
mkdir -p $RPM_BUILD_ROOT/opt/IntegralSystem
mkdir -p $RPM_BUILD_ROOT/opt/IntegralSystem/sqldrivers
mkdir -p $RPM_BUILD_ROOT/opt/IntegralSystem/Licenses
mkdir -p $RPM_BUILD_ROOT/etc/systemd/system

cp $QT_DIR/plugins/sqldrivers/libqsqlpsql.so $RPM_BUILD_ROOT/opt/IntegralSystem/sqldrivers
cp $QT_DIR/lib/libicudata.so.56 $RPM_BUILD_ROOT/opt/IntegralSystem
cp $QT_DIR/lib/libicui18n.so.56 $RPM_BUILD_ROOT/opt/IntegralSystem
cp $QT_DIR/lib/libicuuc.so.56 $RPM_BUILD_ROOT/opt/IntegralSystem
cp $QT_DIR/lib/libQt5Core.so.5 $RPM_BUILD_ROOT/opt/IntegralSystem
cp $QT_DIR/lib/libQt5Network.so.5 $RPM_BUILD_ROOT/opt/IntegralSystem
cp $QT_DIR/lib/libQt5Sql.so.5 $RPM_BUILD_ROOT/opt/IntegralSystem
cp $QT_DIR/lib/libQt5Xml.so.5 $RPM_BUILD_ROOT/opt/IntegralSystem

cp -r $INTEGRAL_SYSTEM_DIR/Deploy/%{CONFIGURATION}-%{PLATFORM}/* $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/Bin/%{CONFIGURATION}-%{PLATFORM}/*.so* $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/Bin/%{CONFIGURATION}-%{PLATFORM}/Carat $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/Bin/%{CONFIGURATION}-%{PLATFORM}/Configurator $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/Components/BackuperDB/%{CONFIGURATION}-%{PLATFORM}/BackuperDB $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/InstallLinux/Carat.service $RPM_BUILD_ROOT/etc/systemd/system
cp $INTEGRAL_SYSTEM_DIR/Scripts/RemoveOldBackups.sh $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/Scripts/WatchTodayLog.sh $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/Resources/Licenses/* $RPM_BUILD_ROOT/opt/IntegralSystem/Licenses

%files
/opt/IntegralSystem/*
/etc/systemd/system/Carat.service

#Сценарий перед установкой пакета
%pre

#Если сервис запущен - выходим с ошибкой
Status=$(systemctl is-active Carat)
if [ $Status = "active" ]; then
	echo "ERROR: Carat service is running! You need to stop it and proceed with the installation. Example stop command: \"service Carat stop\"."
	exit 1
fi

#Сценарий после установки пакета
%post
systemctl daemon-reload
systemctl reset-failed

#Сценарий после удаления пакета
%postun
systemctl daemon-reload
systemctl reset-failed
