RSA256
======

RSA256 is a demo project to use [Verilator](https://www.veripool.org/verilator/) to help validate the hardware design.
It implements the [RSA public key cryptosystem](https://en.wikipedia.org/wiki/RSA_(cryptosystem)) with key size of 256 bits.

# Directory Structure

The repository contains the following directories:

* cmodel: The software model of RSA256.
* docker: The Dockerfile and script to build the Docker container.
* include: The header file for both cmodel and systemc.
* systemc: The SystemC model of RSA256.
* verilog: The SystemVerilog implementation of RSA256.
* vtuber: The directory for the utilities of C++ verilog datatype, and adapter for Verilator.

You can refer to these directories for the relevant files and resources needed to build and run the application.

# Installation

## Prerequisites

Before you can use Docker to build and run the application, you need to have the following installed on your system:
Docker: [Installation instructions for Docker](https://docs.docker.com/get-docker/)

Or you can follow the steps in Dockerfile, and install the required packages.

## Build Instructions

To build the repository using CMake, follow these steps:

1. Open a terminal and navigate to the root directory of the repository.
2. Create a build directory (e.g., build/) for out-of-source builds:

```bash
mkdir build
cd build
```
3. Generate the build files using CMake:
```bash
cmake ..
```

4. Build the repository using the generated Makefile:
```
make
make test
```

# Contributing

We welcome contributions to improve the program. If you would like to contribute, please follow these steps:

1. Fork the repository.
2. Create a new branch: git checkout -b feature-branch
3. Make your changes and commit them: git commit -m "Description of changes"
4. Push your changes to your forked repository: git push origin feature-branch
5. Create a pull request on GitHub.

# License

This program is released under the [MIT license](LICENSE). Please review the license file for more information.

# Credits

1. [yodalee](https://github.com/yodalee)
2. [johnjohnlin](https://github.com/johnjohnlin)