export INTEGRAL_SYSTEM_DIR="/home/artem/IntegralSystem"
export QT_DIR="/home/artem/Qt"
export QT_DIR_BIN="$QT_DIR/5.11.3/gcc_64/bin"
export QT_DIR_LIB="$QT_DIR/5.11.3/gcc_64/lib"
export QT_DIR_PLUGINS="$QT_DIR/5.11.3/gcc_64/plugins"

Configuration=$1

#Создание директории сборки
mkdir -p ../Deploy
mkdir -p ../Deploy/$1-Linux64
rm -R -f ../Deploy/$1-Linux64/*

mkdir -p ../Deploy/$1-Linux64/translations
$QT_DIR_BIN/lconvert -o ../Deploy/$1-Linux64/translations/qt_ru.qm $QT_DIR_BIN/../translations/assistant_ru.qm $QT_DIR_BIN/../translations/designer_ru.qm $QT_DIR_BIN/../translations/linguist_ru.qm $QT_DIR_BIN/../translations/qmlviewer_ru.qm $QT_DIR_BIN/../translations/qtbase_ru.qm $QT_DIR_BIN/../translations/qtconfig_ru.qm $QT_DIR_BIN/../translations/qtconnectivity_ru.qm $QT_DIR_BIN/../translations/qtdeclarative_ru.qm $QT_DIR_BIN/../translations/qtlocation_ru.qm $QT_DIR_BIN/../translations/qtmultimedia_ru.qm $QT_DIR_BIN/../translations/qtquick1_ru.qm $QT_DIR_BIN/../translations/qtquickcontrols_ru.qm $QT_DIR_BIN/../translations/qtscript_ru.qm $QT_DIR_BIN/../translations/qtserialport_ru.qm $QT_DIR_BIN/../translations/qtwebengine_ru.qm $QT_DIR_BIN/../translations/qtwebsockets_ru.qm $QT_DIR_BIN/../translations/qtxmlpatterns_ru.qm

#Компиляция библиотек проекта
./BuildLibrary.sh ISCore $1
./BuildLibrary.sh ISCarat $1

./BuildProject.sh CaratAsteriskQueue $1
./BuildProject.sh CaratAsteriskRecord $1
./BuildProject.sh CaratCalendar $1
./BuildProject.sh CaratCenterSeven $1
./BuildProject.sh CaratNotification $1
./BuildProject.sh CaratScheduler $1
./BuildProject.sh CaratTelephony $1

./BuildProject.sh CaratHighway $1
./BuildProject.sh CaratInformResource $1
./BuildProject.sh CaratMedTech $1

./BuildProject.sh Carat $1
./BuildProject.sh CaratWorker $1
./BuildProject.sh Configurator $1
./BuildProject.sh ClientApplication $1

#Формирование версии
#MAJOR=$(cat ../Resources/Version/Major.txt)
#MINOR=$(cat ../Resources/Version/Minor.txt)
#REVISION=$(cat ../Resources/Version/Revision.txt)
#BUILD=$(cat ../Resources/Version/Build.txt)

#Сборка RPM-пакета
QA_RPATHS=0x0001 rpmbuild -bb IntegralSystem.spec --define "Configuration $1"
# --define "Version $MAJOR.$MINOR.$REVISION.$BUILD"

#Удаление временных файлов
if [ -e ./Makefile ] ; then
	rm ./Makefile
fi
