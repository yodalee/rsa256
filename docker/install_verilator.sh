
NPROC=4

# install gflags
wget https://github.com/gflags/gflags/archive/refs/tags/v2.2.2.tar.gz -O gflags.tar.gz && \
tar zxf gflags.tar.gz && cd gflags-2.2.2 && \
cmake -DCMAKE_INSTALL_PREFIX=${THE_PREFIX} -DBUILD_SHARED_LIBS=ON \
	-DCMAKE_BUILD_TYPE=Release && make -j${NPROC} install
cd ..

# install glog
wget https://github.com/google/glog/archive/refs/tags/v0.6.0.tar.gz -O glog.tar.gz && \
tar zxf glog.tar.gz && cd glog-0.6.0  && \
cmake -DCMAKE_INSTALL_PREFIX=${THE_PREFIX} -DCMAKE_BUILD_TYPE=Release && make -j${NPROC} install
cd ..

# install protobuf
wget https://github.com/protocolbuffers/protobuf/archive/refs/tags/v21.9.tar.gz -O protobuf.tar.gz && \
tar zxf protobuf.tar.gz && cd protobuf-21.9 && cd cmake && \
cmake -DCMAKE_INSTALL_PREFIX=${THE_PREFIX} -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF && make -j${NPROC} install
cd ../..

# install verilator
wget https://github.com/verilator/verilator/archive/refs/tags/v4.228.tar.gz -O verilator.tar.gz && \
tar zxf verilator.tar.gz && cd verilator-4.228 && \
autoconf && ./configure --prefix=${THE_PREFIX} && make -j${NPROC} && make install
