#!/usr/bin/env gnuplot

#          Copyright Tom Westerhout 2017.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

sm = 'static_map.dat'
um = 'unordered_map.dat'
empty = 'empty.dat'
name = 'out'

if (! exists("sm")) {
	print "Error: missing the 'sm' argument"
	exit 1
}

if (! exists("um")) {
	print "Error: missing the 'um' argument"
	exit 1
}

if (! exists("empty")) {
	print "Error: missing the 'empty' argument"
	exit 1
}

if (! exists("name")) {
	print "Error: missing the 'name' argument"
	exit 1
}

bin(x, width) = width * floor(x / width + .5)
normal(x, mu, sd) = exp(- (x - mu)**2 / (2 * sd**2))
half_normal(x, mu, sd) = (x >= mu) ? exp(- (x - mu)**2 / (2 * sd**2)) : 0.0

set style line 101 lt 1 lw 2 lc rgb "#2E911A"
set style line 102 lt 1 lw 2 lc rgb "#1A6A91"
set samples 1000000



#####################################################################

stats empty u 2 nooutput name "E"
binwidth = 1


table_file = '.temp.' . name . '.dat'
set table table_file
plot empty u (bin(column("ticks"), binwidth)):(1.0) smooth freq
unset table

set fit quiet
set fit errorvariables

A = E_records / 2
Mean = E_mean
Stddev = 2
fit A * normal(x, Mean, Stddev) table_file via A, Mean, Stddev

system("rm " . table_file)

set terminal pngcairo noenhanced color notransparent \
	font "DejaVu Sans Mono, 12"
set output name . '.empty.png'

set title 'NOP latency distribution' offset 0,-0.5
set xlabel 'Time, ticks' offset 0,0.5
set ylabel 'Count' offset 1,0
set xtics nomirror out
set ytics nomirror out format "%2.1E"
set key right top

plot [0:100] \
	empty u (bin(column("ticks"), binwidth)):(1.0) \
		smooth freq w boxes ls 101 fs transparent solid 0.8 \
		notitle, \
	A * normal(x, Mean, Stddev) \
		lt 1 lw 4 lc rgb "#1A6A91" notitle
set output


#####################################################################


set terminal pngcairo noenhanced color notransparent \
	font "DejaVu Sans Mono, 12"
set output name . '.time.png'

set title 'Lookup time' offset 0,-0.5
unset xlabel
set ylabel 'Time, ticks' offset 0.75,0
unset xtics
set ytics nomirror out
set key right top

set logscale y 10
plot \
	sm u 0:(column("ticks")) w p \
		lt 1 lw 1 pt 7 ps 0.3 lc rgb "#2E911A" title "static_map", \
	um u 0:(column("ticks")) w p \
		lt 1 lw 1 pt 7 ps 0.3 lc rgb "#1A6A91" title "unordered_map"
set output
unset logscale

# exit 0

#####################################################################


set terminal pngcairo noenhanced color notransparent \
	font "DejaVu Sans Mono, 12"
set output name . '.dist.png'

set title 'Lookup latency distribution' offset 0,-0.5
set xlabel 'Time, ticks' offset 0,0.5
set ylabel 'Count' offset 1,0
set xtics nomirror out
set ytics nomirror out
set key right top

binwidth = 3

set boxwidth binwidth
set style line 101 lt 1 lw 2 lc rgb "#2E911A"
set style line 102 lt 1 lw 2 lc rgb "#1A6A91"

plot [0:100][0:] \
	um u (bin(column("ticks"), binwidth)):(1.0) \
		smooth freq w boxes ls 102 fs transparent solid 0.8 \
		title "unordered_map", \
	sm u (bin(column("ticks"), binwidth)):(1.0) \
		smooth freq w boxes ls 101 fs transparent solid 0.50 \
		title "static_map"

set output

