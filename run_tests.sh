#!/bin/bash
# run_tests.sh

# Path to your compiled simulator executable
SIMULATOR="./z16sim.exe"
# Directory where your test files are located
TEST_DIR="tests"

# ANSI escape codes for colored output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "Running Z16 Simulator Tests..."
echo "---------------------------------"

# Loop through all .bin files in the tests directory
for test_bin in "$TEST_DIR"/*.bin; do
    # Check if the file exists (important for when no .bin files are found)
    if [ -f "$test_bin" ]; then
        # Extract the base name of the test file (e.g., "TC-ZX16-01_ADD")
        test_name=$(basename "$test_bin" .bin)
        expected_file="$TEST_DIR/$test_name.expected"
        actual_file="$TEST_DIR/$test_name.actual"

        echo -n "  Testing: $test_name ... "

        # 1. Run the simulator with the current .bin file
        #    Redirect all output (stdout and stderr) to the .actual file
        "$SIMULATOR" "$test_bin" > "$actual_file" 2>&1

        # 2. Compare the generated .actual file with the .expected file
        #    -q (quiet): suppresses output
        #    >/dev/null: redirects diff's stdout (if any) to nowhere
        if diff -q "$actual_file" "$expected_file" >/dev/null; then
            echo -e "${GREEN}PASS${NC}"
            rm "$actual_file" # Clean up the .actual file if the test passed
        else
            echo -e "${RED}FAIL${NC}"
            echo "    Differences found! Comparing '$actual_file' vs '$expected_file':"
            diff "$actual_file" "$expected_file" # Show the differences
        fi
    fi
done

echo "---------------------------------"
echo "All tests finished."