INCLUDE_DIRECTORIES(${LIBEASTWOOD_SOURCE_DIR}/src)

SET( LIBEASTWOOD_SRCS
  AdlFile.cpp
  CpsFile.cpp
  Decode.cpp
  Dune2File.cpp
  Exception.cpp
  ExeFile.cpp
  Font.cpp
  IcnFile.cpp
  IniFile.cpp
  IStream.cpp
  MapFile.cpp
  OStream.cpp
  PakFile.cpp
  PalFile.cpp
  ShpFile.cpp
  Sound.cpp
  StringFile.cpp
  Surface.cpp
  VocFile.cpp
  WsaFile.cpp
  adl/adlibemu.c
  adl/emuopl.cpp
  adl/fmopl.c
  adl/realopl.cpp
  adl/temuopl.cpp
  emc/EmcFileBase.cpp
  emc/EmcFileAssemble.cpp
  emc/EmcFileDisassemble.cpp
  scaler/scale2xARM.s
  scaler/scale2x.cpp
  scaler/scale3x.cpp
  scaler/scalebit.cpp
)

SET( LIBEASTWOOD_HEADERS
  AdlFile.h
  CpsFile.h
  Decode.h
  Dune2File.h
  Exception.h
  ExeFile.h
  Font.h
  IcnFile.h
  IniFile.h
  IStream.h
  MapFile.h
  OStream.h
  Log.h
  PakFile.h
  PalFile.h
  ShpFile.h
  Sound.h
  StringFile.h
  VocFile.h
  WsaFile.h
  adl/adlibemu.h
  adl/emuopl.h
  adl/fmopl.h
  adl/kemuopl.h
  adl/opl.h
  adl/realopl.h
  adl/temuopl.h
  emc/EmcFileBase.h
  emc/EmcFileAssemble.h
  emc/EmcFileDisassemble.h  
  scaler/scalebit.h
)

SET( SDL_LIBEASTWOOD_SRCS
    SDL/MixerPlayer.cpp
    SDL/MixerSound.cpp
    SDL/Surface.cpp
)

SET( SDL_LIBEASTWOOD_HEADERS
    SDL/MixerPlayer.cpp
    SDL/MixerSound.cpp
    SDL/Surface.h
)

SET( EMC_INTERPRETER
    emc/dst.cpp
)

SET( PYEASTWOOD_SRCS
    python/pyeastwood.c
    python/pakfile.cpp
)

ADD_LIBRARY( eastwood SHARED ${LIBEASTWOOD_SRCS} )
TARGET_LINK_LIBRARIES( eastwood ${LIBSAMPLERATE_LIBRARIES} )
SET_TARGET_PROPERTIES( eastwood PROPERTIES VERSION "${LIBEASTWOOD_VERSION_INFO}" )
SET_TARGET_PROPERTIES( eastwood PROPERTIES SOVERSION "${LIBEASTWOOD_SOVERSION_INFO}" )
INSTALL(TARGETS eastwood LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
INSTALL( FILES ${LIBEASTWOOD_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eastwood" )

IF(LIBEASTWOOD_BUILD_SDL)
ADD_LIBRARY( SDL_eastwood SHARED ${SDL_LIBEASTWOOD_SRCS} )
TARGET_LINK_LIBRARIES( SDL_eastwood eastwood )
TARGET_LINK_LIBRARIES( eastwood ${SDLMIXER_LIBRARY} )
SET_TARGET_PROPERTIES( SDL_eastwood PROPERTIES VERSION "${LIBEASTWOOD_VERSION_INFO}" )
SET_TARGET_PROPERTIES( SDL_eastwood PROPERTIES SOVERSION "${LIBEASTWOOD_SOVERSION_INFO}" )
INSTALL(TARGETS SDL_eastwood LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
INSTALL( FILES ${SDL_LIBEASTWOOD_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eastwood" )
ENDIF(LIBEASTWOOD_BUILD_SDL)

IF(LIBEASTWOOD_BUILD_DST)
ADD_EXECUTABLE( dst ${EMC_INTERPRETER} )
TARGET_LINK_LIBRARIES( dst eastwood )
ENDIF(LIBEASTWOOD_BUILD_DST)

IF(LIBEASTWOOD_BUILD_PYTHON)
  GET_FILENAME_COMPONENT(PYTHON_LIB_INSTALL "${PYTHON_LIBRARY}" PATH)
  INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
  PYTHON_ADD_MODULE( pyeastwood ${PYEASTWOOD_SRCS})
  TARGET_LINK_LIBRARIES( pyeastwood eastwood )
  SET_TARGET_PROPERTIES(pyeastwood PROPERTIES PREFIX "")
ENDIF(LIBEASTWOOD_BUILD_PYTHON)

