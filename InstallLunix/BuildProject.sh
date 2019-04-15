Configuration=$2
echo -e "\033[1m"Сборка проекта $1 в конфигурации $Configuration..."\033[0m"

#Удаление старого бинарника
rm -f ../Bin/$2-Linux64/$1.so

$QT_DIR_BIN/qmake ../Projects/$1/$1.pro -spec linux-g++ CONFIG+=${Configuration,,}
make


if [ $? -eq 0 ]
then
	echo -e "\033[1m"Проект $1 собран успешно: конфигурация $Configuration"\033[0m"
	echo ""

	#Копирование скомпилированных файлов в папку Deploy
	cp -f ../Bin/$2-Linux64/$1 ../Deploy/$2-Linux64/$1
else
	echo -e "\033[1m"Ошибка при сборке проекта $1 в конфигурации $Configuration. Для продолжения нажмите клавишу Enter . . ."\033[0m"; read
fi
