#!/bin/bash

echo "=== Testing dict2 with various test cases ==="
echo

echo "1. Testing with dataset_1.csv (single record):"
echo "   Input: test_keys.txt"
./dict2 2 tests/dataset_1.csv test_output1.txt < test_keys.txt
echo "   Output saved to: test_output1.txt"
echo

echo "2. Testing with dataset_22.csv (multiple records):"
echo "   Input: test_keys_partial.txt"
./dict2 2 tests/dataset_22.csv test_output22.txt < test_keys_partial.txt
echo "   Output saved to: test_output22.txt"
echo

echo "3. Testing exact matches with dataset_22.csv:"
echo "   Input: test_keys_exact.txt"
./dict2 2 tests/dataset_22.csv test_output_exact.txt < test_keys_exact.txt
echo "   Output saved to: test_output_exact.txt"
echo

echo "4. Testing edge cases:"
echo "   Empty input test:"
echo "" | ./dict2 2 tests/dataset_1.csv test_empty.txt
echo "   Output saved to: test_empty.txt"
echo

echo "5. Testing with non-existent key:"
echo "NONEXISTENT_KEY_12345" | ./dict2 2 tests/dataset_1.csv test_nonexistent.txt
echo "   Output saved to: test_nonexistent.txt"
echo

echo "=== All tests completed ==="
echo "Check the output files for detailed results."
