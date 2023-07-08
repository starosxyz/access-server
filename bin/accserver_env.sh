#!/bin/bash
export ACCSERVER_ROOT=/opt/staros.xyz/accserver
export DIPC_ROOT=/opt/staros.xyz/dipc
export BXJSTACK_ROOT=/opt/staros.xyz/bxjstack
export LD_LIBRARY_PATH=$ACCSERVER_ROOT/libs:/opt/staros.xyz/dpdk/libs/:$ACCSERVER_ROOT/libs/voskern-fwd:$DIPC_ROOT/dlls:$BXJSTACK_ROOT/libs:$LD_LIBRARY_PATH
chmod a+x /opt/staros.xyz/dpdk/usertools/*
echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
echo 1024 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
mkdir /mnt/huge
mount -t hugetlbfs nodev /mnt/huge
echo 0 > /proc/sys/kernel/randomize_va_space
modprobe uio
