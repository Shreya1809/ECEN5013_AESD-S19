#!/bin/bash
echo "---------System Information Script--------"
echo "--------------User Information------------"
w
echo
echo "---------Operating System Type------------"
hostnamectl
echo
echo "-------------OS Distribution--------------"
lsb_release -a
echo
echo "----------------OS Version----------------"
cat /etc/os-release
echo
echo "---------------Kernel Version-------------"
uname -r
echo
echo "---------Kernel GCC Version Build---------"
cat /proc/version
echo
echo "-------------Kernel Build Time------------"
uname -v
echo
echo "---------System Architecture Info---------"
lscpu
echo
echo "--------Info on File System Memory--------"
free
echo


