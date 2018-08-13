#!/usr/bin/env gnuplot

#          Copyright Tom Westerhout 2018.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

data_file = 'benchmark_result.dat'

if (! exists("data_file")) {
	print "Error: missing the 'data_file' argument"
	exit 1
}

bin(x, width) = width * floor(x / width + .5)
normal(x, mu, sd) = exp(- (x - mu)**2 / (2 * sd**2))
half_normal(x, mu, sd) = (x >= mu) ? exp(- (x - mu)**2 / (2 * sd**2)) : 0.0

set style line 101 lt 1 pt 7 ps 1 lw 1 lc rgb "#2E911A"
set style line 102 lt 1 pt 7 ps 1 lw 1 lc rgb "#1A6A91"
set samples 1000000

#####################################################################

set terminal pngcairo noenhanced color notransparent \
	font "DejaVu Sans Mono, 12"
set output 'benchmark.lookups.png'

set title 'Key lookup using find()' offset 0,-0.5
set xlabel 'Number of elements' offset 0,0
set ylabel 'Time, ns' offset 0.5,0
set xtics nomirror out
set ytics nomirror out format "%g"
set key right top

plot [][:30]\
	data_file u (column("data_size")):(column("static")):(column("static_err")) \
		w p ls 101 title "boost::static_views::static_map", \
	data_file u (column("data_size")):(column("unordered")):(column("unordered_err")) \
		w p ls 102 title "std::unordered_map", \

set output

