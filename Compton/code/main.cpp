
#include <iostream>
#include <string>
#include <math.h>
#include <fstream>
#include <assert.h>

#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TStyle.h"
#include "TColor.h"
#include "TFitResult.h"
#include "TGraphErrors.h"

#include "hn2hist.cpp"

using std::cout;
using std::endl;
using std::vector;
using std::string;
void define_style();
void DrawOnCanvas(TObject* h1, string name, string options = "", bool log = false, string xaxis = "", string yaxis = "");
TFitResultPtr FitGausInRange(TH1* h1, Double_t x1 ,Double_t x2);
TH1F* ApplyCalibration(TF1* f1, TH1F* h1_orig);

int main ( int argc, char *argv[] ) {

    define_style();

    ////////////////////////////////////////////////////////////////////////////
    //////// 4.3 Energiekalibration
    ////////////////////////////////////////////////////////////////////////////

    Double_t oeffnungszeit;
    TFile* file = new TFile("../tmp/out.root","RECREATE");
    const short rebin = 10;

    TFitResultPtr tfitres_array_eichung[3]; // ba, na, cs - fit-parameter-gruppen
    Double_t dbl_array_proben_energie[] = {0.356, 0.511, 0.662}; // ba, na, cs; in MeV
    Double_t dbl_array_proben_energie_rel_fehler[] = {0.05, 0.04, 0.04};

    // http://de.wikipedia.org/wiki/Halbwertsbreite
    const Double_t fwhm_faktor = 2.0*sqrt(2.0*log(2));

    TH1F* th1f_hintergrund = new TH1F;
    th1f_hintergrund = chn2hist("../data/rauschen2.chn",oeffnungszeit);
    th1f_hintergrund->Rebin(rebin);
    th1f_hintergrund->Scale(1/oeffnungszeit);
    DrawOnCanvas(th1f_hintergrund, "hintergrund");

    TH1F* th1f_barium = new TH1F;
    th1f_barium = chn2hist("../data/ba.chn",oeffnungszeit);
    th1f_barium->Rebin(rebin);
    th1f_barium->Scale(1/oeffnungszeit);
    DrawOnCanvas(th1f_barium,"barium");
    TH1F* th1f_barium_ohne_hintergrund = new TH1F(*th1f_barium);
    th1f_barium_ohne_hintergrund->SetName("barium_ohne_hintergrund");
    th1f_barium_ohne_hintergrund->Add(th1f_hintergrund,-1);
    TFitResultPtr tfitres_barium = FitGausInRange(th1f_barium_ohne_hintergrund, 500, 600);
    DrawOnCanvas(th1f_barium_ohne_hintergrund,"barium_ohne_hintergrund");
    tfitres_array_eichung[0] = tfitres_barium;

    TH1F* th1f_natrium = new TH1F;
    th1f_natrium = chn2hist("../data/na.chn",oeffnungszeit);
    th1f_natrium->Rebin(rebin);
    th1f_natrium->Scale(1/oeffnungszeit);
    DrawOnCanvas(th1f_natrium,"natrium");
    TH1F* th1f_natrium_ohne_hintergrund = new TH1F(*th1f_natrium);
    th1f_natrium_ohne_hintergrund->SetName("natrium_ohne_hintergrund");
    th1f_natrium_ohne_hintergrund->Add(th1f_hintergrund,-1);
    TFitResultPtr tfitres_natrium = FitGausInRange(th1f_natrium_ohne_hintergrund, 700, 850);
    DrawOnCanvas(th1f_natrium_ohne_hintergrund,"natrium_ohne_hintergrund");
    tfitres_array_eichung[1] = tfitres_natrium;

    TH1F* th1f_caesium = new TH1F;
    th1f_caesium = chn2hist("../data/cs.chn",oeffnungszeit);
    th1f_caesium->Rebin(rebin);
    th1f_caesium->Scale(1/oeffnungszeit);
    DrawOnCanvas(th1f_caesium,"caesium");
    TH1F* th1f_caesium_ohne_hintergrund = new TH1F(*th1f_caesium);
    th1f_caesium_ohne_hintergrund->SetName("caesium_ohne_hintergrund");
    th1f_caesium_ohne_hintergrund->Add(th1f_hintergrund,-1);
    TFitResultPtr tfitres_caesium = FitGausInRange(th1f_caesium_ohne_hintergrund, 850,1125);
    DrawOnCanvas(th1f_caesium_ohne_hintergrund,"caesium_ohne_hintergrund");
    tfitres_array_eichung[2] = tfitres_caesium;

    TH1F* th1f_caesium_alu = new TH1F;
    th1f_caesium_alu = chn2hist("../data/cs_alu.chn",oeffnungszeit);
    th1f_caesium_alu->Rebin(rebin);
    th1f_caesium_alu->Scale(1/oeffnungszeit);
    DrawOnCanvas(th1f_caesium_alu,"caesium_alu");
    TH1F* th1f_caesium_alu_ohne_hintergrund = new TH1F(*th1f_caesium_alu);
    th1f_caesium_alu_ohne_hintergrund->SetName("caesium_alu_ohne_hintergrund");
    th1f_caesium_alu_ohne_hintergrund->Add(th1f_hintergrund,-1);
    TFitResultPtr tfitres_alu_caesium = FitGausInRange(th1f_caesium_alu_ohne_hintergrund, 850,1125);
    DrawOnCanvas(th1f_caesium_alu_ohne_hintergrund,"caesium_alu_ohne_hintergrund");

    TGraphErrors* tgrapherrors_energie_eichung_fit = new TGraphErrors(3);
    tgrapherrors_energie_eichung_fit->SetMarkerSize(1.0);
    tgrapherrors_energie_eichung_fit->SetMarkerStyle(20);
    for(unsigned short i=0; i < 3; ++i) {
        // Value 1: mean, Value 2: sigma
        tgrapherrors_energie_eichung_fit->SetPoint(i,tfitres_array_eichung[i]->Value(1),dbl_array_proben_energie[i]);
        tgrapherrors_energie_eichung_fit->SetPointError(i,tfitres_array_eichung[i]->Value(2),dbl_array_proben_energie_rel_fehler[i]);
    }
    TF1 *tf1_linear_eichung = new TF1("f1_linere_eichung","pol1",0,3000);
    TFitResultPtr tfitres_eichung = tgrapherrors_energie_eichung_fit->Fit(tf1_linear_eichung,"S");
    DrawOnCanvas(tgrapherrors_energie_eichung_fit,"eichung","AP",false,"Kanal","Energie [MeV]");

    TH1F* th1f_caesium_ohne_hintergrund_energie = ApplyCalibration(tf1_linear_eichung,th1f_caesium_ohne_hintergrund);
    th1f_caesium_ohne_hintergrund_energie->SetName("caesium_ohne_hintergrund_energie");
    DrawOnCanvas(th1f_caesium_ohne_hintergrund_energie,"caesium_ohne_hintergrund_energie","",false,"Energie [MeV]","Anzahl");

    TH1F* th1f_caesium_alu_energie = ApplyCalibration(tf1_linear_eichung,th1f_caesium_alu);
    th1f_caesium_alu_energie->SetName("caesium_alu_energie");
    DrawOnCanvas(th1f_caesium_alu_energie,"caesium_alu_energie","",false,"Energie [MeV]","Anzahl");

    TH1F* th1f_caesium_alu_ohne_hintergrund_energie = ApplyCalibration(tf1_linear_eichung,th1f_caesium_alu_ohne_hintergrund);
    th1f_caesium_alu_ohne_hintergrund_energie->SetName("caesium_alu_ohne_hintergrund_energie");
    DrawOnCanvas(th1f_caesium_alu_ohne_hintergrund_energie,"caesium_alu_ohne_hintergrund_energie","",false,"Energie [MeV]","Anzahl");

    TGraphErrors* tgrapherrors_energie_aufloesung = new TGraphErrors(3);
    tgrapherrors_energie_aufloesung->SetMarkerSize(1.0);
    tgrapherrors_energie_aufloesung->SetMarkerStyle(20);
    for(unsigned short i=0; i < 3; ++i) {
        // Value 1: mean, Value 2: sigma
        Double_t kanal_nach_energie = tfitres_eichung->Value(1);
        Double_t energie = tf1_linear_eichung->Eval(tfitres_array_eichung[i]->Value(1));
        Double_t sigma = tfitres_array_eichung[i]->Value(2)*kanal_nach_energie;
        Double_t rel_energie_aufloesung = sigma*fwhm_faktor/energie;
        tgrapherrors_energie_aufloesung->SetPoint(i,energie,rel_energie_aufloesung);

        Double_t kanal_nach_energie_fehler = tfitres_eichung->Errors()[1];
        Double_t sigma_fehler = sqrt(
                pow(tfitres_array_eichung[i]->Errors()[2]*kanal_nach_energie,2) + 
                pow(tfitres_array_eichung[i]->Value(2)*kanal_nach_energie_fehler,2));
        Double_t energie_fehler = sigma;
        Double_t rel_energie_fehler = fwhm_faktor*sqrt(
            pow(1/energie*sigma_fehler,2) +
            pow(1*sigma/(energie*energie)*energie_fehler,2)
        );
        tgrapherrors_energie_aufloesung->SetPointError(i,energie_fehler,rel_energie_fehler);
    }
    TF1 *tf1_energie_aufloesung = new TF1("f1", "[0]/(x*[1]+1)",0,1);
    tf1_energie_aufloesung->SetParameters(1,1,1);
    TFitResultPtr tfitres_rel_aufloesung = tgrapherrors_energie_aufloesung->Fit(tf1_energie_aufloesung,"S");
    DrawOnCanvas(tgrapherrors_energie_aufloesung,"rel_aufloesung","AP",false,"Energie [MeV]","relativ. Energieaufloesung");

    ////////////////////////////////////////////////////////////////////////////
    //////// 4.4 Cs Spektrum
    ////////////////////////////////////////////////////////////////////////////

    TH1F* th1f_caesium_energie = ApplyCalibration(tf1_linear_eichung,th1f_caesium);
    th1f_caesium_energie->SetName("caesium_energie");
    DrawOnCanvas(th1f_caesium_energie,"caesium_energie","",false,"Energie [MeV]","Anzahl");

    ////////////////////////////////////////////////////////////////////////////
    //////// 4.5 Cs Spektrum 2
    ////////////////////////////////////////////////////////////////////////////

    TH1F* th1f_caesium2 = new TH1F(*th1f_caesium);
    TF1 *tf1_caesium2 = new TF1("user", "[0]*exp(-0.5*((x-[1])/[2])^2) + [3]*exp(-0.5*((x-[4])/[5])^2) + [6]*1/(1 + exp((x-[7])/[8])^2)", 0, 1300);
    tf1_caesium2->SetParameters(0.5, 600, 200, 1.4, 1000, 160, 7., 200, 100);
    tf1_caesium2->SetLineColor(2);
    TFitResultPtr tfitres_caesium2 = th1f_caesium2->Fit(tf1_caesium2, "RS");
    DrawOnCanvas(th1f_caesium2,"caesium_fit_untergrund","",false,"Kanal","Anzahl");

    ////////////////////////////////////////////////////////////////////////////
    //////// Klein-Nishina-Formel
    ////////////////////////////////////////////////////////////////////////////

    Double_t me = 0.51099892; // Elektronen-Masse
//     TF1* tf1_klein_nishina = new TF1("klein_nishina","1000*2*pi*0.08*((1+x)/(x*x)*((2*(1+x)/(1+2*x)-1/x*log(1+2*x))))+1/(2*x)*log(1+2*x)-(1+3*x)/pow(1+2*x,2)",0.01/me,100/me); // in barn
    TF1* tf1_klein_nishina = new TF1("klein_nishina","1000*2*pi*0.08*((1+x)/(x*x)*((2*(1+x)/(1+2*x)-1/x*log(1+2*x)))+1/(2*x)*log(1+2*x)-(1+3*x)/pow(1+2*x,2))",0.01/me,100/me);
    tf1_klein_nishina->GetXaxis()->SetTitle("\\gamma");
    tf1_klein_nishina->GetYaxis()->SetTitle("totaler Wirkungsquerschnitt \\sigma [mb]");

    TGraphErrors* tgrapherrors_klein_nishina_value = new TGraphErrors(1);
    tgrapherrors_klein_nishina_value->SetPoint(0, 0.662/me,260);
    tgrapherrors_klein_nishina_value->SetPointError(0,0.662*0.04/me*2,7*2);
    tgrapherrors_klein_nishina_value->SetMarkerColor(2);
    cout << "Klein-Nishina: " << tf1_klein_nishina->Eval(0.662/me) << endl;

    TCanvas *c1 = new TCanvas("c1","c1",600,600);
    c1->SetGrid();
    c1->SetLeftMargin(0.15);
    c1->SetBottomMargin(0.15);
    c1->SetRightMargin(0.15);
    c1->SetLogy(1);
    c1->SetLogx(1);
    tf1_klein_nishina->Draw();
    tgrapherrors_klein_nishina_value->Draw("same,P"); //,A
    c1->SaveAs("../tmp/klein_nishina.pdf");
    
    file->Write();
}

void DrawOnCanvas(TObject* obj, string name, string options, bool log, string xaxis, string yaxis) {
    TCanvas *c1 = new TCanvas("c1","c1",600,600);
    c1->SetGrid();
    c1->SetLeftMargin(0.15);
    c1->SetBottomMargin(0.15);
    c1->SetRightMargin(0.15);
    if (log) {
        c1->SetLogy(1);
        c1->SetLogx(1);
    }
    if(obj->InheritsFrom("TH1") || obj->InheritsFrom("TGraph")) {
        if(obj->InheritsFrom("TH1")) {
            TH1* o = (TH1*)obj;
            if (xaxis != "") o->GetXaxis()->SetTitle(xaxis.c_str());
            else o->GetXaxis()->SetTitle("Kanal");
            if (yaxis != "") o->GetYaxis()->SetTitle(yaxis.c_str());
            else o->GetYaxis()->SetTitle("Anzahl");
            o->GetYaxis()->SetTitleOffset(1.4);
            o->Draw(options.c_str());
        } else {
            TGraph* o = (TGraph*)obj;
            if (xaxis != "") o->GetXaxis()->SetTitle(xaxis.c_str());
            if (yaxis != "") o->GetYaxis()->SetTitle(yaxis.c_str());
            o->GetYaxis()->SetTitleOffset(1.4);
            o->Draw(options.c_str());
        }
    } else {
        obj->Draw(options.c_str());
    }
    c1->SaveAs(Form("../tmp/%s.pdf", name.c_str()));
    delete c1;
}

TFitResultPtr FitGausInRange(TH1* h1, Double_t x1 ,Double_t x2) {
    TF1 *f1 = new TF1("f1", "gaus", x1, x2);
    f1->SetLineColor(2);
    return h1->Fit(f1, "RS");
}

TFitResultPtr FitCsUndUntergrund(TH1* h1, Double_t x1 ,Double_t x2) {
    TF1 *f1 = new TF1("user", "[0]*exp(-0.5*((x-[1])/[2])^2) + [3]*exp(-0.5*((x-[4])/[5])^2) + [6]*1/(1 + exp((x-[7])/[8])^2)", x1, x2);
    f1->SetParameters(3.9, 200, 200, 0.5, 600, 60, 1.4, 1000, 160);
    f1->SetLineColor(2);
    return h1->Fit(f1, "RS");
}

TH1F* ApplyCalibration(TF1* f1, TH1F* h1_orig) {
    TH1F* h1 = new TH1F(*h1_orig);
    h1->SetStats(false);
    Int_t bin_count = h1->GetNbinsX();
    Double_t x1 = f1->Eval(h1->GetBinCenter(1));
    Double_t x2 = f1->Eval(h1->GetBinCenter(bin_count));
    h1->GetXaxis()->Set(bin_count,x1,x2);
    return h1;
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
    atlasStyle->SetPaperSize(20,20);
    atlasStyle->SetPadTopMargin(0.05);
    atlasStyle->SetPadRightMargin(0.05);
    atlasStyle->SetPadBottomMargin(0.0);
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
    atlasStyle->SetMarkerSize(0.7);
    // atlasStyle->SetHistLineWidth(2.);
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

// kate: indent-mode cstyle; space-indent on; indent-width 4;
