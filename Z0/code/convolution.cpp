#include <iostream>
#include <math.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TROOT.h>

using namespace std;

TF1* integrandfunc;

double bwfun(double s, double s0, double g2, double m2)
{
  return s0*(s*g2)/(pow(s-m2,2)+m2*g2);
}

double bwfun2(Double_t* x, Double_t* pars)
{
  return bwfun(pow(x[0],2),pars[0],pow(pars[1],2),pow(pars[2],2));
}

double radcorr(double z, double s)
{
  const double me2    = 2.611200e-7;    //electron mass
  const double alphpi = 2.3228196e-3;   //alpha/pi, alpha: fine struc constant	
  const double zeta2  = 1.644934e0;     //riemann zeta function: zeta(2)
  const double cons1  = -2.16487e0;
  const double cons2  = 9.8406e0;
  const double cons3  = -6.26e0;
  const double fac2   = 1.083e0;
  
  double l      = log(s/me2);
  double beta   = 2.*alphpi*(l-1.);
  double del1vs = alphpi*((3./2.)*l+2.*zeta2-2.);
  double del2vs = pow(alphpi,2.)*(cons1*pow(l,2.)+cons2*l+cons3);
  double delvs  = 1.+del1vs+del2vs;
  double del1h  = -alphpi*(1.+z)*(l-1.);
  double delbh  = pow(alphpi,2.)*(1./2.)*pow((l-1.),2.)
    *((1.+z)*(3.*log(z)-4.*log(1.-z))-(4./(1.-z))*log(z)-5.-z);
  double rad    = beta*pow((1.-z),beta-1.)*delvs+del1h+delbh;
  rad *=fac2;
  return rad;
}

double radcorr2(Double_t* x, Double_t* pars)
{	
                 //z   //s
  return radcorr(x[0], pars[0]);
}

double integrand(double z, double s, double s0, double g2, double m2)
{
  return bwfun(s*z,s0, g2,m2)*radcorr(z,s);
}

double integrand2(Double_t* x, Double_t* pars)
{
                   // z	 //s	  //s0	   //g2	    //m2	
  return integrand(x[0], pars[0], pars[1], pars[2], pars[3]);
}

double convolution(double s,double s0, double g2, double m2)
{
  if(s<60){
    cout << "s too small" << endl;
    return -1;
  }
  
  double zmin=pow(60,2)/s;
  double zmax=1-1.e-12; 		//almost one
  double pars[4]={s,s0,g2,m2};
  double result = integrandfunc->Integral(zmin,zmax,pars,1e-6);
  return result;
  
}

double convolution2(Double_t* x, Double_t* pars)
{			 
                     //sqrt(s)	  //s0	   //g	           //m
  return convolution(pow(x[0],2), pars[0], pow(pars[1],2), pow(pars[2],2));
}
