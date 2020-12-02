SELECT 
impl_date AS "Дата", 
pdtp_name AS "Тип продукта", 
ilod_weightnet AS "Масса (нетто)"
FROM implementationload 
LEFT JOIN implementation ON impl_id = ilod_implementation 
LEFT JOIN producttype ON pdtp_id = impl_producttype 
WHERE ilod_stock = :StockID