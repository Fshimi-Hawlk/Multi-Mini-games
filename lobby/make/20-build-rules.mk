# Prevent make from deleting intermediate object files
.SECONDARY: $(LIB_OBJECTS) $(MAIN_OBJECT) $(TEST_OBJECTS)

# Rules
$(BIN): $(LIB_OBJECTS) $(MAIN_OBJECT)
	$(SILENT_PREFIX)mkdir -p $(@D)
	# --start-group/--end-group: linker does multiple passes so circular
	# static deps (e.g. game libs <-> raylib 3D) are all resolved.
	$(SILENT_PREFIX)$(CC) $^ -Wl,--start-group $(LDFLAGS) -Wl,--end-group -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(SILENT_PREFIX)mkdir -p $(@D)
	$(SILENT_PREFIX)$(CC) $(CFLAGS) $(DEP_FLAGS) -c $< -o $@

$(TEST_BIN_DIR)/% : $(LIB_OBJECTS) $(OBJ_DIR)/tests/%.o
	$(SILENT_PREFIX)mkdir -p $(@D)
	$(SILENT_PREFIX)$(CC) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.c
	$(SILENT_PREFIX)mkdir -p $(@D)
	$(SILENT_PREFIX)$(CC) $(CFLAGS) -Isrc $(DEP_FLAGS) -c $< -o $@

$(STATIC_LIB): $(LIB_OBJECTS)
	@mkdir -p $(@D)
	$(SILENT_PREFIX)ar rcs $@ $^
