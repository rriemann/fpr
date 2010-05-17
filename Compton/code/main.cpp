
#include <iostream>
#include <string>
#include <math.h>
#include <fstream>
#include <assert.h>

#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "TVector3.h"

#include "hn2hist.cpp"

using std::cout;
using std::endl;
using std::vector;
using std::string;

int main ( int argc, char *argv[] ) {

    Double_t oeffnungszeit;

    TH1F* th1f_hintergrund = new TH1F;
    th1f_hintergrund = chn2hist("../data/rauschen.chn",oeffnungszeit);
    th1f_hintergrund->Scale(1/oeffnungszeit);

}


// kate: indent-mode cstyle; space-indent on; indent-width 4; replace-trailing-space-save on;
