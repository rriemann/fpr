#!/usr/bin/gnuplot
# kate: encoding utf8
set terminal pdfcairo enhanced solid size 3,3 lw 2
#set terminal postscript portrait enhanced solid color
#set terminal pdfcairo enhanced color
set output "../tmp/similarity.pdf"
#set size ratio 0.71 #a4
#set size 0.6
set size ratio 1
set encoding utf8
#set encoding iso_8859_1
#set title ""
set xlabel "{/Symbol b}{/Symbol g}" enhanced
set xlabel "ln (T / K)"
set ylabel "ln μ(T)"
set logscale x
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
f(x) = y0 + A*exp((x-x0)/s)
y0 = 0
x0 = 0
s = 1
A = 1
fit f(x) "../data/similarity.dat" using 1:3 via y0,x0,s,A
plot "../data/similarity.dat" using ($1):($3) with p pt 7 ps 0.3, f(x) t "Fit"