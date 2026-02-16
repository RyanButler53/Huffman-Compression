find_library(libMetal Metal FRAMEWORK)
add_library(metal::metal INTERFACE IMPORTED GLOBAL)
set_target_properties(metal::metal PROPERTIES 
            IMPORTED_LOCATION ${libMetal}
            INTERFACE_LINK_LIBRARIES "-framework Metal")

find_library(libQuartzCore QuartzCore FRAMEWORK)
add_library(metal::quartzcore INTERFACE IMPORTED GLOBAL)
set_target_properties(metal::quartzcore PROPERTIES 
            IMPORTED_LOCATION ${libQuartzCore}
            INTERFACE_LINK_LIBRARIES "-framework QuartzCore")

find_library(libFoundation Foundation FRAMEWORK)
add_library(metal::foundation INTERFACE IMPORTED GLOBAL)
set_target_properties(metal::foundation PROPERTIES
            IMPORTED_LOCATION ${libFoundation}
            INTERFACE_LINK_LIBRARIES "-framework Foundation")

add_library(metal-cpp INTERFACE)
target_link_libraries(metal-cpp PUBLIC INTERFACE metal::metal metal::quartzcore metal::foundation)
target_include_directories(metal-cpp INTERFACE ${metal_ROOT})

message(STATUS "Found metal-cpp ${metal_ROOT}")
