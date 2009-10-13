INCLUDE_DIRECTORIES(${LIBEASTWOOD_SOURCE_DIR}/include)


SET( LIBEASTWOOD_SRCS
  AdlFile.cpp
  CpsFile.cpp
  Decode.cpp
  Dune2File.cpp
  EmcFileAssemble.cpp
  EmcFileBase.cpp
  EmcFileDisassemble.cpp
  Exception.cpp
  ExeFile.cpp
  Font.cpp
  IcnFile.cpp
  IniFile.cpp
  IStream.cpp
  MapFile.cpp
  OStream.cpp
  PakFile.cpp
  Palette.cpp
  PalFile.cpp
  ShpFile.cpp
  Sound.cpp
  StringFile.cpp
  Surface.cpp
  VocFile.cpp
  WsaFile.cpp
  adl/adlibemu.c
  adl/analopl.cpp
  adl/diskopl.cpp
  adl/emuopl.cpp
  adl/fmopl.c
  adl/realopl.cpp
  adl/temuopl.cpp
  scaler/scale2xARM.s
  scaler/scale2x.cpp
  scaler/scale3x.cpp
  scaler/scalebit.cpp
)

SET( SDL_LIBEASTWOOD_SRCS
    SDL/Mixer/Player.cpp
    SDL/Mixer/Sound.cpp
    SDL/Palette.cpp
    SDL/Surface.cpp
)

ADD_LIBRARY( eastwood SHARED ${LIBEASTWOOD_SRCS} )
TARGET_LINK_LIBRARIES( eastwood ${LIBSAMPLERATE_LIBRARIES} )
SET_TARGET_PROPERTIES( eastwood PROPERTIES SOVERSION "${LIBEASTWOOD_SOVERSION_INFO}" )
INSTALL(TARGETS eastwood LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )


IF(LIBEASTWOOD_BUILD_SDL)
ADD_LIBRARY( SDL_eastwood SHARED ${SDL_LIBEASTWOOD_SRCS} )
TARGET_LINK_LIBRARIES( SDL_eastwood eastwood )
TARGET_LINK_LIBRARIES( eastwood ${SDL_LIBRARY} )
TARGET_LINK_LIBRARIES( eastwood ${SDLMIXER_LIBRARY} )
SET_TARGET_PROPERTIES( SDL_eastwood PROPERTIES SOVERSION "${LIBEASTWOOD_SOVERSION_INFO}" )
INSTALL(TARGETS SDL_eastwood LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
ENDIF(LIBEASTWOOD_BUILD_SDL)

IF(LIBEASTWOOD_BUILD_PYTHON)
  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pyeastwood.so ${CMAKE_CURRENT_BINARY_DIR}/eastwood.py
      POST_BUILD
      COMMAND ${PYTHON_EXECUTABLE}
      ARGS -c "import sys,os; os.environ['LDFLAGS']='-L${CMAKE_CURRENT_BINARY_DIR}'; sys.argv.append('build'); sys.argv.append('-t'), sys.argv.append('${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/pyeastwood.dir'); sys.argv.append('--build-lib'); sys.argv.append('${CMAKE_CURRENT_BINARY_DIR}'); from setup import *"
      WORKING_DIRECTORY ${LIBEASTWOOD_SOURCE_DIR}/src/python
      DEPENDS eastwood
      VERBATIM
      )
  ADD_CUSTOM_TARGET(pyeastwood ALL
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pyeastwood.so)

  INSTALL(CODE "
  execute_process(
      COMMAND ${PYTHON_EXECUTABLE} -c \"import sys,os; os.environ['LDFLAGS']='-L${CMAKE_CURRENT_BINARY_DIR}'; sys.argv.append('build'); sys.argv.append('-t'), sys.argv.append('${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/pyeastwood.dir'); sys.argv.append('--build-lib'); sys.argv.append('${CMAKE_CURRENT_BINARY_DIR}'); sys.argv.append('install'); sys.argv.append('--root'); sys.argv.append('${CMAKE_CACHEFILE_DIR}/$ENV{DESTDIR}'); os.environ['PWD'] = '/home/peroyvind/Dokumenter/doonlunacy/libeastwood/build'; from setup import *\"
      WORKING_DIRECTORY ${LIBEASTWOOD_SOURCE_DIR}/src/python
      )"
      )

ENDIF(LIBEASTWOOD_BUILD_PYTHON)

