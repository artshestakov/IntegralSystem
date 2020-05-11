Configuration=$2
echo -e "\033[1m"Сборка библиотеки $1 в конфигурации $Configuration..."\033[0m"

#Удаление старых бинарников
rm -f ../Bin/$2-Linux64/lib$1.so
rm -f ../Bin/$2-Linux64/lib$1.so.1
rm -f ../Bin/$2-Linux64/lib$1.so.1.0
rm -f ../Bin/$2-Linux64/lib$1.so.1.0.0

#Сборка библиотеки
$QT_DIR_BIN/qmake ../Library/$1/$1.pro -spec linux-g++ CONFIG+=${Configuration,,}
make -j2

if [ $? -eq 0 ]
then
	echo -e "\033[1m"Библиотека $1 собрана успешно: конфигурация $Configuration"\033[0m"
	echo ""

	#Копирование скомпилированных файлов в папку Deploy
	cp -f ../Bin/$2-Linux64/lib$1.so ../Deploy/$2-Linux64/lib$1.so
	cp -f ../Bin/$2-Linux64/lib$1.so.1 ../Deploy/$2-Linux64/lib$1.so.1
	cp -f ../Bin/$2-Linux64/lib$1.so.1.0.0 ../Deploy/$2-Linux64/lib$1.so.1.0.0
	cp -f ../Bin/$2-Linux64/lib$1.so.1.0 ../Deploy/$2-Linux64/lib$1.so.1.0
else
	echo -e "\033[1m"Ошибка при сборке библиотеки $1 в конфигурации $Configuration. Для продолжения нажмите клавишу Enter . . ."\033[0m"; read
fi
