
//geeignete cuts: muon_px sieht für hadron.dat und muon.dat anders aus -> hadron: |mupx| < 10gev; hadrons_n > 20, muon_n < 10; hadrons: muon_pt < 10gev, muons: u.a. pt > 20gev --> kann man rausschneiden
//Evis ist auch unterschiedlich -> nutzen

//oft: keine exakte aussage --> cutflow zum verbessern

//Reaktionen: Z -> mu mubar oder qqbar

//
// ///////////////////////////////////////////////////////////////////////////
//
// >>> analyse.cpp <<<
//
// Beispielprogramm zum Einlesen von Daten und Fuellen eines Histogramms
//
// Erlaeuterungen zu diesem Programm:
//
//   - Class cevent dient zur Speicherung eines e+e- - Kollisions-
//       ereignisses und zur Extraktion der Impulse und Massen der
//       zugehoerigen Teilchen. Bitte nicht veraendern!
//
//   - Die am Ende angehaengte subroutine read_event zum Einlesen
//       eines Ereignisses kann als black box betrachtet werden.
//
//   - Der Histogramm-file kann mit dem (graphischen) Analyseprogramm
//       ROOT eingelesen und weiterverarbeitet werden
//       siehe dazu das ROOT-Beispiel-Script plot.C
//
//   - Namensgebung fuer loop-Variable/Indices:
//      n...  = event counter
//      k...  = "particle" counter
//      l     = momentum component x, y, z
//
// ///////////////////////////////////////////////////////////////////////////
//

// general header files:
#include <iostream>
#include <string>
#include <math.h>
#include <fstream>

// needed for ROOT routines:
#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TLorentzVector.h"

using std::cout;
using std::endl;
using std::ios;
using std::vector;
using std::string;

//
///////////////////////////////////////////////////////////////////////////
//
// class cevent enthaelt e+e- Daten (jeweils ein Ereignis):
//       ======
//
//  _ktotce = Zahl der Teilchen/Kalorimetertreffer ("particle") im Detektor
//
//   Fuer jedes Teilchen/Treffer k (1 ... _ktotce) ist gespeichert:
//
//  _pvecce(k,1) = Impulskomponente in x-Richtung (in LEP-Ebene)
//  _pvecce(k,2) = Impulskomponente in y-Richtung (senkrecht zu LEP-Ebene)
//  _pvecce(k,3) = Impulskomponente in z-Richtung (entlang e- Richtung)
//  _massce(k) = Masse
//  _charge(k) = Ladung (+- 1 fuer Myonen, sonst 0!)
//
//   Massen: alle Massen sind hypotetisch
//
//   Im Myondetektor rekonstruierte Teilen haben die Masse 0.106 GeV, wenn sie
//   durch die Particle ID als solche identifiziert wurden.
//     Diese Masse wird zwar hier nicht gemessen, wir aber als bekannt vorrausgestetz
//     und kann damit den Myonen zugeordnet werden.
//
//   Bei Kalorimetertreffern gibt es keine eindeutige Zuordnung
//     zwischen Treffer und Teilchen. Man kann insbesondere das Teilchen
//     weder identifizieren noch seine Ladung messen. Da aber die meisten
//     Teilchen in Jets Pionen sind, wird den Kalorimetertreffern
//     (etwas willkuerlich) die Masse 0.14 GeV zugeordnet.
//
//   Manchmal kann man Kalorimeterhits als elektromagnetischen Ursprungs
//    (Photon, Elektron) erkennen. Dann wird _massce = 0 gesetzt.
//
//  Alle Einheiten: GeV
//
//  Fuer die Analyse duerfen nur die folgenden Funktionen benutzt werden:
//    print();               druckt Inhalt von cevent und mehr (Bildschirm)
//    number_particles();    Zahl der Teilchen im betrachteten Ereignis
//    momentum(k,l);         Impulskomponente l (x,y,z=1,2,3) von Teilchen k
//    mass(k);               Masse von Teilchen k
//    charge(k);             Ladung von Teilchen k (nur sinnvoll fuer Myonen)
//    Daraus koennen alle weitern Groessen, wie die Gesammtenergie, Gesammtimpuls,
//    invariante Masse zweiter Muonspuren etc.. berechnent werden


const int ktotmx=1000;      // maximal zulaessige Teilchenzahl pro Ereignis

class cevent {
private:
    int _ktotce;
    float _pvecce[ktotmx+1][4];
    float _massce[ktotmx+1];
    float _charge[ktotmx+1];
    bool _ready;
public:
    cevent();
    int setnce ( int );
    int setce ( int,float,float,float,float );
    int print();
    int number_particles();
    float momentum ( int,int );
    float mass ( int );
    float charge ( int );
    float GetEtot();
    float GetParticleEnergy(int k);
};


cevent::cevent() {                  // Konstruktor zum Erzeugen der Klasse cevent vom
    _ktotce = -1;
    _ready = 0;
}


int cevent::setnce ( int ktot )     //  zum Setzen von _ktotce
{                                   //  Danach muss alle Teilcheninformation
    int k;                            //  neu gefuellt werden!

    if ( ktot<0 ) {
        cout << "ERROR cevent::setnce: " << ktot << "negative" << endl;
        return -9;
    } else if ( ktot<ktotmx ) {
        _ktotce = ktot;
        for ( k=1; k<=_ktotce; k++ ) {
            _charge[k]=-9.;
        }
        return 0;
    } else {
        cout << "ERROR cevent::setnce: " << ktot << "too large" << endl;
        return -9;
    }
}


int cevent::setce ( int k, float px, float py, float pz, float m ) {
    int kk;

    if ( _ktotce < 1 ) {
        cout << "ERROR cevent::setce: _ktotce = " << _ktotce << endl;
        return -9;
    } else if ( k<=_ktotce ) {
        _pvecce[k][1] = px;
        _pvecce[k][2] = py;
        _pvecce[k][3] = pz;
        _massce[k] = fabs ( m );
        _charge[k] = 0;
        if ( _massce[k]>0 ) _charge[k] = m/fabs ( m );
        _ready = 1;
        for ( kk=1; kk<=_ktotce; kk++ ) {
            if ( _charge[kk]<-1. ) _ready = 1;
        }
        return 0;
    } else {
        cout << "ERROR cevent::setce: _ktotce > k = " << k << endl;
        return -8;
    }
}


float cevent::GetEtot() {
    int k;
    float p2, etote, etotxe;

    for ( k=1; k<=_ktotce; k++ ) {
        p2 = _pvecce[k][1]*_pvecce[k][1] +
             _pvecce[k][2]*_pvecce[k][2] +
             _pvecce[k][3]*_pvecce[k][3];
        etotxe = sqrt ( p2 + _massce[k]*_massce[k] );
        etote  = etote  + etotxe;
    }
    return etote;
}


int cevent::print()             // Zum ausdrucken der Eventinformation, wir fuer die ersten
{                               // fuenf Ereignisse aufgerufen
    int k;
    float p2, ptotxe, etotxe;
    float pxtote, pytote, pztote, etote;
    float ptote, mtote;

    if ( !_ready ) {
        cout << "ERROR cevent::print:  event not (fully) initialized "<< endl;
        return -9;
    }

    cout << endl
         << "Dump von cevent + weitere Groessen: "
         << endl << endl
         << "particle"
         << "  px/GeV "
         << "  py/GeV "
         << "  pz/GeV "
         << " mass/GeV"
         << " ptot/GeV"
         << " etot/GeV" << endl << endl;

    pxtote = 0.;
    pytote = 0.;
    pztote = 0.;
    etote = GetEtot();

    for ( k=1; k<=_ktotce; k++ ) {
        ptotxe = sqrt ( p2 );

        pxtote = pxtote + _pvecce[k][1];
        pytote = pytote + _pvecce[k][2];
        pztote = pztote + _pvecce[k][3];

        cout.width ( 5 );
        cout << k << "  ";
        cout.setf ( ios::fixed | ios::right );
        cout.precision ( 3 );
        cout.width ( 9 );
        cout << _pvecce[k][1];
        cout.width ( 9 );
        cout << _pvecce[k][2];
        cout.width ( 9 );
        cout << _pvecce[k][3];
        cout.width ( 9 );
        cout << _massce[k];
        cout.width ( 9 );
        cout << ptotxe;
        cout.width ( 9 );
        cout << etotxe << endl;
    }

    ptote = sqrt ( pxtote*pxtote+pytote*pytote+pztote*pztote );
    mtote = sqrt ( etote*etote-ptote*ptote );

    cout.width ( 7 );
    cout << endl << " TOTAL ";
    cout.width ( 9 );
    cout << pxtote;
    cout.width ( 9 );
    cout << pytote;
    cout.width ( 9 );
    cout << pztote;
    cout.width ( 9 );
    cout << mtote;
    cout.width ( 9 );
    cout << ptote;
    cout.width ( 9 );
    cout << etote << endl << endl;

    return 0;

}



int cevent::number_particles()   // Gibt die Zahl der Kalorimetertreffer (Teilchen)
{                                // im Event zurueck

    if ( !_ready ) {
        cout << "ERROR cevent::number_particles: "
             <<  "event not (fully) initialized "<< endl;
        return -9;
    }

    return _ktotce;

}


float cevent::momentum ( int k, int l )  // Liefert den Impuls des Teilchens k an
{                                     // der Position l (1=x, 2=y, 3=z)

    if ( !_ready ) {
        cout << "ERROR cevent::momentum:  event not (fully) initialized "<< endl;
        return -9;
    }

    return _pvecce[k][l];

}



float cevent::mass ( int k ) {     // Liefert die Masse des Teilchens k

    if ( !_ready ) {
        cout << "ERROR cevent::mass:  event not (fully) initialized "<< endl;
        return -9;
    }

    return _massce[k];

}

float cevent::GetParticleEnergy(int k){
    return sqrt( mass(k)*mass(k) + momentum(k,1)*momentum(k,1) + momentum(k,2)*momentum(k,2) + momentum(k,3)*momentum(k,3) );
}

float cevent::charge ( int k ) {  // Liefert die Ladung des Teilchens k

    if ( !_ready ) {
        cout << "ERROR cevent::charge:  event not (fully) initialized "<< endl;
        return -9;
    }

    return _charge[k];

}

///////////////////////////////////////////////////////////////////////////

string str_datfile;
int read_event ( cevent &event );

int main ( int argc, char *argv[] ) {

    int n;
    int k;

    int result;

    int ktot;

    float px_mu;

    char* hbooktitle;
    char* hropenname;
    char* hropenfile;
    char* hropenflag;

    int irecl, istat;
    int icycle;

    int nevmax;
    int nevent = 0;
    int hevent = 0;
    int muevent = 0;

    string str_hisfile = ( string ) argv[2];
    str_datfile = ( string ) argv[1];

    nevmax = 10000;
    
    const int BINS = 50;
    const float s = 91.33;                                          //TODO

    TFile *histofile = new TFile ( str_hisfile.c_str(),"RECREATE" );

    TH1F *hist_p_ges  = new TH1F ( "P_ges","p_{ges}",BINS,0.,30. );
    TH1F *hist_zmass = new TH1F("zmass", "Z-Masse", BINS, 0., 1.5);
    TH1F *hist_E_T = new TH1F("E_T", "E_{T}", BINS, 0., 1.5);
    TH1F *hist_N_cluster = new TH1F("N_Cluster", "N_{Cluster}", BINS, 0., 100.);
    TH1F *hist_E_vis = new TH1F( "E_vis","Normierte sichtbare Energie",BINS,0.,1.5 );

    TH1F *cutflow_muonselection_N_cluster  = new TH1F ( "cutflow_muonselection_N_cls","Anzahl Teilchen im Calo",BINS,0.,100. );                  //histogramme im cutflow
    TH1F *cutflow_muonselection_Muon_px    = new TH1F ( "cutflow_muonselection_mu_px","x-Komponente des Muon-Impulses",BINS,-50.,50. );
    TH1F *cutflow_muonselection_hist_E_vis = new TH1F ( "cutflow_muonselection_Evis","Normierte sichtbare Energie",BINS,0.,1.5 );

    TH1F *cutflow_hadronselection_N_cluster  = new TH1F ( "cutflow_hadronselection_N_cls","Anzahl Teilchen im Calo",BINS,0.,100. );
    TH1F *cutflow_hadronselection_Muon_px    = new TH1F ( "cutflow_hadronselection_mu_px","x-Komponente des Muon-Impulses",BINS,-50.,50. );
    TH1F *cutflow_hadronselection_hist_E_vis = new TH1F ( "cutflow_hadronselection_E_vis","Normierte sichtbare Energie",BINS,0.,1.5 );
    TH1F *cutflow_hadronselection_hist_E_T   = new TH1F("cutflow_hadronselection_E_T", "cutflow E_{T}", BINS, 0., 1.5);
    
    TH1F *zmass_after_hadroncuts = new TH1F("zmass_after_hadroncuts", "Z-Masse nach Had.-Cuts", BINS, 0., 1.5);

//
//  Schleife ueber alle EREIGNISSE (n)
//
    for ( n=1; n<=nevmax; n++ ) {

        cevent event;
        result = read_event ( event );

        nevent++;

//
//  read_event gibt zurueck
//      0 falls alles ok ist
//     -2 falls das Ende des Files erreicht ist
//

//
// Sicherheitsabfrage:
// Fuehre die Analyse nur aus , falls das Ereginis nicht leer ist !
//
        if ( result==0 ) {
            

//
// Auslese der totalen Anzahl an Teilchen (k) im gegebenen Ereignis
//
            ktot = event.number_particles();
            
            hist_N_cluster->Fill(ktot);

            TLorentzVector tlv_event(0,0,0,0);
            vector<TLorentzVector> vec_event;

            for ( k=1; k<=ktot; k++ ) {
                
                TLorentzVector tlv_particle;
                tlv_particle.SetPxPyPzE(event.momentum(k,1), event.momentum(k,2), event.momentum(k,3), event.GetParticleEnergy(k));
                vec_event.push_back(tlv_particle);

                tlv_event += tlv_particle;
                
                hist_p_ges->Fill ( tlv_particle.P() );
            }

//             for(vector<TFile*>::iterator f_it(files.begin()), f_it_end(files.end()); f_it != f_it_end; ++f_it) {
//             *f_it;
//             }
            hist_E_vis->Fill(tlv_event.E()/s);
            
            hist_zmass->Fill(tlv_event.M()/s);

            hist_E_T->Fill(tlv_event.Et()/s);

            ///////////////////////////////////////////////////////// cutflow beginn /////////////////////////////

            if ( tlv_event.E()/s > 0.7 && ktot >= 10 ) {                                                                     //wie gesagt, noch nicht alles perfekt (zT ueberschneidende bereiche)
                hevent++;
                cutflow_hadronselection_hist_E_T->Fill( tlv_event.Et()/s );
                cutflow_hadronselection_hist_E_vis->Fill ( tlv_event.E()/s );
                cutflow_hadronselection_N_cluster->Fill ( ktot );
            }
            zmass_after_hadroncuts->Fill(tlv_event.M()/s);

//       else{
//       if (etot < 0.8 && ktot < 20){                                                                  //wie gesagt, noch nicht alles perfekt (zT ueberschneidende bereiche)
//         muevent++;
//      cutflow_muonselection_hist_E_vis->Fill(etot);
//      cutflow_muonselection_N_cluster->Fill(ktot);
//      for (int l=1; l<=ktot; ++l) {
//          if(fabs(fabs(event.mass(k))-0.106)<0.001) {
//               px_mu = event.momentum(k,1);
//             }
//      }
//       }
//       }
        } else if ( result==-2 ) {
            break;
        }
    }

//
//  Statistik:
//
    cout << "Zahl der analysierten Ereignisse = " << nevent << endl;
    cout << "Zahl der analysierten h-Ereignisse = " << hevent << endl;
    cout << "Zahl der analysierten mu-Ereignisse = " << muevent << endl;

//
// Alle Histogramme werden auf den oben definierten File geschrieben                                                                            woher weiß er, dass die histos in dieses file gehoeren?
//

    histofile->Write();

    if ( nevent > 0 ) {
        return 0;
    } else {
        return -9;
    }
    return 0;
}


///////////////////////////////////////////////////////////////////

// void hadronselection(){
//
// }
//
// void muonselection(){
//
// }

int read_event ( cevent &event ) {
    int k;
    int l;
    int ktot;
    float px, py, pz, m;
    static bool first = 1;
    static ifstream fin;

    if ( first ) {
        cout << str_datfile << endl;
        fin.open ( str_datfile.c_str(),ios::in );
        first=0;
        if ( !fin ) {
            cout << "ERROR read_event: " << str_datfile << " cannot be read !" << endl;
            return -1;
        }
    }

    if ( !fin.eof() ) {
        ktot = -1;
        fin >> ktot;
        if ( ktot > 0 ) {
            event.setnce ( ktot );
            for ( k=1; k<=ktot; k++ ) {
                fin >> l >> px >> py >> pz >> m;
                event.setce ( l,px,py,pz,m );
            }
            return 0;
        } else if ( ktot==0 ) {
            fin.close();
            return -1;
        } else if ( ktot <0 ) {
            fin.close();
            return -2;
        }
    } else {
        fin.close();
        return -2;
    }

    return -9;

}
// kate: indent-mode cstyle; space-indent on; indent-width 4; 
