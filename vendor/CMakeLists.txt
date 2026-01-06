if(NOT EMSCRIPTEN)
  set(DAWN_FETCH_DEPENDENCIES ON)
  add_subdirectory(dawn EXCLUDE_FROM_ALL)
else()
  add_library(webgpu_glfw INTERFACE)
  target_link_libraries(webgpu_glfw INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/libdawn_glfw.a
  )
endif()

add_subdirectory(stb_image)
add_subdirectory(spdlog)
add_subdirectory(glm)

