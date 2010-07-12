#!/usr/bin/gnuplot
# kate: encoding utf8
set terminal pdfcairo enhanced solid size 3,3 lw 2
#set terminal postscript portrait enhanced solid color
#set terminal pdfcairo enhanced color
set output "../tmp/temperatur.pdf"
#set size ratio 0.71 #a4
#set size 0.6
set size ratio 1
#set encoding utf8
set encoding iso_8859_1
#set title ""
#set xlabel "{/Symbol b}{/Symbol g}" enhanced
set ylabel "T in K"
set xlabel "Zeit t in der Einheit der Dauer eines Messdurchgangs"
#set logscale x
#set logscale y
set xtics 50
set mxtics 5
set ytics 25
set mytics 5
#set grid xtics ytics mxtics mytics
#set style line 1 lt 1 lw 3
#set key left top
#set key box
set nokey
#set format x "10^{%L}"
#set tmargin 0
#set rmargin 4
#set lmargin 8
#set bmargin 2.5
set grid xtics ytics 
plot "../data/Daten.DAT" using (100*($1)) with l lc 0