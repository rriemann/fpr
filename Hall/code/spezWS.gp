#!/usr/bin/gnuplot
# kate: encoding utf8
set terminal pdfcairo enhanced solid size 3,3 lw 2
#set terminal postscript portrait enhanced solid color
#set terminal pdfcairo enhanced color
set output "../tmp/spezWS.pdf"
#set size ratio 0.71 #a4
#set size 0.6
set size ratio 1
#set encoding utf8
set encoding iso_8859_1
#set title ""
#set xlabel "{/Symbol b}{/Symbol g}" enhanced
set xlabel "T^{-1} in K^{-1}"
set ylabel "ln ρ(Ωm)^{-1}"
#set logscale x
#set logscale y
set xtics 0.003
set mxtics 3
#set ytics 0.5
#set mytics 4
set grid xtics ytics
#set style line 1 lt 1 lw 3
#set key left top
#set key box
set nokey
#set format x "10^{%L}"
set tmargin 0.0
set rmargin 0.2
set lmargin 8
set bmargin 2.5
plot [0.003:0.019]"../data/Daten.DAT" using (1/(100*($1))):(log(4*($2-$4)/55)) with p pt 7 ps 0.2 lc 0