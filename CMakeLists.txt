cmake_minimum_required(VERSION 3.20)
project(SGI C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# Flags de compilação
# ---------------------------------------------------------------------------
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /utf-8")
else()
    set(CMAKE_C_FLAGS         "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
endif()

# ---------------------------------------------------------------------------
# Recolha de ficheiros fonte
# ---------------------------------------------------------------------------
file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# ---------------------------------------------------------------------------
# Diagnóstico: listar ficheiros encontrados
# ---------------------------------------------------------------------------
message(STATUS "==========================================")
message(STATUS "Ficheiros fonte (.c):")
foreach(FILE ${SOURCE_FILES})
    get_filename_component(FILENAME ${FILE} NAME)
    message(STATUS "  ✓ ${FILENAME}")
endforeach()
message(STATUS "")
message(STATUS "Ficheiros header (.h):")
foreach(FILE ${HEADER_FILES})
    get_filename_component(FILENAME ${FILE} NAME)
    message(STATUS "  ✓ ${FILENAME}")
endforeach()
message(STATUS "==========================================")

# ---------------------------------------------------------------------------
# Validação: garantir que existem ficheiros fonte
# ---------------------------------------------------------------------------
list(LENGTH SOURCE_FILES NUM_SOURCES)
if(NUM_SOURCES EQUAL 0)
    message(FATAL_ERROR "Nenhum ficheiro .c encontrado em src/")
endif()

# ---------------------------------------------------------------------------
# Executável
# ---------------------------------------------------------------------------
add_executable(SGI ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(SGI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ---------------------------------------------------------------------------
# Criar directórios de runtime em tempo de build (data/ e logs/)
# ---------------------------------------------------------------------------
add_custom_command(
        TARGET SGI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/logs"
        COMMENT "A criar directórios de runtime (data/ e logs/)..."
)

message(STATUS "Configuração concluída!")
message(STATUS "Executável: SGI")