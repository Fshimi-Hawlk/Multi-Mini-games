tests: $(TEST_BINS)
	@if [ -z "$(TEST_BINS)" ]; then \
		echo "No tests found — nothing to build."; \
	else \
		echo "Test binaries built: $(notdir $(TEST_BINS))"; \
	fi


run-tests: tests
	@if [ -z "$(TEST_BINS)" ]; then \
		:; \
	else \
		$(eval TIMESTAMP := $(shell date "+%d-%m-%Y-%H-%M-%S")) \
		$(eval LOG_DIR := logs/tests-$(TIMESTAMP)) \
		mkdir -p "$(LOG_DIR)" && \
		if [ -n "$(STDBUF_MISSING_MSG)" ]; then \
			printf "\nWarning: %s\n\n" "$(STDBUF_MISSING_MSG)"; \
		fi && \
		echo "Running all tests (see stdout+stderr in $(LOG_DIR))..." && \
		echo "=====================================================================" && \
		all_passed=1; \
		passed_test_count=0; \
		test_count=0; \
		max_name=0; \
		max_log=0; \
		results_file="$$(mktemp)"; \
		: > "$$results_file"; \
		green='\033[32m'; \
		red='\033[31m'; \
		yellow='\033[33m'; \
		reset='\033[0m'; \
		for test_bin in $(TEST_BINS); do \
			if [ -f "$$test_bin" ]; then \
				test_count=$$((test_count + 1)); \
				test_name=$$(basename "$$test_bin"); \
				name_len="$${#test_name}"; \
				log_len="$${#log_file}"; \
				\
				if [ "$$name_len" -gt "$$max_name" ]; then max_name="$$name_len"; fi; \
				if [ "$$log_len" -gt "$$max_log" ]; then max_log="$$log_len"; fi; \
				if [ "$(MODE)" = "valgrind-debug" ]; then \
					log_file="$(LOG_DIR)/valgrind-$$test_name.log"; \
				else \
					log_file="$(LOG_DIR)/$$test_name.log"; \
				fi; \
				echo ">>> Running $$test_name..."; \
				echo "Command: $(STDBUF) $(TOOL) ./$$test_bin"; \
				echo ""; \
				echo "─── Output ─────────────────────────────────────────────────────────"; \
				\
				stdout_tmp=$$(mktemp); \
				stderr_tmp=$$(mktemp); \
				\
				( $(STDBUF) $(TOOL) ./$$test_bin > >(tee "$$stdout_tmp") 2> >(tee "$$stderr_tmp" >&2) ); \
				test_status=$$?; \
				\
				{ \
					stdout_content=$$(cat "$$stdout_tmp"); \
					stderr_content=$$(cat "$$stderr_tmp"); \
					\
					if [ -n "$$stdout_content" ]; then \
						echo "==================== STDOUT ===================="; \
						echo "$$stdout_content"; \
					fi; \
					\
					if [ -n "$$stderr_content" ]; then \
						echo ""; \
						echo "==================== STDERR ===================="; \
						echo "$$stderr_content"; \
						echo ""; \
					fi; \
					\
					if [ -n "$$stdout_content" ] || [ -n "$$stderr_content" ]; then \
						echo "================================================"; \
					fi; \
				} > "$$log_file"; \
				\
				rm -f "$$stdout_tmp" "$$stderr_tmp"; \
				echo "────────────────────────────────────────────────────────────────────"; \
				echo ""; \
				if [ $$test_status -eq 0 ]; then \
					printf ">>> $$test_name: $${green}PASSED$${reset}\n"; \
					passed_test_count=$$((passed_test_count + 1)); \
					printf "%s\tPASSED\t%s\n" "$$test_name" "$$log_file" >> "$$results_file"; \
				else \
					printf ">>> $$test_name: $${red}FAILED$${reset} (details in $$log_file)\n"; \
					all_passed=0; \
					printf "%s\tFAILED\t%s\n" "$$test_name" "$$log_file" >> "$$results_file"; \
				fi; \
				echo "────────────────────────────────────────────────────────────────────"; \
				echo ""; \
			else \
				printf "Test binary $$test_bin $${yellow}not found$${reset}\n"; \
				all_passed=0; \
				test_count=$$((test_count + 1)); \
				name_len="$${#test_name}"; \
				if [ "$$name_len" -gt "$$max_name" ]; then max_name="$$name_len"; fi; \
				printf "%s\tNOT FOUND\t-\n" "$$(basename $$test_bin)" >> "$$results_file"; \
			fi; \
		done; \
		echo "====================================================================="; \
		echo ""; \
		echo "Test Results:"; \
		\
		# Add small padding for readability \
		name_width=$$((max_name + 2)); \
		log_width=$$((max_log + 2)); \
		status_width=7; \
		\
		# Top border \
		printf '┌─%s┬─%s┬─%s┐\n' \
			"$$(printf '─%.0s' $$(seq 1 $$name_width))" \
			"$$(printf '─%.0s' $$(seq 1 $$status_width))" \
			"$$(printf '─%.0s' $$(seq 1 $$log_width))"; \
		\
		# Header \
		printf "│ %-*s│ %-*s│ %-*s│\n" \
			"$$name_width" "Test Name" \
			"$$status_width" "Status" \
			"$$log_width"  "Log File"; \
		\
		# Separator \
		printf '├─%s┼─%s┼─%s┤\n' \
			"$$(printf '─%.0s' $$(seq 1 $$name_width))" \
			"$$(printf '─%.0s' $$(seq 1 $$status_width))" \
			"$$(printf '─%.0s' $$(seq 1 $$log_width))"; \
		\
		# Data rows \
		while IFS=$$'\t' read -r name status log; do \
			[ -z "$$log" ] && log="-"; \
			if [ "$$status" = "PASSED" ]; then \
				color="$${green}"; \
			elif [ "$$status" = "FAILED" ]; then \
				color="$${red}"; \
			else \
				color="$${yellow}"; \
			fi; \
			printf "│ %-*s│ $${color}%-*s$${reset}│ %-*s│\n" \
				"$$name_width"  "$$name" \
				"$$status_width" "$$status" \
				"$$log_width"   "$$log"; \
		done < "$$results_file"; \
		\
		# Bottom border \
		printf '└%s─┴%s─┴%s─┘\n' \
			"$$(printf '─%.0s' $$(seq 1 $$name_width))" \
			"$$(printf '─%.0s' $$(seq 1 $$status_width))" \
			"$$(printf '─%.0s' $$(seq 1 $$log_width))"; \
		\
		echo ""; \
		failed=$$((test_count - passed_test_count)); \
		printf "Passed: $${green}%d$${reset}   Failed: $${red}%d$${reset}   Total: %d\n" \
			$$passed_test_count $$failed $$test_count; \
		echo ""; \
		if [ $$all_passed -eq 1 ]; then \
			echo "All tests passed."; \
		else \
			echo "Some tests failed. See logs in $(LOG_DIR)"; \
			rm -f "$$results_file"; \
			exit 1; \
		fi; \
		rm -f "$$results_file"; \
	fi