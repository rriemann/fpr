
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
#include "TStyle.h"
#include "TColor.h"

#include "hn2hist.cpp"

using std::cout;
using std::endl;
using std::vector;
using std::string;
void define_style();

int main ( int argc, char *argv[] ) {

    define_style();

    Double_t oeffnungszeit;
    TFile* file = new TFile("../tmp/out.root","RECREATE");

    TH1F* th1f_hintergrund = new TH1F;
    th1f_hintergrund = chn2hist("../data/rauschen2.chn",oeffnungszeit);
    th1f_hintergrund->Scale(1/oeffnungszeit);

    TH1F* th1f_barium = new TH1F;
    th1f_barium = chn2hist("../data/ba.chn",oeffnungszeit);
    th1f_barium->Scale(1/oeffnungszeit);

    TH1F* th1f_caesium = new TH1F;
    th1f_caesium = chn2hist("../data/cs.chn",oeffnungszeit);
    th1f_caesium->Scale(1/oeffnungszeit);

    TH1F* th1f_natrium = new TH1F;
    th1f_natrium = chn2hist("../data/na.chn",oeffnungszeit);
    th1f_natrium->Scale(1/oeffnungszeit);

    TH1F* th1f_caesium_alu = new TH1F;
    th1f_caesium_alu = chn2hist("../data/cs_alu.chn",oeffnungszeit);
    th1f_caesium_alu->Scale(1/oeffnungszeit);

    file->Write();

}

void define_style() {
    TStyle *atlasStyle= new TStyle("ATLAS","Atlas style");
    // use plain black on white colors
    Int_t icol=0;
    atlasStyle->SetFrameBorderMode(icol);
    atlasStyle->SetCanvasBorderMode(icol);
    atlasStyle->SetPadBorderMode(icol);
    atlasStyle->SetPadColor(icol);
    atlasStyle->SetCanvasColor(icol);
    atlasStyle->SetStatColor(icol);
    atlasStyle->SetFrameFillColor(icol);
    // atlasStyle->SetFillColor(icol);

    const Int_t NRGBs = 5;
    const Int_t NCont = 255;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };

    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
    atlasStyle->SetNumberContours(NCont);

    // set the paper & margin sizes
    atlasStyle->SetPaperSize(20,26);
    atlasStyle->SetPadTopMargin(0.05);
    atlasStyle->SetPadRightMargin(0.05);
    atlasStyle->SetPadBottomMargin(0.16);
    atlasStyle->SetPadLeftMargin(0.12);

    // use large fonts
    //Int_t font=72;
    Int_t font=42;
    Double_t tsize=0.05;
    atlasStyle->SetTextFont(font);

    atlasStyle->SetTextSize(tsize);
    atlasStyle->SetLabelFont(font,"x");
    atlasStyle->SetTitleFont(font,"x");
    atlasStyle->SetLabelFont(font,"y");
    atlasStyle->SetTitleFont(font,"y");
    atlasStyle->SetLabelFont(font,"z");
    atlasStyle->SetTitleFont(font,"z");

    atlasStyle->SetLabelSize(tsize,"x");
    atlasStyle->SetTitleSize(tsize,"x");
    atlasStyle->SetLabelSize(tsize,"y");
    atlasStyle->SetTitleSize(tsize,"y");
    atlasStyle->SetLabelSize(tsize,"z");
    atlasStyle->SetTitleSize(tsize,"z");

    //use bold lines and markers
    atlasStyle->SetMarkerStyle(20);
    atlasStyle->SetMarkerSize(0.1);
    atlasStyle->SetHistLineWidth(1.);
    atlasStyle->SetLineStyleString(2,"[12 12]");

    //get rid of X error bars and y error bar caps
    //atlasStyle->SetErrorX(0.001);

    //do not display any of the standard histogram decorations
    atlasStyle->SetOptTitle(0);
    //atlasStyle->SetOptStat(1111);
    // atlasStyle->SetOptStat(0);
    atlasStyle->SetOptFit(1111);
    // atlasStyle->SetOptFit(0);

    // put tick marks on top and RHS of plots
    atlasStyle->SetPadTickX(1);
    atlasStyle->SetPadTickY(1);

    //gStyle->SetPadTickX(1);
    //gStyle->SetPadTickY(1);

    gROOT->SetStyle("ATLAS");
}

// void Diagram::DrawTH1(int index, string histname, string xaxis, string yaxis, string options, bool log) {
//
//   TH1* h1 = (TH1*) m_files[index]->Get(histname.c_str());
//   h1->SetMarkerStyle(20);                                                                       //ATLAS-Style
//   std::string folder = (string)m_files[index]->GetName();
//   folder = folder.replace(folder.find(".root"), 100, "").substr(19);
//
//   if (h1 != NULL) {
//   if (xaxis != "") {
//       h1->GetXaxis()->SetTitle(xaxis.c_str());
//     }
//     if (yaxis != "") {
//       h1->GetYaxis()->SetTitle(yaxis.c_str());
//     }
//     h1->SetTitle(Form("%s %s", m_names[index].c_str(), (histname).c_str()));
//     h1->Scale(1./h1->Integral() * h1->GetEntries());
//     TCanvas* c1 = GetCanvas(histname);
//     h1->Draw(options.c_str());
//     if (log) {
//       c1->SetLogy(1);
//       c1->SaveAs(Form("/afs/ifh.de/user/m/murach/scratch/data/plots/%s/%s_log.png", folder.c_str(), histname.c_str()));
//     }
//     else {
//       c1->SaveAs(Form("/afs/ifh.de/user/m/murach/scratch/data/plots/%s/%s.png", folder.c_str(), histname.c_str()));
//     }
//     delete c1;
//   }
//   else {
//     cout << "ERROR: Histogram " << histname << " in " << m_names[index] << " not found!" << endl;
//   }
//
// }

// kate: indent-mode cstyle; space-indent on; indent-width 0;  replace-trailing-space-save on;
