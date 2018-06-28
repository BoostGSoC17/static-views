#!/bin/bash

declare -r SRC_FILE="static_map_vs_unordered_map.cpp"
declare -r DATA_FILE="test_data.ipp"
declare -r OUT_FILE="benchmark_result"
declare -r REPORT_FILE="benchmark_result.dat"

try_compile ()
{
	declare -ir BUCKET_SIZE=$1
	declare -ir BUCKET_COUNT=$2

	clang++ -O3 -march=native -std=c++14 -I../../../include \
		-fconstexpr-depth=-1 \
		-fconstexpr-steps=-1 \
		-DNDEBUG \
		-DBUCKET_SIZE=$BUCKET_SIZE \
		-DBUCKET_COUNT=$BUCKET_COUNT \
		-DBOOST_STATIC_VIEWS_DISABLE_CHECKS \
		"$SRC_FILE" \
		-lbenchmark -lpthread
}

generate_data ()
{
	declare -ir DATA_SIZE=$1
	rm -f "$DATA_FILE"
	for i in $(seq $DATA_SIZE); do
		echo "{$RANDOM, $RANDOM}," >> $DATA_FILE
	done
}

benchmark_one ()
{
	declare -ir DATA_SIZE=$1
	declare -ir BUCKET_COUNT=$((2 * DATA_SIZE))
	declare -i BUCKET_SIZE=1

	generate_data $DATA_SIZE

	for i in {2..5}; do
		echo "Compiling with bucket_size = $i..." >&2
		try_compile $i $BUCKET_COUNT 2>/dev/null
		if [[ $? -eq 0 ]]; then
			echo "Success for $i!" >&2
			BUCKET_SIZE=$i
			break
		fi
	done
	# To get useful output
	try_compile 5 $BUCKET_COUNT || exit 1

	./a.out --benchmark_format=console \
	        --benchmark_out_format=csv \
	        --benchmark_out="$OUT_FILE.$DATA_SIZE.csv"

	declare -r C1=$(grep "static_map_lookup_BigO" "$OUT_FILE.$DATA_SIZE.csv" | cut -d',' -f 3)
	declare -r C1_ERR=$(grep "static_map_lookup_RMS" "$OUT_FILE.$DATA_SIZE.csv" | cut -d',' -f 3)
	declare -r C2=$(grep "unordered_map_lookup_BigO" "$OUT_FILE.$DATA_SIZE.csv" | cut -d',' -f 3)
	declare -r C2_ERR=$(grep "unordered_map_lookup_RMS" "$OUT_FILE.$DATA_SIZE.csv" | cut -d',' -f 3)
	echo -e "$DATA_SIZE\t$BUCKET_SIZE\t$C1\t$C1_ERR\t$C2\t$C2_ERR" >> "$REPORT_FILE"
}

benchmark ()
{
	echo "# This file is generated automatically by benchmark.sh" > "$REPORT_FILE"
	echo "#" >> "$REPORT_FILE"
	echo -e "data_size\tbucket_size\tstatic\tstatic_err\tunordered\tunordered_err" >> "$REPORT_FILE"
	for i in {1..10}; do
		benchmark_one 128
		benchmark_one 256
		benchmark_one 512
		benchmark_one 1024
		benchmark_one 2048
		benchmark_one 4096
		benchmark_one 8192
	done
}

benchmark
