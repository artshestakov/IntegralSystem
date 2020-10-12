SELECT 
mwag_dateshipping AS "Дата отгрузки", 
mwag_datearrival AS "Дата прибытия", 
stck_name AS "Склад", 
mwag_price * (get_sum_weight_move_wagon_detail(mwag_id) / 1000) AS "Сумма", 
mwag_note AS "Примечание" 
FROM movewagon 
LEFT JOIN stock ON stck_id = mwag_stock 
WHERE NOT mwag_isdeleted 
AND mwag_provider = :CounterpartyID 
ORDER BY mwag_id