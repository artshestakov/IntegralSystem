#!/bin/bash

#Скрипт для восстановления базы данных.
#Он принимает два параметра: DB_NAME и PATH.
#DB_NAME - имя базы данных, в котороую будет выполнено восстановление
#PATH - путь к архиву с резервной копией
#Пример запуска скрипта: ./RestoreDatabase.sh test_db /home/user/file.dmp

pg_restore --verbose --exit-on-error --host=127.0.0.1 --port=5432 --role=postgres --username=postgres --dbname=$1 $2
