
NPROC=4

# install verilator
wget https://github.com/verilator/verilator/archive/refs/tags/v4.228.tar.gz -O verilator.tar.gz && \
tar zxf verilator.tar.gz && cd verilator-4.228 && \
autoconf && ./configure --prefix=${THE_PREFIX} && make -j${NPROC} && ${SUDO} make install
exit 0
