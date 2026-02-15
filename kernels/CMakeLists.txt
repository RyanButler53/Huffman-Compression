set(METAL_SOURCE comp.metal)
add_custom_target(compile_shaders ALL
                   COMMAND xcrun -sdk macosx metal -c ${METAL_SOURCE} -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders.air
                   COMMAND xcrun -sdk macosx metallib ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders.air -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/default.metallib
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)