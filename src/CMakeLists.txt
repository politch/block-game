

add_executable(
  BlockGame
  "main.cpp"
)

target_include_directories(BlockGame PRIVATE "../include/")
target_link_libraries(BlockGame PRIVATE webgpu_dawn webgpu_glfw glfw)

