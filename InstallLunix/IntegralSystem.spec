%{!?Configuration: %define Configuration none}
%{!?Version: %define Version 1.0.0.0}

Name: IntegralSystem
Version: %{Version}
Release: 1
Group: Utilities/System
Group: Applications/Communications
Vendor: Shestakov Artem
License: GPL
Summary(ru): Система управления данными IntegralSystem
BuildRequires: telnet

%description
Not specified

%description -l ru
Not specified

%clean
rm -rf $RPM_BUILD_ROOT

%install
mkdir -p $RPM_BUILD_ROOT/opt/IntegralSystem
#mkdir -p $RPM_BUILD_ROOT/opt/IntegralSystem/audio
#mkdir -p $RPM_BUILD_ROOT/opt/IntegralSystem/mediaservice
#mkdir -p $RPM_BUILD_ROOT/opt/IntegralSystem/playlistformats
mkdir -p $RPM_BUILD_ROOT/opt/IntegralSystem/sqldrivers
mkdir -p $RPM_BUILD_ROOT/opt/IntegralSystem/translations

mkdir -p $RPM_BUILD_ROOT/opt/IntegralSystem/Licenses

#cp $QT_DIR_PLUGINS/audio/* $RPM_BUILD_ROOT/opt/IntegralSystem/audio
#cp $QT_DIR_PLUGINS/mediaservice/* $RPM_BUILD_ROOT/opt/IntegralSystem/mediaservice
#cp $QT_DIR_PLUGINS/playlistformats/* $RPM_BUILD_ROOT/opt/IntegralSystem/playlistformats
cp $QT_DIR_PLUGINS/sqldrivers/libqsqlpsql.so $RPM_BUILD_ROOT/opt/IntegralSystem/sqldrivers

#cp $QT_DIR_LIB/libqgsttools_p.so.1 $RPM_BUILD_ROOT/opt/IntegralSystem
cp $QT_DIR_LIB/libicudata.so.56 $RPM_BUILD_ROOT/opt/IntegralSystem
cp $QT_DIR_LIB/libicui18n.so.56 $RPM_BUILD_ROOT/opt/IntegralSystem
cp $QT_DIR_LIB/libicuuc.so.56 $RPM_BUILD_ROOT/opt/IntegralSystem

cp $QT_DIR_LIB/libQt5Core.so.5 $RPM_BUILD_ROOT/opt/IntegralSystem
cp $QT_DIR_LIB/libQt5Network.so.5 $RPM_BUILD_ROOT/opt/IntegralSystem
cp $QT_DIR_LIB/libQt5Sql.so.5 $RPM_BUILD_ROOT/opt/IntegralSystem
cp $QT_DIR_LIB/libQt5Xml.so.5 $RPM_BUILD_ROOT/opt/IntegralSystem

cp -r $INTEGRAL_SYSTEM_DIR/Deploy/%{Configuration}-Linux64/* $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/Bin/%{Configuration}-Linux64/*.so* $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/Bin/%{Configuration}-Linux64/Carat $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/Bin/%{Configuration}-Linux64/Configurator $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/InstallResources/Carat.service $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/InstallResources/Carat.sh $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/InstallResources/Configurator.sh $RPM_BUILD_ROOT/opt/IntegralSystem
cp $INTEGRAL_SYSTEM_DIR/Resources/Licenses/* $RPM_BUILD_ROOT/opt/IntegralSystem/Licenses

%files
/opt/IntegralSystem/*

%post
cp --force /opt/IntegralSystem/Carat.service /etc/systemd/system/Carat.service
systemctl daemon-reload
chkconfig Carat on
