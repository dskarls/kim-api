# - Find KIM
# Find the KIM headers and libraries.
#
#  KIM_INCLUDE_DIRS - where to find KIM headers, etc.
#  KIM_LIBRARIES    - List of libraries when using KIM.
#  KIM_API_FOUND    - True if KIM found.
#

find_package(PkgConfig)
find_program(XXD_EXECUTABLE "xxd")

if(TARGET kim-api)
    set(KIM_TARGET kim-api)
    set(KIM_CMAKE_DIR ${CMAKE_SOURCE_DIR}/cmake)
    set(KIM_COLLECTION_INFO_EXECUTABLE "") # invalid in build directory
    set(MODEL_INSTALL_PREFIX ${CMAKE_INSTALL_FULL_LIBDIR}/${FULL_PACKAGE_NAME}/models)
    set(MODEL_DRIVER_INSTALL_PREFIX ${CMAKE_INSTALL_FULL_LIBDIR}/${FULL_PACKAGE_NAME}/model_drivers)
else()
    pkg_check_modules(KIM libkim-api-v2 IMPORTED_TARGET QUIET)
    if(KIM_FOUND)
      set(KIM_TARGET PkgConfig::KIM)
      set(KIM_CMAKE_DIR ${KIM_LIBDIR}/kim-api-v2/cmake)
      find_program(KIM_COLLECTION_INFO_EXECUTABLE kim-api-v2-collections-info PATH_SUFFIXES libexec/kim-api-v2 PATHS ${KIM_PREFIX})
    else()
      find_path(KIM_INCLUDE_DIRS KIM_Version.h PATH_SUFFIXES kim-api-v2)
      find_library(KIM_LIBRARIES NAMES kim-api-v2)
      find_path(KIM_CMAKE_DIR parameterized-model_init_wrapper.cpp.in PATH_SUFFIXES lib/kim-api-v2/cmake lib64/kim-api-v2/cmake)
      find_program(KIM_COLLECTION_INFO_EXECUTABLE kim-api-v2-collections-info PATH_SUFFIXES libexec/kim-api-v2)
      add_library(KIM::KIM UNKNOWN IMPORTED)
      set_target_properties(KIM::KIM PROPERTIES IMPORTED_LOCATION ${KIM_LIBRARYS} INTERFACE_INCLUDE_DIRECTORIES ${KIM_INCLUDE_DIRS})
      set(KIM_TARGET KIM::KIM)
    endif()
    set(KIM_VERSION 2)
    include(FindPackageHandleStandardArgs)

    # handle the QUIETLY and REQUIRED arguments and set KIM_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(KIM REQUIRED_VARS KIM_INCLUDE_DIRS KIM_LIBRARIES KIM_CMAKE_DIR KIM_COLLECTION_INFO_EXECUTABLE VERSION_VAR KIM_VERSION)
    mark_as_advanced(KIM_LIBRARIES KIM_INCLUDE_DIRS)

    set(KIM_INSTALL_TYPE "SYSTEM" CACHE STRING "TODO add description here")
    set_property(CACHE KIM_INSTALL_TYPE PROPERTY STRINGS SYSTEM USER ENV)

    if(KIM_INSTALL_TYPE STREQUAL "SYSTEM")
        execute_process(COMMAND ${KIM_COLLECTION_INFO_EXECUTABLE} system models        OUTPUT_VARIABLE MODEL_INSTALL_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND ${KIM_COLLECTION_INFO_EXECUTABLE} system model_drivers OUTPUT_VARIABLE MODEL_DRIVER_INSTALL_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
    elseif(KIM_INSTALL_TYPE STREQUAL "USER")
        execute_process(COMMAND ${KIM_COLLECTION_INFO_EXECUTABLE} config_file models        OUTPUT_VARIABLE MODEL_INSTALL_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND ${KIM_COLLECTION_INFO_EXECUTABLE} config_file model_drivers OUTPUT_VARIABLE MODEL_DRIVER_INSTALL_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
    elseif(KIM_INSTALL_TYPE STREQUAL "ENV")
        execute_process(COMMAND ${KIM_COLLECTION_INFO_EXECUTABLE} env models        OUTPUT_VARIABLE MODEL_INSTALL_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND ${KIM_COLLECTION_INFO_EXECUTABLE} env model_drivers OUTPUT_VARIABLE MODEL_DRIVER_INSTALL_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
        # TODO error if empty
        # TODO split env var if necessary and select first element
    endif()
endif()

set(KIM_VERSION_FULL ${KIM_VERSION})



function(kim_add_model_target)
    set(options "")
    set(oneValueArgs NAME CREATE_FUNCTION_NAME CREATE_FUNCTION_LANG)
    set(multiValueArgs PARAMETER_FILES)
    cmake_parse_arguments(MODEL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # TODO sanity checks to ensure all arguments are passed

    set(MODEL_SOURCES "")

    if(MODEL_PARAMETER_FILES)
        list(LENGTH MODEL_PARAMETER_FILES NUMBER_OF_PARAMETER_FILES)
        set(INIT_WRAPPER_FILE "${KIM_CMAKE_DIR}/parameterized-model_init_wrapper.cpp.in")

        set(IDX 1)
        foreach(FNAME ${MODEL_PARAMETER_FILES})
          set(PARAM_FILE_IN "${CMAKE_CURRENT_SOURCE_DIR}/${FNAME}")
          set(PARAM_FILE_XXD_IN "${CMAKE_CURRENT_BINARY_DIR}/parameter_file_${IDX}")
          set(PARAM_FILE "${PARAM_FILE_XXD_IN}.c")
          list(APPEND MODEL_SOURCES ${PARAM_FILE})
          add_custom_command(OUTPUT ${PARAM_FILE}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PARAM_FILE_IN}" "${PARAM_FILE_XXD_IN}"
            COMMAND ${XXD_EXECUTABLE} -i "parameter_file_${IDX}" "${PARAM_FILE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            )
          MATH(EXPR IDX "${IDX}+1")
        endforeach()
    else()
        set(INIT_WRAPPER_FILE "${KIM_CMAKE_DIR}/stand-alone-model_init_wrapper.cpp.in")
    endif()

    configure_file(${INIT_WRAPPER_FILE} ${CMAKE_CURRENT_BINARY_DIR}/init_wrapper.cpp @ONLY)

    list(APPEND MODEL_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/init_wrapper.cpp)

    add_library(${MODEL_NAME} MODULE ${MODEL_SOURCES})
    set_target_properties(${MODEL_NAME} PROPERTIES OUTPUT_NAME "kim-api-model-v2"
                                                   LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/models/${MODEL_NAME})

    target_link_libraries(${MODEL_NAME} ${KIM_TARGET})

    install(TARGETS ${MODEL_NAME} LIBRARY DESTINATION "${MODEL_INSTALL_PREFIX}/${MODEL_NAME}")
endfunction(kim_add_model_target)

function(kim_add_model_driver_target)
    set(options "")
    set(oneValueArgs NAME CREATE_FUNCTION_NAME CREATE_FUNCTION_LANG)
    set(multiValueArgs "")
    cmake_parse_arguments(MODEL_DRIVER "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    configure_file(${KIM_CMAKE_DIR}/driver_init_wrapper.cpp.in
                   ${CMAKE_CURRENT_BINARY_DIR}/driver_init_wrapper.cpp @ONLY)

    set(MODEL_DRIVER_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/driver_init_wrapper.cpp)

    add_library(${MODEL_DRIVER_NAME} MODULE ${MODEL_DRIVER_SOURCES})
    set_target_properties(${MODEL_DRIVER_NAME} PROPERTIES OUTPUT_NAME "kim-api-model-driver-v2"
                                                          LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/model_drivers/${MODEL_DRIVER_NAME})

    target_link_libraries(${MODEL_DRIVER_NAME} ${KIM_TARGET})

    install(TARGETS ${MODEL_DRIVER_NAME} LIBRARY DESTINATION "${MODEL_DRIVER_INSTALL_PREFIX}/${MODEL_DRIVER_NAME}")
endfunction(kim_add_model_driver_target)
