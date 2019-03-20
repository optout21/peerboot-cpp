add_library(libpeerboot
	lib.cpp
	shell.cpp
	shell.hpp)

target_compile_definitions(libpeerboot
	PRIVATE
		-DPEBO_VERSION_MAJOR=${CPACK_PACKAGE_VERSION_MAJOR}
		-DPEBO_VERSION_MINOR=${CPACK_PACKAGE_VERSION_MINOR})

set_target_properties(libpeerboot
	PROPERTIES
		COMPILE_FLAGS
			"-DSOMEPROP=1")
