#!/usr/bin/gnuplot
# kate: encoding utf8
set terminal pdfcairo enhanced solid size 3,3 lw 2
#set terminal postscript portrait enhanced solid color
#set terminal pdfcairo enhanced color
#set size ratio 0.71 #a4
#set size 0.6
set size ratio 1
#set encoding utf8
set encoding iso_8859_1
#set title ""
set xlabel "{/Symbol b}{/Symbol g}" enhanced
set xlabel "T^{-1} in K^{-1})"
set ylabel "ln T^{3/2}R_H m^3C^{-1}"
#set logscale x
#set logscale y
#set xtics 5
#set mxtics 5
#set ytics 0.5
#set mytics 4
#set grid xtics ytics mxtics mytics
#set style line 1 lt 1 lw 3
set key left top
set key box
#set nokey
#set format x "10^{%L}"
#set xtics 0.003
#set mxtics 3
set grid xtics ytics 
set output "../tmp/Eg_R_H_00.pdf"
fit m*x + n  "../data/Eg_00.dat" using (1/$1):(log(abs(0.3661327231121282*($3-$5)/2)*(100*$1)**(1.5)))  via m, n
plot "../data/Eg_00.dat" using (1/$1):(log(abs(0.3661327231121282*($3-$5)/2)*(100*$1)**(1.5))) with p pt 7 ps 0.3 lc 0 title "ln T^{3/2}R_H m^3C^{-1}", m*x + n lc 1 title "Fit"

set output "../tmp/Eg_R_H_01.pdf"
fit m*x + n  "../data/Eg_01.dat" using (1/$1):(log(abs(0.3661327231121282*($3-$5)/2)*(100*$1)**(1.5)))  via m, n
plot "../data/Eg_01.dat" using (1/$1):(log(abs(0.3661327231121282*($3-$5)/2)*(100*$1)**(1.5))) with p pt 7 ps 0.3 lc 0 title "ln T^{3/2}R_H m^3C^{-1}", m*x + n lc 1 title "Fit"
