add_library(libpbtest
	connector_peer.cpp
	connector_peer.hpp
	simul_client.cpp
	simul_client.hpp
	test_pebo_net.cpp
    test_pebo_net.hpp
	testbench_shells.cpp
	testbench_shells.hpp
)

target_compile_definitions(libpbtest
	PRIVATE
		-DPEBO_VERSION_MAJOR=${CPACK_PACKAGE_VERSION_MAJOR}
		-DPEBO_VERSION_MINOR=${CPACK_PACKAGE_VERSION_MINOR})

set_target_properties(libpbtest
	PROPERTIES
		COMPILE_FLAGS
			"-DSOMEPROP=1")
