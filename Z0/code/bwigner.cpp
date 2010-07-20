#include "convolution.cpp"
#include <iostream>
#include <math.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TROOT.h>
#include <TObject.h>
#include <TMinuit.h>
#include <TPaveText.h>
#include <TLine.h>
#include <TText.h>
#include <TStyle.h>

using namespace std;

TGraphErrors* graph;
TGraph* graph2;
const int funcres=200;

void bwigner()
{
  // Hello
  cout << "********************************************"<< endl;
  cout << "*                 Z0 Res. Fit              *"<< endl;
  cout << "********************************************"<< endl;
  
  // Preparations
  integrandfunc= new TF1("integrandfunc",integrand2,0,1,4);
  TF1* convolutionfunc = new TF1("convolutionfunc",convolution2,60,150,3);
  convolutionfunc->SetNpx(funcres);
  convolutionfunc->SetLineColor(8);
  convolutionfunc->SetParNames("sigma0","gamma","mass");
  
  // Get # Points
  int npoints;
  cout << "Wie viele Datenpunkte?"<< endl;
  cin >> npoints;
  if(npoints<=0){
    cout << "Fehler: Zahl der Punkte muss groesser 0 sein" << endl;
    return;}
  
  // Create Storage for points & errors & get points
  double* x = new double[npoints];
  double* xe = new double[npoints];
  double* y = new double[npoints];
  double* ye = new double[npoints];
  
  for(int i=0;i<npoints;i++){
    cout << i+1 << ". Datenpunkt" << endl;
    cout << "\tx: "; cin >> x[i];
    cout << "\txfehler: "; cin >> xe[i]; cout << endl;
    cout << "\ty: "; cin >> y[i];
    cout << "\tyfehler: "; cin >> ye[i]; cout << endl;
  } 

  // Create graph from data and fit against fitfuncion
  TCanvas* canvas = new TCanvas("canvas","Breit-Wigner Fit fuer Z0-Resonanz",550,550);
//   canvas->SetGrid();
  canvas->SetLeftMargin(0.11);
  canvas->SetBottomMargin(0.1);
  canvas->SetRightMargin(0.06);
  canvas->SetTopMargin(0.03);
  canvas->cd(1);
  graph = new TGraphErrors(npoints,x,y,xe,ye);
  graph2 = new TGraph(npoints,x,y);

  bool qcustominit=0;
  cout << "Eigene Startbedingungen (Startwerte, Parameter festhalten)?";
  cout << " (ja: 1, nein: 0)"<< endl;
  cin >> qcustominit;
  if(qcustominit){
    for(int i=0;i<3;i++){
      double tmpd;
      double tmpb;

      cout << i+1 << ". Parameter (" << convolutionfunc->GetParName(i);
      cout << ")" << endl;
      cout << "========================" << endl;
      cout << "Startwert?" << endl;
      cin >> tmpd;
      convolutionfunc->SetParameter(i,tmpd);
      cout << "Parameter festhalten? (ja: 1, nein: 0)" << endl;
      cin >> tmpb;
      if(tmpb){
	convolutionfunc->FixParameter(i,tmpd);
      }
    }
  }
  else{
    convolutionfunc->SetParameters(40,5,90);
  }
  cout << "fitting ... please wait... " << endl;
  
  graph->Fit(convolutionfunc,"QV","", 70,150);
  
  // Store results in new array
  double results[3];
  double resultserr[3];
  for(int i=0;i<3;i++){
    results[i]=convolutionfunc->GetParameter(i);
    resultserr[i]=convolutionfunc->GetParError(i);
  }
  double chi2 = convolutionfunc->GetChisquare();

  // Printo out the fit result
  cout << "*******************************************"<< endl;
  cout << "*                FIT RESULTS              *"<< endl;
  cout << "*******************************************"<< endl;
  cout << "FIT PARAMETERS [GeV]:" << endl;
  cout << "------------------" << endl;
  cout << "SIGMA0:\t" << results[0] << "\t\tfehler: "<< resultserr[0] << endl;
  cout << "GAMMA :\t" << results[1] << "\t\tfehler: "<< resultserr[1] << endl;
  cout << "MASS  :\t" << results[2] << "\t\tfehler: "<< resultserr[2] << endl;
  cout << "\nfit statistics   :" << endl;
  cout << "------------------" << endl;
  cout << "CHI SQUARE:\t" << chi2 << endl;
  
  TF1* wigner = new TF1("wigner",bwfun2,80,100,3);
  wigner->SetParameters(results[0],results[1],results[2]);
  wigner->SetNpx(funcres);
  wigner->GetXaxis()->SetTitle("center of mass energy [GeV]");
  wigner->GetYaxis()->SetTitle("cross section [nb]");
  
  wigner->Draw("C");
  convolutionfunc->Draw("same");
  graph->Draw("P");
  graph2->Draw("P");

  // Create the legend
  TPaveText* legend = new TPaveText(0.63,0.85,0.92,0.75, "NDCbr");
  TLine* black = legend->AddLine(.05,.73,.15,.73);
  black->SetLineWidth(3);
  black->SetLineColor(1);
  TText* twig = legend->AddText("Breit-Wigner");
  twig->SetTextSize(.03);
  
  TLine* green = legend->AddLine(.05,.2,.15,.2);
  green->SetLineWidth(3);
  green->SetLineColor(8);
  TText* tqed = legend->AddText("with QED effects");
  tqed->SetTextSize(.03);
  legend->Draw("");

  // Make the plots
  gPad->Update();
  gPad->SaveAs("../tmp/fit.pdf");
  bool qcontours;
  cout << "Konturen zeichnen (1/0)? (nur wenn keine parameter festgehalten!)" << endl;
  cin >> qcontours;
  if(!qcontours || qcustominit){
    return;
  } 
    
  TCanvas* contcan = new TCanvas("contcan","Contours (Parameter Correlation)");
  contcan->Divide(2,2);
  contcan->cd(2);
  gPad->Delete();
  
  TGraph* contour;
  char lsg[] = "gamma-sigma";
  char asgx[] = "Zerfallsbreite [GeV]";
  char asgy[] = "Wirkungsquerschnitt [nb]";
  
  char lsm[] = "mass-sigma";
  char asmx[] = "Z0 Masse [GeV]";
  char asmy[] = "Wirkungsquerschnitt [nb]";
  
  char lgm[] = "mass-gamma";
  char agmx[] = "Z0 Masse [GeV]";
  char agmy[] = "Zerfallsbreite [GeV]";
  
  char* larray[5]={0,lsg,0,lsm,lgm};
  char* aarray[5][2]={{},{asgx,asgy},{},{asmx,asmy},{agmx,agmy},};
  
  for(int i=0; i<3;i++){
    for(int j=0;j<i;j++){
      if(i!=j){
	int current=(i-1)*2+(j+1);
	contcan->cd(current);
	for(int k=3;k>0;k--){
	  gMinuit->SetErrorDef(k*k);
	  contour =(TGraph*)gMinuit->Contour(10,i,j);
	  contour->SetTitle(larray[current]);
	  if(k==3){
	    contour->Draw("ac*");
	    contour->GetXaxis()->SetTitle(aarray[current][0]);
	    contour->GetYaxis()->SetTitle(aarray[current][1]);
	  }
	  else{
	    contour->Draw("c*");
	  }
	}
      }
    }
  }
  contcan->cd();
  gPad->SaveAs("../tmp/corr.pdf");
  return;
}

int main(){
  TStyle *atlasStyle= new TStyle("ATLAS","Atlas style");
  // use plain black on white colors
  Int_t icol=0;
  atlasStyle->SetFrameBorderMode(icol);               //  }
  atlasStyle->SetCanvasBorderMode(icol);              //   \
  atlasStyle->SetPadBorderMode(icol);             //    \
  atlasStyle->SetPadColor(icol);                  //         -- alles auf schwarz gesetzt (0=schwarz)
  atlasStyle->SetCanvasColor(icol);               //    /
  atlasStyle->SetStatColor(icol);                 //   /
  atlasStyle->SetFrameFillColor(icol);                //  }
  // atlasStyle->SetFillColor(icol);


  const Int_t NRGBs = 5;
  const Int_t NCont = 255;
      Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
      Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
      Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
      Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };

  // TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);     //geht leider nicht... in CINT wirds nicht vorgeschlagen
  // gStyle->SetNumberContours(NCont);
  atlasStyle->SetNumberContours(NCont);

  // // set the paper & margin sizes
  // atlasStyle->SetPaperSize(20,26);
  // atlasStyle->SetPadTopMargin(0.05);
  // atlasStyle->SetPadRightMargin(0.05);
  // atlasStyle->SetPadBottomMargin(0.16);
  // atlasStyle->SetPadLeftMargin(0.12);

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
  atlasStyle->SetMarkerSize(1.0); // standard was 1.2
  atlasStyle->SetHistLineWidth(2);
  atlasStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  //get rid of X error bars and y error bar caps
  atlasStyle->SetErrorX(0.001);

  //do not display any of the standard histogram decorations
  // atlasStyle->SetOptTitle(0);
//   atlasStyle->SetOptStat(1111);
//   atlasStyle->SetOptFit(1111);
  // atlasStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  atlasStyle->SetPadTickX(1);
  atlasStyle->SetPadTickY(1);

  gROOT->SetStyle("ATLAS");

  gStyle->SetPalette(1);

  bwigner();
  return 0;
}
