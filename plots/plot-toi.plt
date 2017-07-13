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
set output 'hightoi.eps'

set grid ytics lt 0 lw 1 lc rgb "#cccccc"
set grid xtics lt 0 lw 1 lc rgb "#cccccc"
set style line 1 lt 1 pt 8 ps 2 lw 3 lc 0
set style line 2 lt 2 pt 9 ps 1.7 lw 2 lc 0
set style line 3 lt 3 pt 4 ps 2 lw 3 lc 1

#set title "Média total de interesses retransmitidos\nMulticast Melhorado vs Multicast Nativo em cenário de rodovia"
set xlabel "Number of Nodes"
set ylabel "FIP"
set xtics 25
set ytics 200

set xrange [22:102]
set yrange [0:1000]
#high_pdr.dat
#high_pdr.dat
plot "hightoi.dat" using 1:2:3:4  title "Naive Multicast" with linespoints ls 1,\
"hightoi.dat" using 1:2:3:4 notitle with yerrorbars ls 1,\
1 / 0 notitle  smooth csplines with lines ls 1,\
\
"hightoi.dat" using 1:5:6:7 title "Naive Best-Route" with linespoints ls 2,\
"hightoi.dat" using 1:5:6:7 notitle w yerrorbars ls 2,\
2 / 0 notitle  smooth csplines with lines ls 2,\
\
"hightoi.dat" using 1:8:9:10 title "LSIF" with linespoints ls 3,\
"hightoi.dat" using 1:8:9:10 notitle w yerrorbars ls 3,\
2 / 0 notitle  smooth csplines with lines ls 3
