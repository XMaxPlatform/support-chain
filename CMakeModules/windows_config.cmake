if(WIN32)
	message(STATUS "-------------- Window config --------------")

	set(LLVM_DIR "${XMAX_ROOT_DIR}/wasm-compiler/llvm/lib/cmake/llvm")
	message(STATUS "LLVM_DIR: ${LLVM_DIR}")

	set(OPENSSL_ROOT_DIR "${XMAX_ROOT_DIR}/libraries/OpenSSL-Win64")
	set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
	message(STATUS "OPENSSL_ROOT_DIR: ${OPENSSL_ROOT_DIR}")

	set(Secp256k1_ROOT_DIR "${XMAX_ROOT_DIR}/libraries/OpenSSL-Win64/secp256k1-zkp")
	set(Secp256k1_LIBRARY "${Secp256k1_ROOT_DIR}/include")
	set(Secp256k1_INCLUDE_DIR "${Secp256k1_ROOT_DIR}/lib")
	message(STATUS "Secp256k1_ROOT_DIR: ${Secp256k1_ROOT_DIR}")

	message(STATUS "-------------- Window config end -----------")
	
endif(WIN32)
