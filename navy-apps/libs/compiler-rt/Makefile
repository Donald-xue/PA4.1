REPO_PATH = repo
NAVY_HOME = $(NEMU_HOME)/../navy-apps

ifeq ($(wildcard repo/LICENSE.TXT),)
  $(shell git clone --depth=1 git@github.com:NJU-ProjectN/compiler-rt.git $(REPO_PATH))
endif

NAME = compiler-rt
SRCS = $(shell find repo/ -name "*.c")
include $(NAVY_HOME)/Makefile
