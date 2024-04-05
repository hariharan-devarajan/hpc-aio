#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
service ssh restart
cp ${SCRIPT_DIR}/hosts /etc/hosts
ips=`cat ${SCRIPT_DIR}/hosts|awk -F" " '{print $1}'`
for ssh_ip in $ips;
do
ssh-keyscan -H $ssh_ip >> ~/.ssh/known_hosts
done