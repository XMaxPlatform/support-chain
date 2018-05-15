# xmax
xmax blockchain system

# Table of contents

# Xmax for Windows
1. [Prepare](#xmaxprepare)
2. [Install MongoDB](#buildmongodb)
3. [Build Boost](#buildboost)
4. [Build Wasm-Compiler](#buildwasmcompiler)
5. [Build Binaryen](#buildbinaryen)
6. [Build secp256k1-zkp](#secp256k1)
7. [Build Xmax](#buildxmax)

<a name="xmaxprepare"></a>
# Prepare

Tools: CMake, Git

Depends: MongoDB, boost, wasm-compiler, binaryen, OpenSSL, secp256k1-zkp

Install tools

```bash
brew update
brew install git automake libtool openssl llvm@4 gmp wget cmake gettext doxygen 
```


<a name="buildmongodb"></a>
## Install MongoDB

```bash
brew install openssl
cd /usr/local/include
ln -s ../opt/openssl/include/openssl .

brew install mongo-c-driver
brew install mongo-cxx-driver
brew install mongodb
```

<a name="buildboost"></a>
## Build boost
Build specified boost version.
```bash
cd ~
export BOOST_ROOT=${HOME}/boost_1_66_0
curl -L https://sourceforge.net/projects/boost/files/boost/1.66.0/boost_1_66_0.tar.bz2 > boost_1.66.0.tar.bz2
tar xvf boost_1.66.0.tar.bz2
cd boost_1_66_0/
./bootstrap.sh "--prefix=$BOOST_ROOT"
./b2 --build-type=complete --layout=versioned --prefix=${BOOST_ROOT} --without-mpi install

```

<a name="buildwasmcompiler"></a>
## Build wasm-compiler(llvm and wasm)

```bash
cd ~
mkdir wasm-compiler
cd wasm-compiler
git clone --depth 1 --single-branch --branch release_40 https://github.com/llvm-mirror/llvm.git
cd llvm/tools
git clone --depth 1 --single-branch --branch release_40 https://github.com/llvm-mirror/clang.git
cd ..
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/usr/local/wasm -DLLVM_TARGETS_TO_BUILD= -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=WebAssembly -DCMAKE_BUILD_TYPE=Release ../
sudo make -j4 install

WASM_LLVM_CONFIG=/usr/local/wasm/bin/llvm-config
```

<a name="buildbinaryen"></a>
## Build binaryen
```bash
cd ~
git clone https://github.com/WebAssembly/binaryen
cd binaryen
git checkout tags/1.37.14
cmake . && make
sudo mkdir /usr/local/binaryen
sudo mv ~/binaryen/bin /usr/local/binaryen
sudo ln -s /usr/local/binaryen/bin/* /usr/local
sudo rm -rf ${TEMP_DIR}/binaryen
BINARYEN_BIN=/usr/local/binaryen/bin/
```
<a name="secp256k1"></a>
## Build secp256k1-zkp

```bash
cd ~
git clone https://github.com/cryptonomex/secp256k1-zkp.git
cd secp256k1-zkp
./autogen.sh
./configure
make
sudo make install
rm -rf ~/secp256k1-zkp
```


<a name="buildxmax"></a>
## Build Xmax

Before build the project. we need config evn setting first

```bash
vim ~/.bashrc
export BOOST_ROOT=${HOME}/boost_1_66_0
export WASM_ROOT=/usr/local/wasm
export WASM_LLVM_CONFIG=/usr/local/wasm/bin/llvm-config
export LLVM_DIR=/usr/local/wasm/lib/cmake/llvm
export BINARYEN_BIN=/usr/local/binaryen/bin/
export DOXYGEN_EXECUTABLE=/usr/local/Cellar/doxygen/1.8.14/bin/doxygen
export OPENSSL_ROOT_DIR=/usr/local/opt/openssl
export OPENSSL_LIBRARIES=/usr/local/opt/openssl/lib
export XMX_LIBBSONCXX=/usr/local/lib/libbsoncxx.dylib
export XMX_LIBMONGOCXX=/usr/local/lib/libmongocxx.dylib
export CMAKE_BUILD_TYPE=Release
export CXX_COMPILER=clang++
export C_COMPILER=clang

```

Build the project.

```bash
cd ${XMXHOME}  #Enter xmx code path.
mkdir build
cd build
source ~/.bashrc
cmake -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE} " -DCMAKE_CXX_COMPILER="${CXX_COMPILER}" -DCMAKE_C_COMPILER="${C_COMPILER}" -DBOOST_ROOT=${BOOST_ROOT} -DWASM_ROOT="${WASM_ROOT}" -DOPENSSL_ROOT_DIR="${OPENSSL_ROOT_DIR}" -DXMAX_LIBBSONCXX=/usr/local/lib/libbsoncxx.dylib -DXMAX_LIBMONGOCXX=/usr/local/lib/libmongocxx.dylib ..

make -j4 

make install




cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=${CXX_COMPILER} -DCMAKE_C_COMPILER=${C_COMPILER} -DWASM_LLVM_CONFIG=${WASM_LLVM_CONFIG} -DBOOST_ROOT=${BOOST_ROOT} -DBINARYEN_BIN=${BINARYEN_BIN} -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR} -DOPENSSL_LIBRARIES=${OPENSSL_LIBRARIES} -DXMAX_LIBBSONCXX=/usr/local/lib/libbsoncxx.dylib -DXMAX_LIBMONGOCXX=/usr/local/lib/libmongocxx.dylib ..
```

If you want to build the debug version, you can add a arg to cmake command:

-DCMAKE_BUILD_TYPE=Debug.



### Run xmaxrun

`$(SolutionDir)install\bin` is the working directory for xmaxrun program.

Double click `xmaxrun.exe`(Found in working directory) run the Xmax block chain node.

If it is fist run, the program would be crashed.

You will find two new folders( `config-dir` and `data-dir` ) in working directory.

* `config-dir`：Store configuration files of block chain.
* `data-dir`：Store data files of block chain. (You can custom it by set the arg `--data-dir` when setup xmaxrun e.g. `--data-dir /path/to/data`）.

Copy `genesis.json`(Found in Xmax code root path) to working directory.
And then, open `config.ini` (Found in `config-dir`).

Set:
```bash
# Load the testnet genesis state, which creates some initial block producers with the default key
genesis-json = genesis.json     

plugin = Xmaxplatform::chaindata_plugin
plugin = Xmaxplatform::chainhttp_plugin
plugin = Xmaxplatform::blockchain_plugin
plugin = Xmaxplatform::blockbuilder_plugin
plugin = Xmaxplatform::contractutil_plugin

# setting for mongodb.
plugin = Xmaxplatform::mongodb_plugin
mongodb-uri = mongodb://localhost:27017

```
The config of mongodb is not neccessary, if you just want to test or debug.

When first run of xmax block chain, it take a lot of time to init block chain db, please wait patiently.

If you just want to test or debug something, you can set the smaller size of block chain db by config property `shared-file-size`.

Only insert a init command `shared-file-size={size of mb}` into `config.ini`,such as:

`shared-file-size=1024`

`1024` means set the shared file size of block chain db to 1024 mega byte(1G). It would takes less time to init block chain db, when first run of xmax block chain.
