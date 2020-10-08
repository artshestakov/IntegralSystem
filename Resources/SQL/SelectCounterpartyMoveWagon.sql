SELECT mwag_dateshipping AS "Дата отгрузки", mwag_datearrival AS "Дата прибытия", stck_name AS "Склад", mwag_price AS "Цена", mwag_note AS "Примечание" 
FROM movewagon 
LEFT JOIN stock ON stck_id = mwag_stock 
WHERE NOT mwag_isdeleted 
AND mwag_provider = :CounterpartyID 
ORDER BY mwag_id