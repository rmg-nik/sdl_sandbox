project(sandbox_sample)

if(ANDROID)
	add_library(${PROJECT_NAME} SHARED)
else()
	add_executable(${PROJECT_NAME})
endif()

target_sources(${PROJECT_NAME} PRIVATE main.cpp)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		SDL2::SDL2-static
		SDL2::SDL2main
		SDL2_image::SDL2_image-static
		SDL2::mixer
		SDL2_ttf::SDL2_ttf-static
)

if(UNIX OR MSVC)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/assets ${PROJECT_BINARY_DIR}/assets
		COMMENT "Copying assets to output directory"
	)
endif()
