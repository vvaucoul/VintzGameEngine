# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/27 00:53:13 by vvaucoul          #+#    #+#              #
#    Updated: 2025/04/27 00:59:41 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Root Makefile for VintzGameEngine

# Path definitions
ROOT_DIR   := $(shell pwd)
BUILD_DIR  := $(ROOT_DIR)/build
BIN_DIR    := $(ROOT_DIR)/bin
ENGINE_DIR := $(ROOT_DIR)/Engine
PLUGINS_DIR:= $(ROOT_DIR)/Plugins
PLUGIN_DIRS:= $(wildcard $(PLUGINS_DIR)/*)

# Final binary name
BIN_NAME   := VintzGameEngine
BIN        := $(BIN_DIR)/$(BIN_NAME)

.PHONY: all engine plugins clean fclean re run

.SILENT:

all: engine plugins
	@echo "[INFO] Build complete."

engine:
	@echo "[INFO] Building Engine..."
	@$(MAKE) -s -C $(ENGINE_DIR)
	@echo "[INFO] Engine built."

plugins: $(PLUGIN_DIRS)
	@echo "[INFO] Building plugins..."
	@for dir in $(PLUGIN_DIRS); do \
		echo "[INFO] Plugin: $$dir"; \
		$(MAKE) -s -C $$dir; \
	done
	@echo "[INFO] Plugins built."

clean:
	@echo "[INFO] Cleaning Engine..."
	@$(MAKE) -s -C $(ENGINE_DIR) clean
	@echo "[INFO] Cleaning Plugins..."
	@for dir in $(PLUGIN_DIRS); do \
		$(MAKE) -s -C $$dir clean; \
	done
	@echo "[INFO] Clean complete."

fclean:
	@echo "[INFO] Full clean (fclean) Engine..."
	@$(MAKE) -s -C $(ENGINE_DIR) fclean
	@echo "[INFO] Full clean (fclean) Plugins..."
	@for dir in $(PLUGIN_DIRS); do \
		$(MAKE) -s -C $$dir fclean; \
	done

re: fclean all

run: all
	@echo "[INFO] Running $(BIN_NAME)..."
	@LD_LIBRARY_PATH=$(BIN_DIR)/plugins:$$LD_LIBRARY_PATH $(BIN)
