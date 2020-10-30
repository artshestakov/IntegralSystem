CURRENT_DAY=$(date +"%d")
CURRENT_MONTH=$(date +"%m")
CURRENT_YEAR=$(date +"%Y")
watch -n1 cat /opt/IntegralSystem/Logs/$CURRENT_YEAR/$CURRENT_MONTH/Carat_$CURRENT_DAY.$CURRENT_MONTH.$CURRENT_YEAR.log
