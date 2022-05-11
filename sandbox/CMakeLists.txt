project(sandbox)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE main.cpp)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		SDL2::SDL2-static
		SDL2::SDL2main
		SDL2_image::SDL2_image-static
		SDL2::mixer
		SDL2_ttf::SDL2_ttf-static
)