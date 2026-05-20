#!/bin/bash

# Initialize
echo '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"test","version":"1.0"}}}'

# Send initialized notification
echo '{"jsonrpc":"2.0","method":"notifications/initialized"}'

# List tools
echo '{"jsonrpc":"2.0","id":2,"method":"tools/list","params":{}}'

# call tool
echo '{"jsonrpc":"2.0","method":"tools/call","params":{"name":"cut","arguments":{"path":"/home/alex/src/butcher/samples/libffi-6.dll","addr":"0x044FC7AC"}},"id":3}'

# Keep stdin open
cat
