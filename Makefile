###############################################################################
# Executable                                                                  #
###############################################################################

# executable name
EXE             := main

# build folder name
BIN             := build


###############################################################################
# Commands                                                                    #
###############################################################################

# C compiler
CC              := gcc

# C++ compiler
CXX             := g++

# assembler
AS              := gcc

# linker
LD              := gcc

# object dump
OBJDUMP         := objdump

# object copy
OBJCOPY         := objcopy

# remove
RM              := rm

# make directory
MKDIR           := mkdir


###############################################################################
# Commands' flags                                                             #
###############################################################################

# C flags
CFLAGS          := -O0 -Wall -ggdb -Iinclude -Ilib/array

# C++ flags
CXXFLAGS        := 

# assembler flags
ASFLAGS         := 

# linker flags
LDFLAGS         := -pthread

# object dump flags
OBJDUMPFLAGS    := --source --all-headers --demangle --line-numbers --wide $(BIN)/$(EXE) > $(basename $(BIN)/$(EXE)).lst

# object copy flags
OBJCOPYFLAGS    := -O binary -S $(BIN)/$(EXE) $(basename $(BIN)/$(EXE)).bin

# remove flags
RMFLAGS         := -rf

# make directory flags
MKDIRFLAGS      := -p


###############################################################################
# Sources and Includes                                                        #
###############################################################################

# C files to compile
CSRCS           := \
	src/event.c \
	src/task.c \
	src/thread.c \
	src/event_queue.c \
	src/simulation.c \
	src/main.c

# additional C files to compile
XCSRCS          := 

# C++ files to compile
CXXSRCS         := 

# additional C++ files to compile
XCXXSRCS        := 

# assembly files to compile
ASSRCS          := 

# additional assembly files to compile
XASSRCS         := 

# header files
INCS            := \
	include/simulation_time.h \
	include/event.h \
	include/task.h \
	include/thread.h \
	include/event_queue.h \
	include/simulation.h

# additional header files
XINCS           := \
	lib/array/array.h


###############################################################################
# Objects and libraries                                                       #
###############################################################################

# C object files
COBJS           := $(addprefix $(BIN)/, $(addsuffix .o, $(CSRCS) $(XCSRCS)))

# C++ object files
CXXOBJS         := $(addprefix $(BIN)/, $(addsuffix .o, $(CXXSRCS) $(XCXXSRCS)))

# assembly object files
ASOBJCS         := $(addprefix $(BIN)/, $(addsuffix .o, $(ASSRCS) $(XASSRCS)))

# object files
OBJS            := $(COBJS) $(CXXOBJS) $(ASOBJCS)

# additional object files
XOBJS           := 

# libraries
LDLIBS          := 

# additional libraries
XLDLIBS         := 


###############################################################################
# Command calls                                                               #
###############################################################################

# compile C source
CCOMPILE        = $(CC) $(CFLAGS) -c $< -o $@

# compile C++ source
CXXCOMPILE      = $(CXX) $(CXXFLAGS) -c $< -o $@

# compile assembly source
ASCOMPILE       = $(AS) $(ASFLAGS) -c $< -o $@

# link objects
LINKOBJS        = $(LD) $(LDFLAGS) $^ -o $@


###############################################################################
# Targets                                                                     #
###############################################################################

# build the executable
.PHONY: all
all: $(BIN)/$(EXE)

# executable target
$(BIN)/$(EXE): $(OBJS) $(XOBJS) $(LDLIBS) $(XLDLIBS)
	$(LINKOBJS)

# C targets
$(COBJS): $(BIN)/%.o: % $(INCS) $(XINCS)
	@ $(MKDIR) $(MKDIRFLAGS) $(dir $@)
	$(CCOMPILE)

# C++ targets
$(CXXOBJS): $(BIN)/%.o: % $(INCS) $(XINCS)
	@ $(MKDIR) $(MKDIRFLAGS) $(dir $@)
	$(CXXCOMPILE)

# assembly targets
$(ASOBJCS): $(BIN)/%.o: % $(INCS) $(XINCS)
	@ $(MKDIR) $(MKDIRFLAGS) $(dir $@)
	$(ASCOMPILE)

# force rebuild
.PHONY: again
again: clean $(BIN)/$(EXE)

# run executable on host
.PHONY: run
run: $(BIN)/$(EXE)
	./$(BIN)/$(EXE)

# create listing
.PHONY: lst
lst:
	$(OBJDUMP) $(OBJDUMPFLAGS)

# create binary from executable
.PHONY: bin
bin:
	$(OBJCOPY) $(OBJCOPYFLAGS)

# remove previous build
.PHONY: clean
clean:
	$(RM) $(RMFLAGS) $(BIN)