SELECT 
impl_date AS "Дата реализации", 
pdtp_name AS "Тип продукта", 
iunl_weightnet AS "Масса (нетто)" 
FROM implementationunload 
LEFT JOIN implementation ON impl_id = iunl_implementation 
LEFT JOIN producttype ON pdtp_id = impl_producttype 
WHERE NOT iunl_isdeleted 
AND NOT (SELECT impl_isdeleted FROM implementation WHERE impl_id = iunl_implementation) 
AND iunl_stock = :StockID