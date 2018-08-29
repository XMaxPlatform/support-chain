set(current_lib_name xmaxchain)
set(src_dir_name ${current_lib_name})
set(H_FILES)
set(S_FILES)

function(list_root ref_hpps ref_cpps hpps cpps)

	if( WIN32 )
		set(hscope "include")
		set(sscope ${src_dir_name})
	else()
		set(hscope "include")
		set(sscope ${src_dir_name})
	endif()


	file(GLOB hfiles "include/*.hpp")
	file(GLOB sfiles "*.cpp")

	source_group( 
		${hscope}
		FILES 
		${hfiles}
	)
	
	source_group( 
		${sscope}
		FILES 
		${sfiles}
	)
	set(newhpp "${hpps};${hfiles}")
	set(${ref_hpps} ${newhpp} PARENT_SCOPE)

	set(newcpp "${cpps};${sfiles}")
	set(${ref_cpps} ${newcpp} PARENT_SCOPE)

endfunction(list_root)


function(list_source ref_hpps ref_cpps hpps cpps code_scope)

	if( WIN32 )
		set(hscope "include\\${code_scope}")
		set(sscope "${src_dir_name}\\${code_scope}")
	else()
		set(hscope "include/${code_scope}")
		set(sscope "${src_dir_name}/${code_scope}")
	endif()


	file(GLOB hfiles "include/${code_scope}/*.hpp")
	file(GLOB sfiles "${code_scope}/*.cpp")

	source_group( 
		${hscope}
		FILES 
		${hfiles}
	)
	
	source_group( 
		${sscope}
		FILES 
		${sfiles}
	)
	set(newhpp "${hpps};${hfiles}")
	set(${ref_hpps} ${newhpp} PARENT_SCOPE)

	set(newcpp "${cpps};${sfiles}")
	set(${ref_cpps} ${newcpp} PARENT_SCOPE)

endfunction(list_source)


list_root(H_FILES S_FILES "${H_FILES}" "${S_FILES}")
list_source(H_FILES S_FILES "${H_FILES}" "${S_FILES}" objects)
list_source(H_FILES S_FILES "${H_FILES}" "${S_FILES}" cash)

if(USE_V8_VM)
list_source(H_FILES S_FILES "${H_FILES}" "${S_FILES}" jsvm_objbind)
list_source(H_FILES S_FILES "${H_FILES}" "${S_FILES}" jsvm_util)
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj") 
endif(MSVC)

if(USE_V8_VM)
add_library( ${current_lib_name} ${S_FILES} ${H_FILES} ${JSVM_BIND} ${JSVM_UTIL})
else()
add_library( ${current_lib_name} ${S_FILES} ${H_FILES}  )
endif()

target_link_libraries(
        ${current_lib_name} fc basetypes basechain xmax_native_contract
        Runtime IR WASM
        ${Boost_LIBRARIES} ${PLATFORM_LIBRARIES} )
target_include_directories(
        ${current_lib_name} PUBLIC "include"
        ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../virtualmachine/wasm-jit/Include
        ${CMAKE_SOURCE_DIR}/contracts_cpp/include
)

if(USE_V8_VM)
target_include_directories(${current_lib_name}  PUBLIC "include" ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/v8/include )
endif()

set_target_properties( ${current_lib_name} PROPERTIES PUBLIC_HEADER "${H_FILES}" FOLDER "4.BlockChain")
