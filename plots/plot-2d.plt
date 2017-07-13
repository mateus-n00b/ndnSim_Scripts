#!/usr/bin/gnuplot
# A Script to create graphs with errorbars
#
# Mateus Sousa (n00b), May 2017
#
# License GPLv3
#
# Version 1.0

#set terminal pdf
set terminal postscript eps color "Times" 20
set encoding utf8
set output 'output.eps'

set style line 1 lt 1 pt 8 ps 2 lw 3 lc 0
set style line 2 lt 2 pt 9 ps 1.7 lw 2 lc 0
set style line 3 lt 3 pt 4 ps 2 lw 3 lc 1

set title "Taxa de entrega de pacotes IP vs NDN em cenário de rodovia"
set xlabel "Número de veículos"
set ylabel "PDR\n(%)"
set xtics 25
set ytics 10

set xrange [22:102]
set yrange [0:100]
#high_pdr.dat
#urban_pdr.dat
plot "high_pdr.dat" using 1:2:3:4  title "IP" with linespoints ls 1,\
"high_pdr.dat" using 1:2:3:4 notitle with yerrorbars ls 1,\
1 / 0 notitle  smooth csplines with lines ls 1,\
\
"high_pdr.dat" using 1:5:6:7 title "ICN" with linespoints ls 2,\
"high_pdr.dat" using 1:5:6:7 notitle w yerrorbars ls 2,\
2 / 0 notitle  smooth csplines with lines ls 2
