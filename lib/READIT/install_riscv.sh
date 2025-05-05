install_riscv_toolchain() {
    # Update system and install required dependencies
    sudo apt-get update
    sudo apt-get install -y autoconf automake autotools-dev curl python3 python3-pip python3-tomli \
        libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool \
        patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev libslirp-dev \
        device-tree-compiler libboost-all-dev

    # Clone the RISC-V GNU toolchain repository and build it
    git clone https://github.com/riscv/riscv-gnu-toolchain
    cd riscv-gnu-toolchain
    mkdir -p build/install
    cd build
    ../configure --prefix=$(pwd)/install --enable-multilib --with-arch=rv64gcv --with-abi=lp64d
    make -j$(nproc)
    echo 'export PATH=$HOME/riscv-gnu-toolchain/build/install/bin:$PATH' >> ~/.bashrc
    source ~/.bashrc
    cd ../../
}

install_spike() {
    # Clone the Spike (RISC-V ISA simulator) repository and build it
    git clone https://github.com/riscv/riscv-isa-sim.git
    cd riscv-isa-sim
    mkdir -p build/install
    cd build
    ../configure --prefix=$(pwd)/install
    make -j$(nproc)
    make install
    echo 'export PATH=$HOME/riscv-isa-sim/build/install/bin:$PATH' >> ~/.bashrc
    source ~/.bashrc
    cd ../../
}

install_pk() {
    # Clone the RISC-V Proxy Kernel (PK) repository and build it
    git clone https://github.com/riscv/riscv-pk.git
    cd riscv-pk
    mkdir -p build/install
    cd build
    ../configure --prefix=$(pwd)/install --host=riscv64-unknown-elf
    make -j$(nproc)
    make install
    echo 'export PATH=$HOME/riscv-pk/build/install/riscv64-unknown-elf/bin:$PATH' >> ~/.bashrc
    source ~/.bashrc
    cd ../../
}
add_aliases(){
    cat << 'EOF' >> ~/.bashrc

# RISC-V Aliases and Functions
alias spike_run='spike --isa=RV64IMACV $HOME/riscv-pk/build/install/riscv64-unknown-elf/bin/pk'
alias rvgpp='riscv64-unknown-elf-g++ -march=rv64gcv -mabi=lp64d'
alias rvgcc='riscv64-unknown-elf-gcc -march=rv64gcv -mabi=lp64d'

spike_log() {
    if [ "$#" -lt 2 ]; then
        echo "Usage: spike_log <elf_file> <log_file_name.txt>"
        return 1
    fi
    spike -l --log="$2" --isa=RV64IMACV $HOME/riscv-pk/build/install/riscv64-unknown-elf/bin/pk "$1"
}

spike_logcommits() {
    if [ "$#" -lt 2 ]; then
        echo "Usage: spike_logcommits <elf_file> <log_file.txt>"
        return 1
    fi
    spike -l --log-commits --isa=RV64IMACV $HOME/riscv-pk/build/install/riscv64-unknown-elf/bin/pk "$1" > "$2" 2>&1
}

spike_debug() {
    if [ "$#" -lt 2 ]; then
        echo "Usage: spike_debug <elf_file> <log_file.txt>"
        return 1
    fi
    spike -d --isa=RV64IMACV $HOME/riscv-pk/build/install/riscv64-unknown-elf/bin/pk "$1" > "$2" 2>&1
}

EOF

    source ~/.bashrc
}

# Main function to execute all installations
install_riscv_toolchain
install_spike
install_pk
add_aliases
source ~/.bashrc




