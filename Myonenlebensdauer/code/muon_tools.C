//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Utility functions for muon life-time measurement                         //
// ================================================                         //
// Author: O.M.Kind, May 2007                                               //
//                                                                          //
// mca2hist - conversion of ascii or binary MCA spectra to                  //
//            RooT histograms                                               //
//                                                                          //
// do_calibration - performs channel->time calibration of                   //
//                  histograms using a given calibration function           //
//                                                                          //
// channel2time - converts channel number to time value                     //
//                                                                          //
// get_calibration - obtain channel->time calibration function either       //
//                   from a given graph (created eg using a text file) or   //
//                   by automatic finding the peaks in a calibration        //
//                   spectrum and respective keyboard input                 //
//                                                                          //
// get_fitfunction - create function for fitting the muon spectrum          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include "TMath.h"

TH1F* mca2hist(const char* FileName, const char* hname = "h_mca") {
  //
  // Converts ascii or binary output data from Ortec's MCA
  // into a RooT histogram
  //
  // Input: - filename of the MCA file
  //        - histogram name (default = "h_mca")
  //
  // Return value: pointer to the created TH1F histogram object
  //
  TH1F *h_mca = 0;

  // Check input file
  if ( gSystem->AccessPathName(FileName, kFileExists) ) {
    cout << "Error: Input file does not exist!" << endl;
    return 0;
  }

  // Check file type
  if ( gSystem->Exec(Form("file -b %s | grep -i data > /dev/null",
			  FileName)) == 0 ) {
    //
    // Binary format
    //

    // Create temporary text file from binary data
    const char *tmp_file = Form("/tmp/mca%d.dat", Int_t(gRandom->Rndm()*1e7));
    gSystem->Exec(Form("mcadata %s %s", FileName, tmp_file));
    
    // Get no. of channels
    FILE *fp = gSystem->OpenPipe(Form("cat %s | wc -l", tmp_file), "r");
    TString line;
    line.Gets(fp);
    Int_t nch= atoi(line.Data());
    gSystem->ClosePipe(fp);
    
    // Get channel off-set
    FILE *fp = gSystem->OpenPipe(Form("cat %s | head -1 | awk '{print $1}'",
				      tmp_file), "r");
    TString line;
    line.Gets(fp);
    Int_t ch1= atoi(line.Data());
    gSystem->ClosePipe(fp);
    
    // Book and fill histogram
    h_mca = new TH1F(hname, Form("File %s", FileName), nch, ch1, ch1+nch-1);
    
    Int_t ch = 0; Float_t entry = 0.;
    ifstream mca_input(tmp_file);
    while ( !mca_input.eof() ) {
      mca_input >> ch >> entry;
      h_mca->Fill(1+ch-ch1, entry);
    }
    
    // Clean-up
    gSystem->Exec(Form("rm %s", tmp_file));
    
  } else if ( gSystem->Exec(Form("file -b %s | grep -i ascii > /dev/null",
				 FileName)) == 0 ) {
    //
    // ASCII format
    //
    
    // Create temporary text file from ascii data
    const char *tmp_file = Form("/tmp/mca%d.dat", Int_t(gRandom->Rndm()*1e7));
    gSystem->Exec(Form("cat %s | awk \'/  /{print $1}\' > %s",
		       FileName, tmp_file));
    
    // Get channel off-set
    FILE *fp = gSystem->OpenPipe(Form("cat %s | head -12 | tail -1 | awk '{print $1}'", FileName), "r");
    TString line;
    line.Gets(fp);
    Int_t ch1= atoi(line.Data());
    gSystem->ClosePipe(fp);
    
    // Get no. of channels
    FILE *fp = gSystem->OpenPipe(Form("cat %s | head -12 | tail -1 | awk '{print $2}'", FileName), "r");
    TString line;
    line.Gets(fp);
    Int_t ch2= atoi(line.Data());
    Int_t nch = ch2-ch1+1;
    gSystem->ClosePipe(fp);

    // Book and fill histogram
    h_mca = new TH1F(hname, Form("File %s", FileName), nch, ch1, ch2);

    Int_t ch = 0; Float_t entry = 0.;
    ifstream mca_input(tmp_file);
    while ( !mca_input.eof() ) {
      mca_input >> entry;
      h_mca->Fill(++ch, entry);
    }

    // Clean-up
    gSystem->Exec(Form("rm %s", tmp_file));

  } else {
    // File type not recognised
    cout << "Error: Unknown file type" << endl;
    return 0;
  }

  // Layout
  h_mca->SetXTitle("Channel");
  h_mca->SetYTitle("Number of Entries");
  h_mca->SetStats(0);

  return h_mca;
}

//____________________________________________________________________

TH1F* do_calibration(TH1F *h_mca, TF1 *f_calib) {
  //
  // Performs time calibration of a histogram
  // using the given calibration function
  //
  // Input: - Histogram to be calibrated
  //        - calibration function
  //
  // Return value: pointer to calibrated histogram
  //
  
  // Create time spectrum
  Int_t nbins = h_mca->GetNbinsX();
  TH1F *h_mca_calib = new TH1F(Form("%s_calib", h_mca->GetName()),
			       h_mca->GetTitle(),
			       nbins,
			       channel2time(h_mca->GetBinLowEdge(1), f_calib),
			       channel2time(h_mca->GetBinLowEdge(nbins)
					    + h_mca->GetBinWidth(nbins),
					    f_calib));
  for ( Int_t i = 0; i <= nbins+1; i++ ) {
    h_mca_calib->SetBinContent(i, h_mca->GetBinContent(i));
      h_mca_calib->SetBinError(i, h_mca->GetBinError(i));
  }
  h_mca_calib->SetXTitle("Time (#mus)");
  h_mca_calib->SetYTitle("Number of Entries");

  return h_mca_calib;
}

//____________________________________________________________________

Float_t channel2time(Float_t channel, TF1 *f_calib) {
  //
  // Convert channel number to time for given
  // calibration function
  //
  // Input: - channel number
  //        - calibration function
  //
  // Return value: time value
  //
  return f_calib->Eval(channel);
}

//____________________________________________________________________

TF1* get_calibration(TGraph *g, const char* formula = "[0]*x+[1]") {
  //
  // Get channel -> time calibration by fitting the given graph
  //
  // Input: - graph of calibration measurements (channel, time)
  //        - fit formula a la TF1 (default = mx+b)
  // Return value: fit function
  //

  // Fit graph
  TAxis *axis_x = g->GetHistogram()->GetXaxis();
  TF1 *f_calib = new TF1("f_calib", formula,
			 axis_x->GetXmin(), axis_x->GetXmax());
  f_calib->SetParName(0, "m");
  f_calib->SetParName(1, "b");
  g->Fit(f_calib, "", "");

  return f_calib;
}

//____________________________________________________________________

TF1* get_calibration(TH1F *h_time) {
  //
  // Get channel -> time calibration
  // The channel numbers are taken from the given histogram by
  // finding the position of sharp peaks within the histogram.
  // For every peak found the user has to give the corresponding
  // time value.
  //
  // Input: - MCA spectrum
  //
  // Return value: calibration function
  //
  static const Int_t kMaxPeaks = 100;
  
  // Create canvas
  TCanvas *c = new TCanvas("time_calib", "Time Calibration", 700, 700);
  c->Divide(1,2);
  c->cd(2);

  // Find peaks
  TSpectrum *spec = new TSpectrum(kMaxPeaks);
  Int_t npeaks = spec->Search(h_time);
  Float_t *xpeaks = spec->GetPositionX();
  Float_t *ypeaks = spec->GetPositionY();
  Int_t idx_p[kMaxPeaks];
  TMath::Sort(npeaks, xpeaks, idx_p, kFALSE);

  // Draw histogram
  TPolyMarker *pm = ((TPolyMarker*)h_time->GetListOfFunctions())
    ->FindObject("TPolyMarker");
  pm->SetMarkerColor(kBlue);
  h_time->Draw();
  gPad->SetLogy(1);
  
  // Create graph
  c->cd(1);
//  TGraph *g = new TGraph(npeaks);
  TGraph *g = new TGraph(0);
  TH1F *h_axis = gPad->DrawFrame(0., 0., 2050., 20.);
//  g->SetHistogram(h_axis);
  g->SetMarkerColor(kMagenta);
  g->SetMarkerStyle(29);
  g->SetMarkerSize(1.2);
  TAxis *axis_x = h_axis->GetXaxis();
  axis_x->SetTitle("Channel");
  TAxis *axis_y = h_axis->GetYaxis();
  axis_y->SetTitle("Time (#mus)");
  axis_y->SetNdivisions(505);
  h_axis->SetTitle("Time Calibration");
  h_axis->Draw();
  g->SetTitle("Time Calibration");
  g->Draw("P");
  
  // Loop over all peaks and fill graph
  const Int_t bufsize = 2048;
  char buf[bufsize];
  buf[0] = '\0';
  cout << endl;
  for (Int_t i = 0; i < npeaks; i++ ) {
    Float_t xp = xpeaks[idx_p[i]];
    Float_t yp = ypeaks[idx_p[i]];
    TMarker *marker = new TMarker(xp, yp, pm->GetMarkerStyle());
    marker->SetMarkerColor(kRed);
    marker->SetMarkerSize(pm->GetMarkerSize()*1.1);
    c->cd(2);
    marker->Draw(); gPad->Modified(); gPad->Update();
    Int_t ch = h_time->FindBin(xpeaks[idx_p[i]]);
    cout << "Peak "; cout.width(2); cout << i+1;
    cout << ", Channel ";
    cout.width(4); cout<< ch << " : Time [mus] = ";
    cin.clear();
    cin.getline(buf, bufsize);
    Float_t t = atof(buf);
    g->SetPoint(i, ch, t);
    c->cd(1);
    gPad->Modified(); gPad->Update();
    delete marker;
  }

  // Fit graph
  return get_calibration(g);
}

//____________________________________________________________________

TF1* get_fitfunction(const char* name = "user") {
  //
  // Create function for fitting the muon spectrum a la script
  // and add it to the list of functions in RooT's memory
  //
  // Input: function name
  //
  // Return value: fit function
  //
  TF1 *f_mu = new TF1(name,
		      "[0]*exp(-x/[1])*(1.+1./[2]*exp(-x/[3]))+[4]",
		      0, 20);
  f_mu->SetParName(0, "N_{#mu}");
  f_mu->SetParName(1, "#tau_{#mu}");
  f_mu->SetParName(2, "N_{#mu^{+}} / N_{#mu^{-}}");
  f_mu->SetParName(3, "#tau_{C}");
  f_mu->SetParName(4, "N_{bkg}");
  //  f_mu->SetParameter(0, h_mca_calib->GetMaximum());
  f_mu->SetParameter(0, 20.);
  f_mu->SetParameter(1, 2.);
  f_mu->FixParameter(2, 1.270);
  f_mu->FixParameter(3, 2.1);
  f_mu->SetParameter(4, 25.);
  gStyle->SetOptFit(1112);
  gROOT->GetListOfFunctions()->Add(f_mu);
  
  return f_mu;
}

