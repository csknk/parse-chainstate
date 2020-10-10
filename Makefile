# Copyright (c) David Egan 2020
# SPDX-License-Identifier: GPL-2.0-or-later

WARNINGS = -pedantic -Wall -Wfatal-errors -Wextra -Wno-unused-parameter -Wno-unused-variable
BIN_DIR = bin
OBJS_DIR = $(BIN_DIR)/objs
# leveldb must not be a position independent executable, so the -no-pie flag is required
EXTRAFLAGS = -no-pie
#CXXFLAGS = $(EXTRAFLAGS) $(WARNINGS) -std=c++17 -g
# Don't compile with ASAN if using Valgrind...
CXXFLAGS = $(EXTRAFLAGS) $(WARNINGS) -std=c++17 -g -fsanitize=leak -fsanitize=address
LDFLAGS = 
LDLIBS = -lleveldb -lsnappy -lpthread 
CXX = g++ ${CXXFLAGS}
SRCS = $(wildcard *.cpp)
EXECUTABLE_FILES = $(BIN_DIR)/main
OBJECT_FILES = $(SRCS:%.cpp=$(OBJS_DIR)/%.o)

.PHONY: all
all: main $(OBJS_DIR)/DBWrapper.o $(OBJS_DIR)/main.o

main: $(OBJS_DIR)/DBWrapper.o $(OBJS_DIR)/main.o $(OBJS_DIR)/varint.o
	$(info Building executable from object file "$(^)")
	@$(CXX) $(LDFLAGS) -o $(BIN_DIR)/$@ $^ $(LDLIBS)

.PRECIOUS: $(OBJS_DIR)/%.o

$(OBJS_DIR)/DBWrapper.o: DBWrapper.cpp
	$(info Building object file for "$(<)")
	@mkdir -p $(@D)
	@$(CXX) -o $@ -c $<

$(OBJS_DIR)/varint.o: varint.cpp
	$(info Building object file for "$(<)")
	@mkdir -p $(@D)
	@$(CXX) -o $@ -c $<

$(OBJS_DIR)/main.o: main.cpp
	$(info Building object file for "$(<)")
	@mkdir -p $(@D)
	@$(CXX) -o $@ -c $<

.PHONY: clean
clean:
	$(info Running clean...)
	rm $(OBJS_DIR)/*.o
	rm $(BIN_DIR)/*
