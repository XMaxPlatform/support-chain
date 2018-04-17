# xmax
xmax blockchain system

# Table of contents

1. [Build MongoDB](#buildmongodb)

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