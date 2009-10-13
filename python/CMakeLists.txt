ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pyeastwood.so ${CMAKE_CURRENT_BINARY_DIR}/eastwood.py
    POST_BUILD
    COMMAND ${PYTHON_EXECUTABLE}
    ARGS -c "import sys,os; os.environ['LDFLAGS']='-L${CMAKE_CURRENT_BINARY_DIR}'; sys.argv.append('build'); sys.argv.append('-t'), sys.argv.append('${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/pyeastwood.dir'); sys.argv.append('--build-lib'); sys.argv.append('${CMAKE_CURRENT_BINARY_DIR}'); from setup import *"
    WORKING_DIRECTORY ${LIBEASTWOOD_SOURCE_DIR}/python
    DEPENDS eastwood
    VERBATIM
    )
ADD_CUSTOM_TARGET(pyeastwood ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pyeastwood.so)

INSTALL(CODE "
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c \"import sys,os; os.environ['LDFLAGS']='-L${CMAKE_CURRENT_BINARY_DIR}'; sys.argv.append('build'); sys.argv.append('-t'), sys.argv.append('${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/pyeastwood.dir'); sys.argv.append('--build-lib'); sys.argv.append('${CMAKE_CURRENT_BINARY_DIR}/lib'); sys.argv.append('install'); sys.argv.append('--root'); sys.argv.append('${CMAKE_CACHEFILE_DIR}/$ENV{DESTDIR}'); sys.argv.append('--skip-build'); os.environ['PWD'] = '/home/peroyvind/Dokumenter/doonlunacy/libeastwood/build'; from setup import *\"
    WORKING_DIRECTORY ${LIBEASTWOOD_SOURCE_DIR}/python
    )"
    )

