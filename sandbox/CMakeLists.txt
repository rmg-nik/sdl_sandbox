project(sandbox)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE main.cpp)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		SDL3::SDL3-static
		SDL3_image::SDL3_image-static
		SDL3_mixer::SDL3_mixer-static
		SDL3_ttf::SDL3_ttf-static
)
