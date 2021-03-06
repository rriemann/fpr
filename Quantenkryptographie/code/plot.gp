#!/usr/bin/gnuplot
# kate: encoding utf8
set terminal pdfcairo enhanced solid size 3,3 lw 2
#set terminal postscript portrait enhanced solid color
#set terminal pdfcairo enhanced color
set output "../tmp/similarity_vs_Hz.pdf"
#set size ratio 0.71 #a4
#set size 0.6
set size ratio 1
set encoding utf8
#set encoding iso_8859_1
#set title ""
#set xlabel "{/Symbol b}{/Symbol g}" enhanced
set xlabel "log (f / Hz)"
set ylabel "Similarity / %"
#set logscale x
#set logscale y
#set xtics 3
#set mxtics 3
#set ytics 0.5
#set mytics 4
#set grid xtics ytics mxtics mytics
#set style line 1 lt 1 lw 3
#set key left top
#set key box
set nokey
#set format x "10^{%L}"
#set xtics 0.2
set mxtics 3
set grid xtics ytics 
#set tmargin 0
#set rmargin 0.0
set key box
set key left
error_frequ = 0.01
error_attenuation = 0.01

plot "../data/similarity.dat" using (log10($1)):($5):(error_attenuation*$5) pt 7 ps 0.5 t "{/Symbol a = 4,5}" with yerrorbars , "" using (log10($1)):($2):(error_attenuation*$2) pt 1 ps 0.7 lc 0 t "{/Symbol a = 5,0}" with yerrorbars, "" using (log10($1)):($3):(error_attenuation*$3) pt 2 ps 0.7 t "{/Symbol a = 5,5}" with yerrorbars, "" using (log10($1)):($4):(error_attenuation*$4) pt 4 ps 0.5 lc 2 t "{/Symbol a} = 6,0" with yerrorbars
############################## um zu beachten, dass der fehler durch alpha nicht symmetrisch ist, muesste man ja die funktion kennen, und dann fehlerfortpflanzung betreiben. geht hier nicht. also: wieder annahme 1prozent

set output "../tmp/similarity_vs_alpha1.pdf"
set xlabel "Dämpfungsexponent {/Symbol a}"
set xtics 0.25
set mxtics 2
set grid mxtics ytics 
set key right

plot "../data/similarity_transponiert1.dat" using ($1):($2):(error_frequ*$2) pt 7 ps 0.5 t "f = 50 Hz" with yerrorbars, "" using ($1):($3):(error_frequ*$3) pt 1 ps 0.7 lc 0 t "f = 100 Hz" with yerrorbars, "" using ($1):($4):(error_frequ*$4) pt 2 ps 0.7 t "f = 500 Hz" with yerrorbars

set output "../tmp/similarity_vs_alpha2.pdf"
plot "../data/similarity_transponiert2.dat" using ($1):($2):(error_frequ*$2) pt 7 ps 0.5 t "f =   1 kHz" with yerrorbars, "" using ($1):($3):(error_frequ*$3) pt 1 ps 0.7 lc 0 t "f =   5 kHz" with yerrorbars, "" using ($1):($4):(error_frequ*$4) pt 2 ps 0.7 t "f = 10 kHz" with yerrorbars


#fehler: nur y, dämpfung : vllt 1%, frequ: auch 1%