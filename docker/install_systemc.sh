
NPROC=4

# install systemc
wget https://github.com/accellera-official/systemc/archive/refs/tags/2.3.4.tar.gz -O systemc.tar.gz && \
tar zxf systemc.tar.gz && cd systemc-2.3.4 && \
cmake -DCMAKE_INSTALL_PREFIX=${THE_PREFIX} -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 && \
	make -j${NPROC} install
exit 0
