#!/bin/bash

function env_set() {                                                                                                
    sed -i -e "/^export $2=.*/d" ~/.bashrc
    echo "export $2=`readlink -e $1`" >> ~/.bashrc
}

#bash ./init.sh nemu
#bash ./init.sh abstract-machine
#bash ./init.sh fceux-am
#bash ./init.sh am-kernels
#bash ./init.sh nanos-lite
#bash ./init.sh navy-apps

echo "set ENV ..."
env_set nemu NEMU_HOME
env_set abstract-machine AM_HOME
env_set navy-apps NAVY_HOME
echo "export alias pbcopy='xclip -selection clipboard'" >> ~/.bashrc
echo "export alias pbpaste='xclip -selection clipboard -o'" >> ~/.bashrc
source ~/.bashrc
echo "done!"

cd nemu
make menuconfig
cd ..

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
