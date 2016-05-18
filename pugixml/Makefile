TARGETNAME = pugixml
TARGETPATH = .
# 0 -- Application; 1 -- Static library; 2 -- Dynamic library
TARGETTYPE = 2
TARGETSRCS = $(wildcard *.c *.cpp)

CPP = g++
DEBUG = -g -DDEBUG -D_REENTRANT
RELEASE = -g -O2
CPPFLAGS = 
LINKFLAGS = -lpthread -lrt -lnsl -lgcc_s
OBJS = $(patsubst %.c,%.o,$(TARGETSRCS:.cpp=.o))

ifeq ($(strip $(TARGETPATH)),)
    TARGETPATH := .
endif

ifneq ($(strip $(MAKECMDGOALS)), release)
    CPPFLAGS := $(CPPFLAGS) $(DEBUG)
else
    CPPFLAGS := $(CPPFLAGS) $(RELEASE)
endif

ifneq ($(TARGETTYPE), 1)
    ifeq ($(TARGETTYPE), 2)
        TYPE = -shared
        TARGETNAME := $(TARGETNAME).so
    endif
    $(TARGETNAME) : $(OBJS)
	@if [ ! -d $(TARGETPATH) ]; then mkdir -p $(TARGETPATH); fi
	$(CPP) $(CPPFLAGS) -o $(TARGETPATH)/$@ $^ $(LINKFLAGS) $(TYPE)
else
    TYPE = -rcs
    TARGETNAME := $(TARGETNAME).a
    $(TARGETNAME) : $(OBJS)
	@if [ ! -d $(TARGETPATH) ]; then mkdir -p $(TARGETPATH); fi
	$(AR) cq $(TARGETPATH)/$@ $^
    $(OBJS) : $(TARGETSRCS)
	@if [ ! -d $(TARGETPATH) ]; then mkdir -p $(TARGETPATH); fi
	$(CPP) $(CPPFLAGS) -c $^
endif

.PHONY: all debug release clean

all debug release: $(TARGETNAME)

clean:
	rm -f *.o $(TARGETPATH)/$(TARGETNAME) $(TARGETPATH)/core*
