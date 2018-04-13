if(WIN32)
	message(STATUS "-------------- Window config --------------")
	
	set(LLVM_DIR "${XMAX_ROOT_DIR}/wasm-compiler/llvm/build/lib/cmake/llvm")
	message(STATUS "LLVM_DIR: ${LLVM_DIR}")

	set(OPENSSL_ROOT_DIR "${XMAX_ROOT_DIR}/libraries/OpenSSL-Win64")
	set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
	set(OPENSSL_LIBRARY_DIR ${OPENSSL_ROOT_DIR}/lib)
	message(STATUS "OPENSSL_ROOT_DIR: ${OPENSSL_ROOT_DIR}")

	set(Secp256k1_ROOT_DIR "${XMAX_ROOT_DIR}/libraries/secp256k1-zkp")
	set(Secp256k1_LIBRARY "${Secp256k1_ROOT_DIR}/lib")
	set(Secp256k1_INCLUDE_DIR "${Secp256k1_ROOT_DIR}/include")
	message(STATUS "Secp256k1_ROOT_DIR: ${Secp256k1_ROOT_DIR}")

	set(MASM_FOUND false)
	ENABLE_LANGUAGE(ASM_MASM)
	message(STATUS "CMAKE_ASM_MASM_COMPILER: ${CMAKE_ASM_MASM_COMPILER}")
	if(CMAKE_ASM_MASM_COMPILER)
		set(MASM_FOUND true)
	endif(CMAKE_ASM_MASM_COMPILER)

	message(STATUS "-------------- Window config end -----------")
	
	
endif(WIN32)
