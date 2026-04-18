#!/b in/bash

DIR=$(dirname "$0")
for f in $DIR/*.sed; do
	sed -i -f "$f" "$1"
done
