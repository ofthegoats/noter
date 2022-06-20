# ofthegoats C/C++ Makefile: 14/05/22

# PROJECT CONFIG
NAME=$(shell basename $(CURDIR))

CXX=clang++
CXXFLAGS=-std=c++17 -Wall -Wextra -IGLAD/include -lglfw
CC=clang
CCFLAGS=-std=c17 -IGLAD/include
LINKER=$(CXX)
LDFLAGS=-IGLAD/include -lglfw

SRCDIR=src
OBJDIR=obj
BINDIR=bin

OUTPUT=$(BINDIR)/$(NAME)
# END PROJECT CONFIG

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRC_CXX = $(call rwildcard,$(SRCDIR),*.cpp)
OBJ_CXX = $(SRC_CXX:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
SRC_C = $(call rwildcard,$(SRCDIR),*.c)
OBJ_C = $(SRC_C:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJ= $(OBJ_C) $(OBJ_CXX)

DEBUG ?= 1 # by default, debug
ifeq ($(DEBUG), 1)
	CFLAGS += -O0 -g
	CXXFLAGS += -O0 -g
else
	CFLAGS += -O3 -DNDEBUG
	CXXFLAGS += -O3 -DNDEBUG
endif

.PHONY: all clean run

all: $(OBJDIR) $(BINDIR) $(OUTPUT) 

clean:
	rm -rf $(BINDIR) $(OBJDIR)

run:
	./$(OUTPUT)

$(BINDIR):
	mkdir -p $(BINDIR)
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OUTPUT): $(OBJ)
	$(LINKER) -o $@ $^ $(LDFLAGS)

# dynamically generated dependency "graph"
-include $(OBJ:.o=.d)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -o $@ $< $(CXXFLAGS) -c -MMD
	@mv -f $(OBJDIR)/$*.d $(OBJDIR)/$*.d.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.d.tmp > $(OBJDIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.d
	@rm -f $(OBJDIR)/$*.d.tmp
	@sed -i '/\\\:/d' $(OBJDIR)/$*.d

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -o $@ $< $(CCFLAGS) -c -MMD
	@mv -f $(OBJDIR)/$*.d $(OBJDIR)/$*.d.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.d.tmp > $(OBJDIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.d
	@rm -f $(OBJDIR)/$*.d.tmp
	@sed -i '/\\\:/d' $(OBJDIR)/$*.d
