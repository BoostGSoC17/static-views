#!/usr/bin/env gnuplot

#          Copyright Tom Westerhout 2017.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

if (! exists("testcase")) {
	print "Error: missing the 'filename' argument"
	exit 1
}

output_file = testcase . ".png"
input_file  = testcase . ".dat" 

set terminal pngcairo noenhanced color notransparent
set output output_file

set title 'Memory usage for test "' . testcase . '"'
set xlabel "Input size"
set ylabel "Peak memory usage, Mb"
set key left top

plot \
	input_file u 1:($2/1024) w l lt 1 lw 1 lc rgb "#777777" notitle, \
	""         u 1:($2/1024) w p pt 7 ps 1 lc rgb "#2E911A" title 'RAM', \
	""         u 1:($3/1024) w l lt 1 lw 1 lc rgb "#777777" notitle, \
	""         u 1:($3/1024) w p pt 7 ps 1 lc rgb "#1A6A91" title 'Swap'

set output
