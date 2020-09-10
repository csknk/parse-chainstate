# Copyright (c) David Egan 2020
# SPDX-License-Identifier: GPL-2.0-or-later

WARNINGS = -pedantic -Wall -Wfatal-errors -Wextra -Wno-unused-parameter -Wno-unused-variable
BIN_DIR = bin
OBJS_DIR = $(BIN_DIR)/objs
# leveldb must not be a position independent executable, so -no-pie
EXTRAFLAGS = -no-pie
CXXFLAGS = $(EXTRAFLAGS) $(WARNINGS) -std=c++17 -g
LDFLAGS = 
LDLIBS = -lleveldb -lsnappy -lpthread
CXX = g++ ${CXXFLAGS}
SRCS = $(wildcard *.cpp)
EXECUTABLE_FILES = $(BIN_DIR)/main
OBJECT_FILES = $(SRCS:%.cpp=$(OBJS_DIR)/%.o)

.PHONY: all
#all: $(EXECUTABLE_FILES)
all: main $(OBJS_DIR)/DBWrapper.o $(OBJS_DIR)/ob-test.o

main: $(OBJS_DIR)/DBWrapper.o $(OBJS_DIR)/ob-test.o
#	$(info Building executable from object file "$(OBJS_DIR)/$(^)")
	$(info Building executable from object file "$(^)")
	@$(CXX) $(LDFLAGS) -o $(BIN_DIR)/$@ $^ $(LDLIBS)

.PRECIOUS: $(OBJS_DIR)/%.o

$(OBJS_DIR)/DBWrapper.o: DBWrapper.cpp
	$(info Building object file for "$(<)")
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJS_DIR)/ob-test.o: ob-test.cpp
	$(info Building object file for "$(<)")
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -o $@ -c $<


# Specify if header files need to be linked with referenced object files
#$(OBJS_DIR)/DBWrapper.o: DBWrapper.h utilities.h

.PHONY: clean
clean:
	$(info Running clean...)
	rm $(OBJS_DIR)/*.o
	rm $(BIN_DIR)/*
