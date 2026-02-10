# Output buffering helper (improves crash visibility)
STDBUF :=
ifneq ($(shell command -v stdbuf 2>/dev/null),)
    STDBUF := stdbuf --output=L --error=L
else
    # We check again only when run-tests is actually called
    STDBUF_MISSING_MSG := stdbuf not found (from GNU coreutils). Last printf lines may be missing on crash/abort.\nPlease add 'fflush(stdout);' after important prints or install coreutils.
endif

# Verbose mode (default silent)
ifeq ($(VERBOSE),1)
	SILENT_PREFIX :=
else
	SILENT_PREFIX := @
endif

# Automatic dependency tracking control (enabled by default)
ifeq ($(NO_DEPENDENCY_TRACKING),1)
	DEP_FLAGS :=
	DEPS :=
else
	DEP_FLAGS := -MMD -MP
	# All dependency files
	DEPS := $(LIB_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d) $(TEST_OBJECTS:.o=.d)
endif

ifneq ($(NO_DEPENDENCY_TRACKING),1)
	-include $(DEPS)
endif


clean:
	$(SILENT_PREFIX)rm -rf $(BUILD_DIR)

static-lib: rebuild-obj $(STATIC_LIB)

help:
	@echo "Usage: make [OPTIONS] [TARGET]"
	@echo ""
	@echo "TARGETS:"
	@echo "    help                 Print this help message"
	@echo "    all                  Build the main executable (default)"
	@echo "    tests                Build all test executables (no execution)"
	@echo "    rebuild              Clean and rebuild the main executable"
	@echo "    rebuild-tests        Clean and rebuild test executables"
	@echo "    static-lib           Build static library only (lib$(LIB_NAME).a)"
	@echo "    run-main             Run the main binary (uses Valgrind in valgrind-debug mode)"
	@echo "    run-gdb              Debug the main binary with gdb"
	@echo "    run-tests            Build and run all tests, reporting failures at the end"
	@echo "    clean                Remove all build artifacts and build folder"
	@echo ""
	@echo "OPTIONS:"
	@echo "    MODE=<str>                  Compilation type: release, debug, strict-debug, clang-debug, valgrind-debug"
	@echo "                                Default: release"
	@echo "                                Note: clang-debug requires Clang installed system-wide."
	@echo "                                Note: valgrind-debug requires Valgrind installed system-wide; runs binaries under Valgrind with leak checks."
	@echo "                                Logging in valgrind-debug and run-tests:"
	@echo "                                  - Creates timestamped folder: logs/valgrind-<dd-mm-YYYY-HH-MM-SS>/ or logs/tests-<dd-mm-YYYY-HH-MM-SS>/"
	@echo "                                  - run-main: main-run-valgrind.log"
	@echo "                                  - run-tests: per-test .log files (valgrind- prefixed when in valgrind-debug)"
	@echo "    MAIN_NAME=<str>             Set custom main entry point file (without .c; default: main)"
	@echo "    VERBOSE=1                   Show all compilation commands (default: silent)"
	@echo "    NO_DEPENDENCY_TRACKING=1    Disable automatic dependency tracking (.d files)"
	@echo "    EXTRA_CFLAGS=\"<str>\"        Add custom compiler flags"
	@echo "    EXTRA_LDFLAGS=\"<str>\"       Add custom linker flags"
	@echo ""
	@echo "Portability Notes:"
	@echo "    run-tests uses stdbuf (from GNU coreutils) if available for reliable output on crashes;"
	@echo "    warns if missing and suggests alternatives (e.g., fflush in test code)."