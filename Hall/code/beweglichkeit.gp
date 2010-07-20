#!/usr/bin/gnuplot
# kate: encoding utf8
set terminal pdfcairo enhanced solid size 3,3 lw 2
#set terminal postscript portrait enhanced solid color
#set terminal pdfcairo enhanced color
set output "../tmp/beweglichkeit.pdf"
#set size ratio 0.71 #a4
#set size 0.6
set size ratio 1
#set encoding utf8
set encoding iso_8859_1
#set title ""
set xlabel "{/Symbol b}{/Symbol g}" enhanced
set xlabel "ln (T / K)"
set ylabel "ln μ(T)"
#set logscale x
#set logscale y
#set xtics 5
#set mxtics 5
#set ytics 0.5
#set mytics 4
#set grid xtics ytics mxtics mytics
#set style line 1 lt 1 lw 3
#set key left top
#set key box
set nokey
#set format x "10^{%L}"
set xtics 0.2
set mxtics 3
set grid xtics ytics 
#set tmargin 0
#set rmargin 0.0
plot "../data/Daten.DAT" using (log(abs(100*($1)))):(log(abs(0.3661327231121282*($3-$5)/2/(4*($2-$4)/55)))) with lp pt 7 ps 0.3 lc 0