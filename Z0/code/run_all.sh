#!/usr/bin/env bash

if ! [ -d ../tmp ]; then
  mkdir ../tmp
fi

./analyseZ0 ../data/89gev.dat ../tmp/89gev.root 89.48
./analyseZ0 ../data/91gev.dat ../tmp/91gev.root 91.33
./analyseZ0 ../data/93gev.dat ../tmp/93gev.root 93.02
./analyseZ0 ../data/hadrons.dat ../tmp/hadrons.root 91.33
./analyseZ0 ../data/muons.dat ../tmp/muons.root 91.33