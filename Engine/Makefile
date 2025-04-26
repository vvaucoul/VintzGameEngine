# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/27 00:47:50 by vvaucoul          #+#    #+#              #
#    Updated: 2025/04/27 00:59:41 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# -------------------------------------------------------------------------- #
#  Path configuration                                                        #
# -------------------------------------------------------------------------- #

ROOT_DIR   ?= $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
BUILD_DIR  ?= $(ROOT_DIR)/build
BIN_DIR    ?= $(ROOT_DIR)/bin

# -------------------------------------------------------------------------- #
#  Tools and flags                                                           #
# -------------------------------------------------------------------------- #

CXX        := g++
CC         := gcc

INCLUDES   := -I$(ROOT_DIR)/include \
              -I$(ROOT_DIR)/Engine \
              -I$(ROOT_DIR)/third_party/glad/include \
              -I/usr/include/stb

CXXFLAGS   := -std=c++17 -Wall -Wextra $(INCLUDES) -MMD -MP -g
CFLAGS     := -std=c99      -Wall -Wextra $(INCLUDES) -MMD -MP -g

LDFLAGS    := -lGL -ldl -lpthread -lX11 -lXrandr -lXi \
              -lXxf86vm -lXcursor -lXinerama -lglfw \
              -lstb -lassimp

# -------------------------------------------------------------------------- #
#  Files                                                                     #
# -------------------------------------------------------------------------- #

SRCS       := $(shell find . -name '*.cpp')
OBJ_DIR    := $(BUILD_DIR)/obj/engine
OBJS       := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEP_DIR    := $(BUILD_DIR)/deps/engine
DEPS       := $(patsubst %.cpp,$(DEP_DIR)/%.d,$(SRCS))
GLAD_SRC   := $(ROOT_DIR)/third_party/glad/src/glad.c
OBJ_GLAD   := $(OBJ_DIR)/glad.o
DEP_GLAD   := $(DEP_DIR)/glad.d
BIN_NAME   := VintzGameEngine
BIN        := $(BIN_DIR)/$(BIN_NAME)

.PHONY: all clean fclean re

.SILENT:

# -------------------------------------------------------------------------- #
#  Main rules                                                                #
# -------------------------------------------------------------------------- #

all: $(BIN)

$(BIN): $(OBJS) $(OBJ_GLAD)
	@mkdir -p $(dir $@)
	@echo "[LINK] $@"
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[SUCCESS] Engine build finished: $@"

# -------------------------------------------------------------------------- #
#  C++ Compilation (.cpp → .o)                                                #
# -------------------------------------------------------------------------- #

$(OBJ_DIR)/%.o: %.cpp
	@echo "[CXX] Compiling $<"
	@mkdir -p $(dir $@) \
	         $(dir $(patsubst $(OBJ_DIR)/%.o,$(DEP_DIR)/%.d,$@))
	$(CXX) $(CXXFLAGS) -c $< -o $@ \
	        -MF $(patsubst $(OBJ_DIR)/%.o,$(DEP_DIR)/%.d,$@)

# -------------------------------------------------------------------------- #
#  C Compilation (GLAD)                                                      #
# -------------------------------------------------------------------------- #

$(OBJ_GLAD): $(GLAD_SRC)
	@echo "[CC] Compiling $<"
	@mkdir -p $(dir $@) $(dir $(DEP_GLAD))
	$(CC) $(CFLAGS) -c $< -o $@ -MF $(DEP_GLAD)

# -------------------------------------------------------------------------- #
#  Auto-inclusion of dependencies                                            #
# -------------------------------------------------------------------------- #

-include $(DEPS) $(DEP_GLAD)

# -------------------------------------------------------------------------- #
#  Cleaning                                                                  #
# -------------------------------------------------------------------------- #

clean:
	@echo "[CLEAN] Engine"
	@rm -rf $(OBJ_DIR) $(DEP_DIR)

fclean: clean
	@echo "[FCLEAN] Engine"
	@rm -f $(BIN)

re: fclean all
