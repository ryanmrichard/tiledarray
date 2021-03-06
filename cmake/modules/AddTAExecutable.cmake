macro(add_ta_executable _name _source_files _libs)

  # build the entire contents of the executable as a library to support loading whole thing as a single shared library
  add_library(${_name}-lib EXCLUDE_FROM_ALL ${_source_files})
  foreach(_lib ${_libs})
    set(libtarget ${_lib})
    if ((${_lib} MATCHES "^tiledarray" OR ${_lib} MATCHES "^MAD") AND BUILD_SHARED_LIBS AND TARGET ${_lib}-static)
#   should not need to load whole archive unless ${_name}-lib lacks direct refs to requisite TA/MADNESS code from main
#      if (CMAKE_SYSTEM_NAME MATCHES "Linux")
#        set(libtarget -Wl,--whole-archive ${_lib}-static -Wl,--no-whole-archive )
#      elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
#        set(libtarget -Wl,-all_load ${_lib}-static)
#      endif()
      set(libtarget ${_lib}-static)
    endif((${_lib} MATCHES "^tiledarray" OR ${_lib} MATCHES "^MAD") AND BUILD_SHARED_LIBS AND TARGET ${_lib}-static)
    target_link_libraries(${_name}-lib PRIVATE ${libtarget})
  endforeach(_lib "${_libs}")
  add_dependencies(${_name}-lib External-tiledarray)

  # now make the executable itself
  file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_name}-stub.cpp CONTENT "")
  add_executable(${_name} EXCLUDE_FROM_ALL ${CMAKE_CURRENT_BINARY_DIR}/${_name}-stub.cpp)
  # MUST load whole archive since the stub lacks direct refs to any code
  if (NOT BUILD_SHARED_LIBS)
    if (CMAKE_SYSTEM_NAME MATCHES "Linux")
      target_link_libraries(${_name} -Wl,--whole-archive ${_name}-lib -Wl,--no-whole-archive )
    elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
      target_link_libraries(${_name} -Wl,-all_load ${_name}-lib)
    else ()
      target_link_libraries(${_name} ${_name}-lib)
    endif()
  else (NOT BUILD_SHARED_LIBS)
    target_link_libraries(${_name} ${_name}-lib)
  endif (NOT BUILD_SHARED_LIBS)

endmacro()
