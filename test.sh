#!/bin/bash

bash ./init.sh nemu
bash ./init.sh abstract-machine
bash ./init.sh fceux-am
bash ./init.sh am-kernels
bash ./init.sh nanos-lite
bash ./init.sh navy-apps
make menuconfig

cd navy-apps/apps 
cd nterm
make ISA=riscv32 install
cd ..
cd bird
make ISA=riscv32 install
cd ..
cd menu
make ISA=riscv32 install
cd ..
cd pal
make ISA=riscv32 install
cd ..
cd nslider
make ISA=riscv32 install
cd ..
cd busybox
make ISA=riscv32 install
cd ..
cd ../..

cd nanos-lite
make ARCH=riscv32-nemu update
make ARCH=riscv32-nemu run
