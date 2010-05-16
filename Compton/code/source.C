#include <iostream> 
#include <sys/time.h>
#include <math.h>

int main(int argc,char* argv[]){
  const double daysperyear = 365.22;
  struct tm dd;  
  dd.tm_sec = 0;           /* seconds */
  dd.tm_min = 0;           /* minutes */
  dd.tm_hour = 12;         /* hours */
  dd.tm_mday = 1;          /* day of the month */
  dd.tm_mon = 11-1;           /* month */
  dd.tm_year = 1996-1900;        /* year */
  dd.tm_wday = 5;        /* day of the week, 0=Sun */
  dd.tm_yday = 0;        /* day in the year */
  dd.tm_isdst = 0;       /* daylight saving time */
  time_t tp = mktime(&dd);
  time_t now = time(0);
  std::cout << "Eichung am " << ctime(&tp);
  std::cout << "Jetzt      " << ctime(&now);
  double s(now-tp); 
  double m(s/60.);
  double h(m/60.);
  double d(h/24.);
  double j(d/daysperyear);
  std::cout << "Differenz  " << s << " s" << std::endl;
  std::cout << "Differenz  " << m << " min" << std::endl;
  std::cout << "Differenz  " << h << " h" << std::endl;
  std::cout << "Differenz  " << d << " d" << std::endl;
  std::cout << "Differenz  " << j << " a" << std::endl;
  return 0;
}
