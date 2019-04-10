find_package(Threads)

add_library(libpeerboot
	empty_net_handler.cpp
	empty_net_handler.hpp
	ipebo_net.hpp
	ipebo_peer.hpp
	ipebo_peer_cb.hpp
	inet_handler.hpp
	istore.hpp
	lib.cpp
	message.cpp
	message.hpp
	net_client.cpp
	net_client.hpp
	pebo_net.cpp
	pebo_net.hpp
	timestamp.cpp
	timestamp.cpp
	shell.cpp
	shell.hpp
	net_handler.cpp
	net_handler.hpp
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

target_link_libraries(libpeerboot
	${PLATFORM_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
)
