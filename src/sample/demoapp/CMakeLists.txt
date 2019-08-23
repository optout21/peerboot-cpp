# sources of this exec
add_executable(peerboot_demoapp
	demoapp.cpp
	peers.hpp
	peers.cpp
)

# some compile definitions
target_compile_definitions(peerboot_demoapp
	PRIVATE
		-DPEBO_VERSION_MAJOR=${CPACK_PACKAGE_VERSION_MAJOR}
		-DPEBO_VERSION_MINOR=${CPACK_PACKAGE_VERSION_MINOR})

# some properties
set_target_properties(peerboot_demoapp
	PROPERTIES
		COMPILE_FLAGS
			"-DSOMEPROP=1")

# link with our library, and default platform libraries
target_link_libraries(peerboot_demoapp
	libpeerboot
	${PLATFORM_LIBS}
	${CMAKE_THREAD_LIBS_INIT}
)

if(NOT APPLE)
	install(TARGETS peerboot_demoapp
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib
	)
endif()
