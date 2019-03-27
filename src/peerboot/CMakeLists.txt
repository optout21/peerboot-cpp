add_library(libpeerboot
	ipebo_net.hpp
	ipebo_peer.hpp
	ipebo_peer_cb.hpp
	istore.hpp
	lib.cpp
	pebo_net.cpp
	pebo_net.hpp
	shell.cpp
	shell.hpp
	store.cpp
	store.hpp
)

target_compile_definitions(libpeerboot
	PRIVATE
		-DPEBO_VERSION_MAJOR=${CPACK_PACKAGE_VERSION_MAJOR}
		-DPEBO_VERSION_MINOR=${CPACK_PACKAGE_VERSION_MINOR})

set_target_properties(libpeerboot
	PROPERTIES
		COMPILE_FLAGS
			"-DSOMEPROP=1")
