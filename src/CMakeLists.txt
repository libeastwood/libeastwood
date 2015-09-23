INCLUDE_DIRECTORIES(${LIBEASTWOOD_SOURCE_DIR}/include)


SET( LIBEASTWOOD_SRCS
  AdlFile.cpp
  #  ArchiveManager.cpp
  #BlowFish.cpp
  CpsFile.cpp
  Decode.cpp
  Dune2File.cpp
  EmcFileAssemble.cpp
  EmcFileBase.cpp
  EmcFileDisassemble.cpp
  Exception.cpp
  ExeFile.cpp
  FntFile.cpp
  IcnFile.cpp
  IffFile.cpp
  IniFile.cpp
  MapFile.cpp
  PakFile.cpp
  Palette.cpp
  PalFile.cpp
  PcxFile.cpp
  ShpFile.cpp
  Sound.cpp
  StringFile.cpp
  Surface.cpp
  VocFile.cpp
  WsaFile.cpp
  #WsKey.cpp
  XmiFile.cpp
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

#These files break if optimization is used on 64bit, at least with GCC
#set_source_files_properties(WsKey.cpp PROPERTIES COMPILE_FLAGS "-O0 -DNDEBUG")
#set_source_files_properties(BlowFish.cpp PROPERTIES COMPILE_FLAGS "-O0 -DNDEBUG")


ADD_LIBRARY( eastwood SHARED ${LIBEASTWOOD_SRCS} )

IF (BUILD_STATIC_LIBS)
  ADD_LIBRARY( eastwood-static STATIC ${LIBEASTWOOD_SRCS} )
  SET_TARGET_PROPERTIES(eastwood-static PROPERTIES OUTPUT_NAME eastwood CLEAN_DIRECT_OUTPUT 1)
ENDIF(BUILD_STATIC_LIBS)

TARGET_LINK_LIBRARIES( eastwood ${LIBSAMPLERATE_LIBRARIES} )
IF(WIN32)
  IF(MSVC)
    TARGET_LINK_LIBRARIES( eastwood "ws2_32.lib" )
  ELSE(MSVC)
    TARGET_LINK_LIBRARIES( eastwood "-lws2_32" )
  ENDIF(MSVC)
ENDIF(WIN32)
SET_TARGET_PROPERTIES( eastwood PROPERTIES SOVERSION "${LIBEASTWOOD_SOVERSION_INFO}" )
IF(NOT WIN32)
  IF( DEFINED CMAKE_INSTALL_LIBDIR )
    INSTALL(TARGETS eastwood ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
  ENDIF( DEFINED CMAKE_INSTALL_LIBDIR )
ENDIF(NOT WIN32)

# With the SDL wrapper classes being quite simple and trivial, we no longer
# ship them as part of a separate library, but write the whole class in
# the headers, leaving for whoever using them to build, this way we avoid
# additional dependencies for our library. We still build this library though
# just to verify that the code still builds, but we don't actually install it
# anymore...
IF(LIBEASTWOOD_BUILD_SDL)
  ADD_LIBRARY( SDL_eastwood SHARED ${SDL_LIBEASTWOOD_SRCS} )
  TARGET_LINK_LIBRARIES( SDL_eastwood eastwood )
  TARGET_LINK_LIBRARIES( eastwood ${SDL_LIBRARY} )
  TARGET_LINK_LIBRARIES( eastwood ${SDLMIXER_LIBRARY} )
  SET_TARGET_PROPERTIES( SDL_eastwood PROPERTIES SOVERSION "${LIBEASTWOOD_SOVERSION_INFO}" )
ENDIF(LIBEASTWOOD_BUILD_SDL)

