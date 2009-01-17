prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@LIB_INSTALL_DIR@
libdir=@LIB_INSTALL_DIR@
includedir=@CMAKE_INSTALL_PREFIX@/include

Name: libeastwood
Description: libeastwood game data library
Version: 0.1
URL: https://launchpad.net/doonlunacy
Cflags: -I${includedir}
Libs: -L${libdir} -lSDL -lsamplerate

