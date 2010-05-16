#include <iostream>
#include <fstream>
#include <assert.h>

//root includes
#include <TH1F.h>

/* chn2hist.C: Ulli Schwanke
   converts a chn-file to a ROOT TH1F, a typical root session might look like this:

  root [0] .L chn2hist.C+
  Info in <TUnixSystem::ACLiC>: creating shared library /data/ehs/schwanke/compton/analysis/./chn2hist_C.so
  root [1] Double_t live = 0;
  root [2] TH1F* myhist = chn2hist("co57on.Chn",live)
  Read histogram "hco57on" from file "co57on.Chn".
   livetime = 23.24 s

*/   

typedef short int mWORD; //2 bytes
typedef long mLONG;      //4 bytes

struct ChnFormat {
  mWORD code;      //Must be -1
  mWORD mca;       //MCA number
  mWORD segment;   //segment number
  mWORD start; //ASCII seconds of start time
  mLONG realtime; //Realtime (increments of 20 ms)
  mLONG livetime; //Livetime (increments of 20 ms)
  char startdate[8]; //Start date as ASCII DDMMMYY0, or binary zero if not known
  char starttime[4]; //Start time as ASCII HHMM, or binary zero if not known 
  mWORD offset;       //Channel offset of data
  mWORD channels;     //number of channels (length of data)
  mLONG data[2];
};

struct ChnFormat* ReadFile(const char* filename){
  std::ifstream in(filename);
  if(!in){
    std::cout << "Cannot open " << filename << std::endl;
    return 0;
  }
  in.seekg(0,std::ios::end);
  int filelen = in.tellg();
  char* data = new char[filelen];
  in.seekg(0,std::ios::beg);
  in.read(data,filelen);
  in.close();
  return (struct ChnFormat*)data;
}

//Create TH1F from file. Return livetime (in seconds) in the 2nd argument
TH1F* chn2hist(const char* filename,Double_t& livetime){
  //check byte size
  assert(sizeof(mWORD)==2 && sizeof(mLONG)==4);
  //read file
  struct ChnFormat* data = ReadFile(filename);
  if( data==0 ) return 0;
  //create histogram name
  std::string hname(filename);
  std::string::size_type pos = hname.rfind('.');
  if( pos != std::string::npos ) hname = hname.substr(0,pos);
  pos = hname.rfind('/');
  if( pos != std::string::npos ) hname = hname.substr(pos+1);
  hname = "h" + hname;
  //book histogram and fill
  int nbins = data->channels;
  mLONG* array = &data->data[0];
  //std::cout << "nbins = " << nbins << std::endl;
  TH1F* h = new TH1F(hname.c_str(),filename,nbins,-0.5,-0.5+nbins);
  h->Sumw2();  
  for(int i=0;i<nbins;i++){    
    int c = array[i];
    for(int k=0;k<c;k++) h->Fill(i);
  }
  livetime = data->livetime*20e-3;
  std::cout << "Read histogram \"" << hname << "\" from file \"" << filename << "\"." << std::endl;
  std::cout << " livetime = " << livetime << " s" << std::endl;
  return h;
}
