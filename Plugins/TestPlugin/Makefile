# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/27 00:53:42 by vvaucoul          #+#    #+#              #
#    Updated: 2025/04/27 00:59:41 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Makefile for TestPlugin

ROOT_DIR   ?= $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../..)
BUILD_DIR  ?= $(ROOT_DIR)/build
BIN_DIR    ?= $(ROOT_DIR)/bin
SRC_DIR    := .
OBJ_DIR    := $(BUILD_DIR)/obj/plugins/TestPlugin
PLUGIN_BIN := $(BIN_DIR)/plugins

CXX        := g++
CXXFLAGS   := -std=c++17 -Wall -Wextra -fPIC -I$(ROOT_DIR)/Engine -g
LDFLAGS    := -shared

PLUGIN_NAME:= TestPlugin
SRC        := $(SRC_DIR)/$(PLUGIN_NAME).cpp
OBJ        := $(OBJ_DIR)/$(PLUGIN_NAME).o
SO         := $(PLUGIN_BIN)/lib$(PLUGIN_NAME).so

.PHONY: all clean fclean re

.SILENT:

all: dirs $(SO)

dirs:
	@mkdir -p $(OBJ_DIR) $(PLUGIN_BIN)

$(OBJ): $(SRC)
	@echo "[CXX] $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SO): $(OBJ)
	@echo "[LINK] $@"
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	@echo "[CLEAN] TestPlugin"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "[FCLEAN] TestPlugin"
	@rm -f $(SO)

re: fclean all
