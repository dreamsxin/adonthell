AM_CXXFLAGS = -I$(top_builddir)
DEFAULT_INCLUDES = -I$(top_builddir)
EXTRA_DIST = CMakeLists.txt sdl/screen_sdl.h sdl/surface_sdl.h sdl/gfx_sdl.cc \
	sdl/screen_sdl.cc sdl/surface_sdl.cc sdl2/screen_sdl.h sdl2/surface_sdl.h \
	sdl2/gfx_sdl.cc sdl2/screen_sdl.cc sdl2/surface_sdl.cc


adonthellincludedir = $(pkgincludedir)-@VERSION@/adonthell

## Our header files
pkgincludegfxdir = $(adonthellincludedir)/gfx
pkgincludegfx_HEADERS = \
	drawable.h \
	drawing_area.h \
	gfx.h \
	png_wrapper.h \
	screen.h \
    sprite.h \
	surface.h \
    surface_ext.h \
    surface_cacher.h

## Main library
lib_LTLIBRARIES = libadonthell_gfx.la

## Rules to build libgfx
libadonthell_gfx_la_SOURCES = \
	drawable.cc \
	drawing_area.cc \
	gfx.cc \
	png_wrapper.cc \
	screen.cc \
    sprite.cc \
	surface.cc \
    surface_ext.cc \
    surface_cacher.cc


libadonthell_gfx_la_CXXFLAGS = $(PY_CFLAGS) $(AM_CXXFLAGS)
libadonthell_gfx_la_LIBADD = $(PY_LIBS) -lltdl \
    $(top_builddir)/src/base/libadonthell_base.la \
    $(top_builddir)/src/event/libadonthell_event.la \
    -lstdc++ -lpng



###### Following definitions are for the backends
pkglibgfxdir = $(pkglibdir)/gfx
pkglibgfx_LTLIBRARIES = _sdl.la


### SDL backend

## Our header files
noinst_HEADERS = \
	$(sdlbackenddir)/screen_sdl.h \
	$(sdlbackenddir)/surface_sdl.h

## Rules to build libgfx_sdl
_sdl_la_SOURCES = \
	$(sdlbackenddir)/gfx_sdl.cc \
	$(sdlbackenddir)/screen_sdl.cc \
	$(sdlbackenddir)/surface_sdl.cc

_sdl_la_CXXFLAGS = $(SDL_CFLAGS) $(AM_CXXFLAGS)
_sdl_la_LDFLAGS = -module -avoid-version
_sdl_la_LIBADD = $(SDL_LIBS) -ladonthell_gfx

## define dependencies in case of parallel build
_sdl_la_DEPENDENCIES = libadonthell_gfx.la

