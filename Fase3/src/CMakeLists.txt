cmake_minimum_required(VERSION 3.5)

# Project Name
project(engine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Adicione os arquivos de código fonte do engine
set(ENGINE_SOURCE_FILES 
    engine_src/engine.cpp 
    utils/point.cpp
    utils/list.cpp
    utils/primitive.cpp
    utils/matrix.cpp
)

# Adicione os arquivos de código fonte do generator
set(GENERATOR_SOURCE_FILES
    generator_src/generator.cpp 
    utils/point.cpp
    utils/list.cpp
    utils/primitive.cpp
    utils/matrix.cpp
    generator_src/bezier.cpp
    generator_src/plane.cpp
    generator_src/box.cpp
    generator_src/cone.cpp
    generator_src/sphere.cpp
    generator_src/ring.cpp
)

# Diretório para TinyXML
set(TINYXML_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/tinyXML")

# Adiciona TinyXML como biblioteca
add_library(tinyxml2 ${TINYXML_FOLDER}/tinyxml2.cpp)

# Configura o executável do engine
add_executable(engine ${ENGINE_SOURCE_FILES})

# Configura o executável do generator
add_executable(generator ${GENERATOR_SOURCE_FILES})

if(WIN32)
    # Configurações específicas para o Windows
    if(NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(FATAL_ERROR "GLUT não encontrado")
    endif()

    include_directories(${TOOLKITS_FOLDER}/glut)
    target_link_libraries(engine ${TOOLKITS_FOLDER}/glut/glut32.lib)
    target_link_libraries(generator ${TOOLKITS_FOLDER}/glut/glut32.lib)

    if(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll")
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
    endif()

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

else()

    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
    set(CMAKE_C_COMPILER "/usr/bin/clang")

    # Adiciona a flag -stdlib=libc++ para usar a biblioteca C++ do LLVM
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

    # Configurações para Linux e Mac
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)

    include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${TINYXML_FOLDER})
    target_link_libraries(engine ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} tinyxml2)
    target_link_libraries(generator ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} tinyxml2)
endif()
