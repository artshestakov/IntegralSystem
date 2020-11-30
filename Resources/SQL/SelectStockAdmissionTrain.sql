SELECT 
mwag_dateshipping AS "Дата отгрузки", 
cnpr_name AS "Поставщик", 
mwag_datearrival AS "Прибыло", 
-((get_sum_weight_move_wagon_detail(mwag_id) * mwag_technicallosses / 100) - get_sum_weight_move_wagon_detail(mwag_id)) AS "Масса", 
(SELECT COUNT(*) FROM movewagondetail WHERE NOT mwdt_isdeleted AND mwdt_movewagon = mwag_id) AS "Количество вагонов" 
FROM movewagon 
LEFT JOIN counterparty ON cnpr_id = mwag_provider 
LEFT JOIN movewagondetail ON mwdt_movewagon = mwag_id AND NOT mwdt_isdeleted 
WHERE NOT mwag_isdeleted 
AND mwag_datearrival IS NOT NULL 
AND mwag_stock = :StockID