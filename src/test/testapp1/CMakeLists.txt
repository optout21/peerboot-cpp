find_package(Threads)

# sources of this exec
add_executable(testapp1
	app1.cpp
)

# some compile definitions
target_compile_definitions(testapp1
	PRIVATE
		-DPEBO_VERSION_MAJOR=${CPACK_PACKAGE_VERSION_MAJOR}
		-DPEBO_VERSION_MINOR=${CPACK_PACKAGE_VERSION_MINOR})

# some properties
set_target_properties(testapp1
	PROPERTIES
		COMPILE_FLAGS
			"-DSOMEPROP=1")

# link with our library, and default platform libraries
target_link_libraries(testapp1
	libpeerboot
    libpbtest
	${PLATFORM_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
)

if(NOT APPLE)
	install(TARGETS testapp1
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib
	)
endif()
