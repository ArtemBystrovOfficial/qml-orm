cmake_minimum_required (VERSION 3.1.0)
project (pqxx_qml_simple_project)
#TODO -D

SET(DEBUG, TRUE)

if(DEBUG)
    set(CMAKE_PREFIX_PATH "C:\\QtCreator\\6.6.0\\msvc2019_64\\lib\\cmake") 
    set(QT_QMAKE_EXECUTABLE "C:\\QtCreator\\6.6.0\\msvc2019_64\\bin\\")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Qt Libraries
file(GLOB SOURCE_FILES "source/*.cpp")
file(GLOB HEADER_FILES "include/*.hpp")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Qml)
qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

qt_add_executable(main #WIN32 
    UI/main.qrc
    ${SOURCE_FILES}
    ${HEADER_FILES}
    main.cpp
)

set_target_properties(main
    PROPERTIES
    CMAKE_INCLUDE_CURRENT_DIR ON
    CMAKE_AUTOMOC ON
    CMAKE_AUTOUIC ON)

#Postgres CPP libraries
add_subdirectory(libpqxx)
target_include_directories (main PUBLIC libpqxx/include include)
target_link_libraries(main PRIVATE pqxx Qt6::Core Qt6::Widgets Qt6::Qml)
target_compile_features(main PRIVATE cxx_std_17)