cmake_minimum_required(VERSION 2.8.11)

project(descriptors)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)

# There may be a way to tell up front if Qt5 is going to be found, but I haven't found
# a foolproof way to do it yet, so settle for the default error message for now.
#if(NOT CMAKE_PREFIX_PATH AND NOT Qt5Widgets_DIR)
#    message(WARNING "CMAKE_PREFIX_PATH is not defined, so find_package may not work. Set the CMAKE_PREFIX_PATH "
#            "environment variable to the install prefix of Qt 5, either on the command line as "
#            "-DCMAKE_PREFIX_PATH=\"path/to/Qt5/lib/cmake\" or with set(CMAKE_PREFIX_PATH path/to/Qt5/lib/cmake)")
#endif(NOT CMAKE_PREFIX_PATH AND NOT Qt5Widgets_DIR)

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)

# Generate code from ui files
qt5_wrap_ui(UI_HEADERS
    #frame.ui
)

# Generate rules for building source files from the resources
#qt5_add_resources(QRCS resources.qrc)

# Tell CMake to create the polygon executable
add_executable(descriptors
    main.cpp
    ../../lib/polygon.cpp
    ../../lib/parser.cpp
    ../../lib/converter.cpp
    ../../lib/simplification.cpp
${UI_HEADERS} ${QRCS})

# Add the Qt5 Widgets for linking
target_link_libraries(descriptors Qt5::Widgets)


#Required DGtal
FIND_PACKAGE(DGtal REQUIRED)
INCLUDE_DIRECTORIES(${DGTAL_INCLUDE_DIRS})
LINK_DIRECTORIES(${DGTAL_LIBRARY_DIRS})
TARGET_LINK_LIBRARIES(descriptors ${DGTAL_LIBRARIES})
