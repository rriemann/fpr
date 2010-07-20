#!/usr/bin/gnuplot
# kate: encoding utf8
set terminal pdfcairo enhanced solid size 3,3 lw 2
#set terminal postscript portrait enhanced solid color
#set terminal pdfcairo enhanced color
set output "../tmp/hallkonstante2.pdf"
#set size ratio 0.71 #a4
#set size 0.6
set size ratio 1
#set encoding utf8
set encoding iso_8859_1
#set title ""
set xlabel "{/Symbol b}{/Symbol g}" enhanced
set xlabel "1/T in K^{-1}"
set ylabel "R_H in m^3 C^{-1}"
#set logscale x
#set logscale y
#set xtics 5
set mxtics 5
#set ytics 0.5
#set mytics 4
#set grid xtics ytics mxtics mytics
#set style line 1 lt 1 lw 3
#set key left top
#set key box
set nokey
#set format x "10^{%L}"
set xtics 0.003
set mxtics 3
set grid xtics ytics 
plot [0.003:0.019] "../data/Daten.DAT" using (1/(100*($1))):(((0.3661327231121282*($3-$5)/2))) with lp pt 7 ps 0.3 lc 0