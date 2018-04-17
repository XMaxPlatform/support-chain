if(WIN32)
	message(STATUS "-------------- Window config --------------")
	
	
	if(NOT DEFINED XMAX_ROOT_DIR)
		set(XMAX_ROOT_DIR ${CMAKE_SOURCE_DIR})
	endif()
	message(STATUS "XMAX_ROOT_DIR: ${XMAX_ROOT_DIR}")
	
	if("${LLVM_DIR}" STREQUAL "LLVM_DIR-NOTFOUND")
		if(NOT "$ENV{LLVM_DIR}" STREQUAL "")
			set(LLVM_DIR "$ENV{LLVM_DIR}")
		endif()
	endif()

	if(EXISTS ${LLVM_DIR})
	else()
		message(STATUS "Not defined LLVM_DIR")
		set(LLVM_DIR "${XMAX_ROOT_DIR}/wasm-compiler/llvm/build/lib/cmake/llvm")
	endif()
	message(STATUS "LLVM_DIR: ${LLVM_DIR}")




	if(NOT DEFINED OPENSSL_ROOT_DIR)
		set(OPENSSL_ROOT_DIR "${XMAX_ROOT_DIR}/libraries/OpenSSL-Win64")
	endif()
	message(STATUS "OPENSSL_ROOT_DIR: ${OPENSSL_ROOT_DIR}")
	
	set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
	set(OPENSSL_LIBRARY_DIR ${OPENSSL_ROOT_DIR}/lib)


	if(NOT DEFINED Secp256k1_ROOT_DIR)
		set(Secp256k1_ROOT_DIR "${XMAX_ROOT_DIR}/libraries/secp256k1-zkp")
	endif()	
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
