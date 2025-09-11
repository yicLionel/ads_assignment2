#!/bin/bash

echo "=== Performance Test for dict2 ==="
echo

echo "Testing with dataset_1067.csv (largest dataset):"
echo "Running 10 queries..."

# Create a test file with 10 queries
cat > perf_test_keys.txt << EOF
18 PROFESSORS WALK
230 GRATTAN STREET
783 SWANSTON STREET
151 BERKELEY STREET
601/640 SWANSTON STREET
225-235 BOUVERIE STREET
18 SPENCER ROAD
14 KERNOT
161 BARRY
23 PROFESSORS WALK
EOF

echo "Input queries:"
cat perf_test_keys.txt
echo

echo "Running performance test..."
time ./dict2 2 tests/dataset_1067.csv perf_output.txt < perf_test_keys.txt

echo
echo "Performance test completed. Check perf_output.txt for results."
