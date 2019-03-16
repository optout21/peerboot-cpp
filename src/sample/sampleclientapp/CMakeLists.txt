# sources of this exec
add_executable(peerboot_sampleclientapp
	app.cpp)

# some compile definitions
target_compile_definitions(peerboot_sampleclientapp
	PRIVATE
		-DPEBO_VERSION_MAJOR=${CPACK_PACKAGE_VERSION_MAJOR}
		-DPEBO_VERSION_MINOR=${CPACK_PACKAGE_VERSION_MINOR})

# some properties
set_target_properties(peerboot_sampleclientapp
	PROPERTIES
		COMPILE_FLAGS
			"-DSOMEPROP=1")

# link with our library, and default platform libraries
target_link_libraries(peerboot_sampleclientapp
	libpeerboot
	${PLATFORM_LIBS}
)

if(NOT APPLE)
	install(TARGETS peerboot_sampleclientapp
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib
	)
endif()
