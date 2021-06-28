bin_PROGRAMS = pong
pong_SOURCES = colours.h enemy.c enemy.h game.c game.h main.c sounds.c sounds.h text.c text.h title.c title.h ../res/pixmaps/title.xpm
pong_CFLAGS = $(SDL2_CFLAGS) -Wall -Wextra
pong_LDADD = $(SDL2_LIBS) -lm
pong_CPPFLAGS = -DPKGDATA='"$(pkgdatadir)"'
