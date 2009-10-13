ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pyeastwood.so ${CMAKE_CURRENT_BINARY_DIR}/eastwood.py
    POST_BUILD
    COMMAND ${PYTHON_EXECUTABLE}
    ARGS -c "import sys,os; os.environ['LDFLAGS']='-L${CMAKE_CACHEFILE_DIR}/src'; sys.argv.append('build'); sys.argv.append('-t'), sys.argv.append('${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/pyeastwood.dir'); sys.argv.append('--build-lib'); sys.argv.append('${CMAKE_CURRENT_BINARY_DIR}/lib'); from setup import *"
    COMMAND touch ${LIBEASTWOOD_SOURCE_DIR}/python/CMakeLists.txt
    WORKING_DIRECTORY ${LIBEASTWOOD_SOURCE_DIR}/python
    DEPENDS eastwood
    VERBATIM
    )
ADD_CUSTOM_TARGET(pyeastwood ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pyeastwood.so)

INSTALL(CODE "
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c \"import sys,os; os.environ['LDFLAGS']='-L${CMAKE_CACHEFILE_DIR}/src'; sys.argv.append('build'); sys.argv.append('-t'), sys.argv.append('${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/pyeastwood.dir'); sys.argv.append('--build-lib'); sys.argv.append('${CMAKE_CURRENT_BINARY_DIR}/lib'); sys.argv.append('install'); sys.argv.append('--root'); sys.argv.append('$ENV{DESTDIR}'); os.environ['PWD'] = '${CMAKE_CACHEFILE_DIR}'; from setup import *\"
    WORKING_DIRECTORY ${LIBEASTWOOD_SOURCE_DIR}/python
    )"
    )

