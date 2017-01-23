# --------------------------------------------- #
# Makefile for N-subjettiness                   #
# Luke de Oliveira (lukedeoliveira@lbl.gov      #
# Created 1/21/17                               #
# --------------------------------------------- #

NAME := nsj

# set directories
BUILD        := build
SRC          := src
INC          := include
LIB          := $(CURDIR)/lib

MAKE         := make

LIBRARY      := $(LIB)/lib$(NAME).a

# set search path
vpath %.o    $(BUILD)
vpath %.cc   $(SRC) 
vpath %.h    $(INC) 
vpath %.hh   $(INC) 

# set compiler and flags
CXX          ?= g++
CXXFLAGS     := -Wall -fPIC -I$(INC) -g -std=c++11 

# get the stdlib in there
ifeq ($(CXX),clang++)
CXXFLAGS += -stdlib=libc++
endif

# HEP flags
FASTJETFLAGS   := $(shell fastjet-config --cxxflags --plugins)
FASTJETLDFLAGS  =
FASTJETLIBS     = $(shell fastjet-config --libs --plugins)

ROOTFLAGS      := $(shell root-config --cflags)
ROOTLDFLAGS    := $(shell root-config --ldflags)
ROOTLIBS       := $(shell root-config --glibs)

PYTHIAFLAGS    := $(shell pythia8-config --cxxflags)
PYTHIALDFLAGS  := $(shell pythia8-config --ldflags)
PYTHIALIBS      = $(shell pythia8-config --libs)


HEPLIBS += $(FASTJETLIBS)
HEPLIBS += $(ROOTLIBS)
HEPLIBS += $(PYTHIALIBS)

# Add HEP flags to common stuff
CXXFLAGS += $(ROOTFLAGS)
CXXFLAGS += $(FASTJETFLAGS)
CXXFLAGS += $(PYTHIAFLAGS)

LIBS     += $(HEPLIBS)
LDFLAGS   = $(ROOTLDFLAGS) $(PYTHIALDFLAGS) $(FASTJETLDFLAGS)

GEN_OBJ_SRC   := $(wildcard $(SRC)/*.cc)
GEN_OBJ       := $(notdir $(GEN_OBJ_SRC:%.cc=%.o))
GEN_OBJ_PATHS := $(GEN_OBJ:%=$(BUILD)/%)

# compilation of the code (default target)
all: $(LIBRARY)

$(LIBRARY): $(GEN_OBJ_PATHS)
	@echo "Building $@"
	@mkdir -p $(LIB)
	@ar cru $(LIBRARY) $(GEN_OBJ_PATHS)
	@ranlib $(LIBRARY)
# auto dependency generation for build

# compile rule
$(BUILD)/%.o: %.cc
	@echo compiling $<
	@mkdir -p $(BUILD)
	@$(CXX) -c $(CXXFLAGS) $< -o $@

.SECONDARY: $(GEN_OBJ_PATHS)

# use auto dependency generation
DEP = $(BUILD)

DEPTARGSTR = -MT $(BUILD)/$*.o -MT $(DEP)/$*.d
$(DEP)/%.d: %.cc
	@echo making dependencies for $<
	@mkdir -p $(DEP)
	@$(CXX) -MM -MP $(DEPTARGSTR) $(CXXFLAGS) $< -o $@ 

# clean
.PHONY : clean rmdep
CLEANLIST     = *~ *.o *.o~ *.d core 

clean:
	@echo "cleaning up the mess in $(CURDIR)..." 
	@rm -fr $(CLEANLIST) $(CLEANLIST:%=$(BUILD)/%) $(CLEANLIST:%=$(DEP)/%) $(BUILD) 

purge: clean rmdep
	@echo "removing all created files/folders under $(CURDIR)" 
	@rm -fr $(LIB) 

rmdep:
	@echo "removing dependencies from $(CURDIR)" 
	@rm -f $(DEP)/*.d