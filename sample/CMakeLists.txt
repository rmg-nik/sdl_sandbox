project(sandbox_sample)

if(ANDROID)
	add_library(${PROJECT_NAME} SHARED)
else()
	add_executable(${PROJECT_NAME})
endif()

target_sources(${PROJECT_NAME} PRIVATE main.cpp)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		SDL3::SDL3-static
		SDL3_image::SDL3_image-static
		SDL3_mixer::SDL3_mixer-static
		SDL3_ttf::SDL3_ttf-static
)

if(NOT ANDROID)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/assets ${PROJECT_BINARY_DIR}/assets
		COMMENT "Copying assets to output directory"
	)
endif()
