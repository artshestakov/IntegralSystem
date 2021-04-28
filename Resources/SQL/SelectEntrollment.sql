SELECT 
cnpr_name AS "Контрагент", 
cpen_date AS "Дата начисления", 
cpen_sum AS "Сумма", 
cpen_note AS "Примечание" 
FROM counterpartyenrollment 
LEFT JOIN counterparty ON cnpr_id = cpen_counterparty 
WHERE cpen_sendtocoming 
ORDER BY cpen_id DESC