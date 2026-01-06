
FetchContent_MakeAvailable(spdlog)

add_executable(
  BlockGame
  "config.cpp"
  "logger.cpp"

  "webgpu.cpp"
  "window.cpp"
  "app.cpp"

  "main.cpp"
)

target_include_directories(BlockGame PRIVATE "../include/")
target_link_libraries(BlockGame PRIVATE spdlog)

if(EMSCRIPTEN)
  set_target_properties(BlockGame PROPERTIES SUFFIX ".html")
  target_link_libraries(BlockGame PRIVATE emdawnwebgpu_cpp webgpu_glfw)
  target_link_options(BlockGame PRIVATE "-sASYNCIFY=1" "-sUSE_GLFW=3")
else()
  target_link_libraries(BlockGame PRIVATE webgpu_dawn webgpu_glfw glfw)
endif()
