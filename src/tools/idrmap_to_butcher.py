import sys
import re

base = 0x00401000
names = []
file = open(sys.argv[1],'r')
for line in file:
	# ' 0001:000042D8       System.kernel32.CloseHandle'
	m = re.match(r" 0001:(\w+)[ ]+([^ ]+)$",line)
	if m != None:
		offset = m.group(1)
		name = m.group(2)
		name = name.replace("\n", "")
		name = name.replace(".", "_")
		name = name.replace("@", "_")
		name = name.replace("$", "_")
		name = name.replace("<", "_")
		name = name.replace(">", "_")
		name = name.replace("(", "_")
		name = name.replace(")", "_")
		name = name.replace("&", "_")
		name = name.replace(",", "_")
		if name in names:
			count = 1
			while True:
				if name+"_"+str(count) not in names:
					name = name + "_"+str(count)
					break
				count += 1
		names.append(name)
		print("0x"+hex(int(offset,16)+base)+","+name)
file.close()
