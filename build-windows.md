# xmax
xmax blockchain system

# Table of contents

# Xmax for Windows
1. [Prepare](#xmaxprepare)
2. [Build MongoDB](#buildmongodb)
3. [Build Boost](#buildboost)
4. [Build Wasm-Compiler](#buildwasmcompiler)
5. [Build Binaryen](#buildbinaryen)
6. [Install OpenSSL](#installopenssl)
7. [Build Xmax](#buildxmax)

<a name="xmaxprepare"></a>
# Prepare

Develop IDE: Visual Studio 2015

Tools: CMake, Git

Depends: MongoDB, boost, wasm-compiler, binaryen, OpenSSL-Win64, secp256k1-zkp

CMake download: https://cmake.org/download/

Git download: https://git-scm.com/downloads

<a name="buildmongodb"></a>
## Build MongoDB on Windows

### Build mongo-c-driver:

#### Build libbson in mongo-c-driver
Copy `mongo-c-driver-1.9.4` from xmax/libraries to anywhere you want to compile mongo-c-driver. Then execute:

```bash
cd mongo-c-driver-1.9.4/src/libbson
cmake -G "Visual Studio 14 2015 Win64" "-DCMAKE_INSTALL_PREFIX=C:\mongo-c-driver" "-DCMAKE_BUILD_TYPE=Release"
```
`C:\mongo-c-driver` is default build location and you can change to whatever you want.

Open `Developer Command Prompt for 2015` and goto `mongo-c-driver-1.9.4/src/libbson` then execute:

```bash
msbuild.exe /p:Configuration=Release ALL_BUILD.vcxproj
msbuild.exe /p:Configuration=Release INSTALL.vcxproj
```

#### Build mongo-c-driver itselft:

```bash
cd mongo-c-driver-1.9.4
cmake -G "Visual Studio 14 2015 Win64" "-DENABLE_SSL=WINDOWS" "-DENABLE_SASL=SSPI" "-DCMAKE_INSTALL_PREFIX=C:\mongo-c-driver" "-DCMAKE_PREFIX_PATH=C:\mongo-c-driver" "-DCMAKE_BUILD_TYPE=Release"
```
Make sure the build directory is same as previous `libbson` build directory.

Open `Developer Command Prompt for 2015` and goto `mongo-c-driver-1.9.4` then execute:

```bash
msbuild.exe /p:Configuration=Release ALL_BUILD.vcxproj
msbuild.exe /p:Configuration=Release INSTALL.vcxproj
```

### Build mongo-cxx-driver:

Find a location for compiling the source then execute:
```bash
git clone https://github.com/mongodb/mongo-cxx-driver.git --branch releases/stable --depth 1
cd mongo-cxx-driver/build
cmake -G "Visual Studio 14 Win64"  -DCMAKE_INSTALL_PREFIX=C:\mongo-cxx-driver -DCMAKE_PREFIX_PATH=c:\mongo-c-driver -DBOOST_ROOT=d:\boost_1_66_0 ..
```
Open `Developer Command Prompt for 2015` and goto `mongo-cxx-driver/build` then execute:

```bash
msbuild.exe ALL_BUILD.vcxproj
msbuild.exe INSTALL.vcxproj
```

### Set CMake variables for MongoDB
In order to enable mongodb in XMax, you may set following CMake variables. For example:
```bash
-DMONGO_DB_C_ROOT=c:/mongo-c-driver
-DMONGO_DB_CXX_ROOT=c:/mongo-cxx-driver
```
If you use the same paths as above, you can ignore this step. These are default paths. 
But if you want to use other directories you `MUST` set these variables yourself.


### Run MongoDB:

Download and install MongoDB server for Windows which could be found at it's offcial website: `https://www.mongodb.com/download-center`.

Then find a custom data location and start the service, for example:
```bash
"D:\Program Files\MongoDB\Server\3.6\bin\mongod.exe" --dbpath d:\mongo_db\data
```

Add plugin config to `config.ini`
```bash
plugin = Xmaxplatform::mongodb_plugin
mongodb-uri = mongodb://localhost:27017
```

After success compilation,  make sure to build `INSTALL` project in solution.  It will install dependent mongodb dynamic libraries.

Then you can run `xmaxrun` with MongoDB in install directory.

<a name="buildboost"></a>
## Build boost on Windows

Boost version: 1.66.0
Binaries of boost for msvc-14(x64), or build from source.

Binaries download link: https://dl.bintray.com/boostorg/release/1.66.0/binaries/

Source download link: https://dl.bintray.com/boostorg/release/1.66.0/source/

### build boost from source

Open cmd or Windows PowerShell and cd to boost root path, call build command:
```bash
.\bootstrap.bat --prefix="c:\Boost\boost_1_66_0"
.\b2 --build-type=complete --toolset=msvc-14.0 --layout=versioned --prefix="c:\Boost\boost_1_66_0" --without-mpi install 
```
<a name="buildwasmcompiler"></a>
## Build wasm-compiler on Windows

Build wasm-compiler from source.

Open cmd or Windows PowerShell and cd to any path you want.

```bash
mkdir wasm-compiler
cd wasm-compiler
git clone --depth 1 --single-branch --branch release_40 https://github.com/llvm-mirror/llvm.git
cd llvm/tools
git clone --depth 1 --single-branch --branch release_40 https://github.com/llvm-mirror/clang.git
cd ..
mkdir build
cd build
cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_INSTALL_PREFIX="c:/wasm" -DLLVM_TARGETS_TO_BUILD= -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=WebAssembly ../
```

If cmake succeed, you will found the file `LLVM.sln` in `build` folder. Double click `LLVM.sln`, open the LLVM solution by Visual Studio 2015.

If you want to build the release version of Xmax, you must to change the `Solution Configurations` to `Release`.

If you want to build the debug version of Xmax, you must to change the `Solution Configurations` to `Debug`.

Build the solution.

Right click `INSTALL` project in `Solution Explorer`, click `Build`.

<a name="buildbinaryen"></a>
## Build binaryen on Windows

Open cmd or Windows PowerShell and cd to `tools\binaryen` in Xmax code root.

```bash
cmake -G "Visual Studio 14 2015 Win64" .
```
If cmake succeed, you will found the file `binaryen.sln` in `binaryen` folder. Double click `binaryen.sln`, open the binaryen solution by Visual Studio 2015.

Build the solution.

<a name="installopenssl"></a>
## Install OpenSSL-Win64 on Windows

Download Page: https://slproweb.com/products/Win32OpenSSL.html

Choose Version: Win64 OpenSSL v1.1.0h

Install it to `C:\OpenSSL-Win64`.

<a name="buildxmax"></a>
## Build Xmax on Windows

Open cmd or Windows PowerShell and cd to the root path of Xmax code.


```bash
mkdir build
cd build
cmake -G "Visual Studio 14 2015 Win64" -DLLVM_DIR="c:/wasm/lib/cmake/llvm" -DWASM_LLVM_CONFIG="c:/wasm/bin/llvm-config.exe" -DBOOST_ROOT="c:/Boost/boost_1_66_0" -DOPENSSL_ROOT_DIR="C:\OpenSSL-Win64" -DXMAX_LIBBSONCXX="C:/mongo-cxx-driver/lib/bsoncxx.lib" -DXMAX_LIBMONGOCXX="C:/mongo-cxx-driver/lib/mongocxx.lib" ../
```

If cmake succeed, you will found the file `Xmax.sln` in `build` folder. Double click `Xmax.sln`, open the Xmax solution by Visual Studio 2015.

Build the solution: 

Right click `INSTALL` project in `Solution Explorer`, click `Build`.

Set as StartUp Project: 

Right click `xmaxrun` project in `Solution Explorer`, click `Set as StartUp Project`.

### Debug setting for xmaxrun on Windows

Right click `xmaxrun` project, click `Properties`.

Select `Configuration Properties` --> `Debugging`.

Set the `Working Directory` to `$(SolutionDir)install\bin`.

You can debug it with VS2015 now.

### Run xmaxrun on Windows

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

### Tips for debug

The config of `MongoDB` is not neccessary, if you just want to test or debug.

In first run of xmax block chain, it take a lot of time to init block chain db, please wait patiently.

If you just want to test or debug something, you can set the smaller size of block chain db by config property `shared-file-size`.

Only insert a init command `shared-file-size={size of mb}` into `config.ini`,such as:

`shared-file-size=1024`

`1024` means set the shared file size of block chain db to 1024 mega byte(1G). It would takes less time to init block chain db, when first run of xmax block chain.

