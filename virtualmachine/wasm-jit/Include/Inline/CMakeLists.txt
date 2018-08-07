set(PublicHeaders
	BasicTypes.h
	DenseStaticIntSet.h
	Errors.h
	Floats.h
	Serialization.h
	Timing.h
	UTF8.h)
add_custom_target(Inline SOURCES ${PublicHeaders})

set_target_properties(Inline PROPERTIES FOLDER "3.Virtual Machine")