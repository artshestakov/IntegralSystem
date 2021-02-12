SELECT 
name AS "Имя параметра", 
category AS "Категория", 
setting AS "Значение", 
unit AS "Единица измерения", 
short_desc AS "Описание", 
extra_desc AS "Расширенное описание", 
vartype AS "Тип данных", 
min_val AS "Минимальное значение", 
max_val AS "Максимальное значение", 
enumvals  AS "Допустимые значения" 
FROM pg_settings 
ORDER BY category, name