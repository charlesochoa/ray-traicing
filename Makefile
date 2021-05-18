PROG=ogl_venus
SRCS= \
obj.cc \
png.cc \
main.cc

CXX:=g++
CXXFLAGS:=-O3 -march=native


# Linux
#LIBS:=-lglfw -lGLEW -lGLU -lGL
# Win
LIBS:=-lglfw3 -lglew32 -lglu32 -lopengl32 -lgdi32 -lpng -lz
OBJS:=$(SRCS:.cc=.o)
DEPS:=$(SRCS:.cc=.d)

all: $(PROG)

$(PROG): $(OBJS)
	@echo LD $<
	@$(CXX) -o $@ $^ $(LIBS)

%.o: %.cc
	@echo CXX $<
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	@if [ -n "$(OBJS)" ]; then rm -f $(OBJS); fi
	@if [ -n "$(DEPS)" ]; then rm -f $(DEPS); fi
	@if [ -f core ]; then rm -f core; fi
	@rm -f tags *.linkinfo

mrproper:
	@if [ -n "$(PROG)" ]; then rm -f $(PROG); fi
	@if [ -n "$(OBJS)" ]; then rm -f $(OBJS); fi
	@if [ -n "$(DEPS)" ]; then rm -f $(DEPS); fi
	@if [ -f core ]; then rm -f core; fi
	@rm -f tags *.linkinfo

-include $(DEPS)
