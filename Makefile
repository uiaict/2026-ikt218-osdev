PROJECT_DIR := src/31_group31
BUILD_DIR := build/31_group31
ISO := $(BUILD_DIR)/kernel.iso
QEMU_AUDIO := $(PROJECT_DIR)/scripts/run_qemu_audio.sh

.PHONY: start run build configure clean help

start: build
	$(QEMU_AUDIO) $(ISO)

run: start

build: configure
	cmake --build $(BUILD_DIR)

configure:
	@if [ ! -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		echo "Configuring KebabOS build directory..."; \
		cmake -S $(PROJECT_DIR) -B $(BUILD_DIR); \
	fi

clean:
	cmake --build $(BUILD_DIR) --target clean

help:
	@echo "KebabOS shortcuts:"
	@echo "  make start  Build and boot KebabOS in QEMU with audio"
	@echo "  make run    Same as make start"
	@echo "  make build  Build kernel.iso only"
	@echo "  make clean  Clean CMake build outputs"
