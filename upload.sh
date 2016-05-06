#!/bin/bash

ip_base=${ip_base-"192.168.1"}
retries=${retries-5}
partial_cmd="pio run -t upload --upload-port="
if [ $# -eq 0 ]; then
  echo "usage: upload.sh args, where args are a list of last octets of IPs to update."
  echo "example: 'upload.sh 119 124' updates devices at 192.168.1.119 and 192.168.1.124."
  echo "example: 'ip_base=10.10.1 upload.sh 119' updates device at 10.10.1.119."
  echo "example: 'retries=2 ip_base=10.10.1 upload.sh 119' updates with 2 retries."
fi
for ip in $*; do 
  counter=1
  cmd="${partial_cmd}${ip_base}.${ip}"
  echo "*** ${ip}: Attempt 1 of ${retries}, '${cmd}'"
  ${cmd}
  while [[ $? -ne 0 && counter -lt retries ]]; do
    counter=$[ counter + 1 ] 
    echo "*** ${ip}: Attempt ${counter} of ${retries}, '${cmd}'"
    ${cmd}
  done
done