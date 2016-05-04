#!/bin/bash 
cmd='pio run -t upload --upload-port=192.168.1.'
if [ $# -eq 0 ]; then
  echo "usage: upload.sh args, where args are a list of last octets of IPs to update."
  echo "example: 'upload.sh 119 124' updates devices at 192.168.1.119 and 192.168.1.124."
fi
RETRIES=5
for ip in $*; do 
  COUNTER=1
  echo "*** $ip: Attempt 1 of $RETRIES, '$cmd$ip'"
  $cmd$ip
  while [[ $? -ne 0 && COUNTER -lt RETRIES ]]; do
    COUNTER=$[ COUNTER + 1 ] 
    echo "*** $ip: Attempt $COUNTER of $RETRIES, '$cmd$ip'"
    $cmd$ip
  done
done