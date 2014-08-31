fold -1|uniq -c|sort -rk2|awk '!a[$2]++'|awk '{print$2"="$1}'
