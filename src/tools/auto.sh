#!/bin/bash
#
# $1 = script name
# $2 = memory file name

while true
do
	out=$(./$1 2>&1)
	err=$(echo "$out" | grep -o "ERROR|[0-9]\+|[^|]*")
	if [[ ! -z "$err" ]]; then
		code=$(echo $err | awk -F '|' '{print $2}')
		info=$(echo $err | awk -F '|' '{print $3}')
		case $code in
			4)
				echo "Call error: $info"
				grep "$info" $2 1>/dev/null
				result=$?
				if [ "$result" -ne 0 ]; then
					echo "$info" >> $2
				else
					echo "repeated"
					exit
				fi
				;;
			*)
				exit
				;;
		esac
	fi
done
