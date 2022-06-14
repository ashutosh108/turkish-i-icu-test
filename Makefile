CFLAGS  = -ggdb -std=c99 -pedantic -Wall -Wextra -Werror \
          `pkg-config --cflags icu-uc icu-io` #-DU_DISABLE_RENAMING
LDLIBS = `pkg-config --libs icu-uc icu-io`

all: main
