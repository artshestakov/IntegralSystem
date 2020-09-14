@ECHO off
SetLocal

REM Скрипт для восстановления базы данных.
REM Он принимает два параметра: DB_NAME и PATH.
REM DB_NAME - имя базы данных, в котороую будет выполнено восстановление
REM PATH - путь к архиву с резервной копией
REM Пример запуска скрипта: RestoreDatabase.cmd test_db C:\file.dmp

SET DB_NAME=%1
SET PATH=%2
pg_restore.exe --verbose --exit-on-error --host=127.0.0.1 --port=5432 --role=postgres --username=postgres --dbname=%DB_NAME% %PATH%