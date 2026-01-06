
FetchContent_MakeAvailable(spdlog)
FetchContent_MakeAvailable(glm)

add_executable(
  BlockGame
  "config.cpp"
  "logger.cpp"

  "webgpu.cpp"
  "window.cpp"
  "app.cpp"

  "pipeline.cpp"
  "texture.cpp"

  "main.cpp"
)

target_include_directories(BlockGame PRIVATE "../include/")
target_link_libraries(BlockGame PRIVATE spdlog glm::glm stb_image)

if(EMSCRIPTEN)
  set_target_properties(BlockGame PROPERTIES SUFFIX ".html")
  target_link_libraries(BlockGame PRIVATE emdawnwebgpu_cpp webgpu_glfw)
  target_link_options(
    BlockGame PRIVATE
    "-sASYNCIFY=1"
    "-sUSE_GLFW=3"
    "--preload-file"
    "${CMAKE_CURRENT_SOURCE_DIR}/../assets@assets"
  )
else()
  target_link_libraries(BlockGame PRIVATE webgpu_dawn webgpu_glfw glfw)
  configure_file("../assets/shader.wgsl" "${CMAKE_CURRENT_BINARY_DIR}/assets/shader.wgsl" COPYONLY)
  configure_file("../assets/cobblestone.png" "${CMAKE_CURRENT_BINARY_DIR}/assets/cobblestone.png" COPYONLY)
endif()
