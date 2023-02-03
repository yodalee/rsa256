
NPROC=4

# install verilator

wget https://github.com/verilator/verilator/archive/refs/tags/v5.006.tar.gz -O verilator.tar.gz && \
tar zxf verilator.tar.gz && cd verilator-5.006 && \
autoconf && ./configure --prefix=${THE_PREFIX} && make -j${NPROC} && ${SUDO} make install
exit 0
