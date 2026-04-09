all: $(BIN)

rebuild: clean all

rebuild-obj: clean $(LIB_OBJECTS)

rebuild-tests: clean tests

static-lib: rebuild-obj 

run-main:
	@if [ -f $(BIN) ]; then \
		if [ "$(MODE)" = "valgrind-debug" ]; then \
			$(eval TIMESTAMP := $(shell date "+%d-%m-%Y-%H-%M-%S")) \
			$(eval LOG_DIR := logs/valgrind-$(TIMESTAMP)) \
			mkdir -p "$(LOG_DIR)" && \
			log_file="$(LOG_DIR)/main-run-valgrind.log" && \
			echo "Running main under Valgrind... (output saved to $$log_file)" && \
			$(TOOL) $(BIN) > "$$log_file" 2>&1 && \
			echo "Valgrind finished. Log: $$log_file" || \
			(echo "Valgrind failed. See $$log_file" && exit 1); \
		else \
			$(TOOL) $(BIN); \
		fi; \
	else \
		echo "No executable"; \
	fi

run-gdb:
	@if [ -f $(BIN) ]; then gdb $(BIN); else echo "No executable"; fi
