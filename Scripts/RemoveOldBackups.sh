#Пример запуска скрипта ./RemoveOldBackups.sh [FOLDER] [DAYS]
#Где:
#	[FOLDER] - путь к директории со слешем в конце.
#	[DAYS] - количество дней, за которые нужно хранить резервные копии.
#
# ./RemoveOldBackups.sh /home/user/backups/ 10
# В примере выше удаляются все резервные копии в папке /home/user/backups/ кроме как за последние 10 дней

#Директория
DIRECTORY=$1

#Количество дней
Days=$2

#Начальный индекс
Index=1

#Получаем список файлов в директории
FILES=($(ls -1t $DIRECTORY))

#Обходим список файлов
for File in "${FILES[@]}"
do

	#Если индекс больше Days - удаляем файл
	if [ "$Index" -gt "$Days" ]; then
		rm $DIRECTORY$File
	fi;
	
	#Инкрементируем индекс
	Index=$((Index + 1))
done;
