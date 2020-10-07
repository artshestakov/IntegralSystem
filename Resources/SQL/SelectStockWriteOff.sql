SELECT 
impl_date AS "Дата", 
pdtp_name AS "Тип продукта", 
ilod_weightnet AS "Масса (нетто)"
FROM implementationload 
LEFT JOIN implementation ON impl_id = ilod_implementation 
LEFT JOIN producttype ON pdtp_id = impl_producttype 
WHERE NOT ilod_isdeleted 
AND NOT impl_isdeleted 
AND ilod_stock = :StockID