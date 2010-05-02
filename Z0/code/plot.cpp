{
//
//  M. zur Nedden, 2007-02-02
//
//  Example file who to make plolts in root
//
//  start this macro in root with   .x  plot.C
//

// say hello
  printf("Example for plots in root plot.C\n");
   
// some general options 
  gStyle->SetOptStat(0);
  
// open frame, white color
  canvas1 = new TCanvas("canvas1", "test plot");
  canvas1->SetFillColor(10);

// define input root file with Monte Carlo
  file_MC = new TFile("hadrons.root");

// plot MC histogram, after normalization, with some options
  mu_px->SetNormFactor(1.);
  mu_px->SetLineColor(2);
  mu_px->SetLineWidth(2);
  mu_px->Draw(); 
  
// define input root file with data 
  file_data = new TFile("91gev.root");

// plot data histogram, after normalization, with some options
// plot on the same than MC
  mu_px->SetNormFactor(1.);
  mu_px->SetMarkerStyle(22);
  mu_px->SetMarkerSize(1.0);
  mu_px->SetMarkerColor(4);
  mu_px->Draw("P,SAME"); 

// add some text
  texty = new TLatex(-58.,0.20,"number of muons");
  texty->SetTextAngle(90.); 
  texty->Draw();
  
  textx = new TLatex(32.3,-0.04,"x / GeV");
  textx->Draw();
  
  text1 = new TLatex(11.2,0.35,"L3");
  text1->SetTextColor(2);
  text1->Draw();
  
  text2 = new TLatex(12.6,0.2,"Monte Carlo");
  text2->SetTextColor(3);
  text2->Draw();

// print plot as jpg file
  canvas1->Print("plot.jpg");
}
