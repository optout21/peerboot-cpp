find_package(Threads)

# sources of this exec
add_executable(testapp2
	app2.cpp
)

# some compile definitions
target_compile_definitions(testapp2
	PRIVATE
		-DPEBO_VERSION_MAJOR=${CPACK_PACKAGE_VERSION_MAJOR}
		-DPEBO_VERSION_MINOR=${CPACK_PACKAGE_VERSION_MINOR})

# some properties
set_target_properties(testapp2
	PROPERTIES
		COMPILE_FLAGS
			"-DSOMEPROP=1")

# link with our library, and default platform libraries
target_link_libraries(testapp2
    libpbtest
	libpeerboot
	${PLATFORM_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
)

if(NOT APPLE)
	install(TARGETS testapp2
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib
	)
endif()
