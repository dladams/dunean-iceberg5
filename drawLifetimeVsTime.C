#include "dirent.h"
void drawLifetimeVsTime(int day1, int day2, bool doDraw =false) {
  string dnam = "reports-tau";
  bool doPm = true;
  bool expandErrors = true;
  float maxerr = 500.0;
  using Index = unsigned int;
  TTimeStamp ts0(2021, 2, 27, 0, 0, 0);
  Index tim0 = ts0.GetSec();
  TGraphErrors* pg = new TGraphErrors;
  LineColors lc;
  pg->SetLineWidth(2);
  pg->SetLineColor(lc.blue());
  pg->SetMarkerColor(lc.blue());
  string line;
  float secToDay = 1.0/(3600*24);
  if ( auto dir = opendir(dnam.c_str()) ) {
    while (auto f = readdir(dir)) {
      if ( f->d_name[0] != '5' ) continue;
      string fnam = dnam + "/" + f->d_name;
      ifstream fin(fnam.c_str());
      getline(fin, line);
      string sper = line;
      Index nroi;
      Index itim, dtim;
      float tau, dtau;
      fin >> nroi >> itim >> dtim >> tau >> dtau;
      if ( expandErrors ) {
        float csq, ndf;
        fin >> csq >> ndf;
        float xfac = csq > ndf ? csq/ndf : 1.0;
        dtau *= xfac;
      }
      if ( dtau > maxerr ) continue;
      float day = secToDay*(itim - tim0);
      float dday = secToDay*dtim;
      cout << setw(20) << sper << setprecision(2) << ": "
           << fixed << day<< " +/- " << dday;
      cout << ", " << tau << " +/- " << dtau << endl;
      Index ipt = pg->GetN();
      pg->SetPoint(ipt, day, tau);
      pg->SetPointError(ipt, dday, dtau);
    }
    closedir(dir);
  }
  TGraph* pgpm = nullptr;
  if ( doPm ) {
    pgpm = new TGraph();
    pgpm->SetMarkerStyle(4);
    pgpm->SetMarkerColor(lc.orange());
    ifstream fin("iceberg5-lifetime.csv");
    getline(fin, line);
    cout << line << endl;
    while ( getline(fin, line) ) {
      StringManipulator sm(line,2);
      sm.split(",");
      StringManipulator smtim(sm.splits()[0]);
      smtim.replace("T", " ");
      string stim = smtim.str();
      float tau = 1000000.0*StringManipulator(sm.splits()[2]).toFloat();
      if ( tau <= 0.0 ) continue;
      Index itim = TDatime(stim.c_str()).Convert();
      float day = secToDay*(itim - tim0);
      //cout << stim << " (" << itim << "): " << tau << endl;
      pgpm->SetPoint(pgpm->GetN(), day, tau);
    }
  }
  TPadManipulator* pman = doDraw ? new TPadManipulator(1050, 750)
                                 : new TPadManipulator(1400, 1000);
  TH2* pha = new TH2F("ha", "Electron lifetime vs. time; Days since 2021-02-27; Lifetime [#mus]", 10, day1, day2, 10, 0, 1200);
  pha->SetStats(0);
  pman->add(pha, "axis");
  if ( pgpm != nullptr ) pman->add(pgpm, "p");
  pman->add(pg, "p");
  float yl1 = 0.15;
  float yl2 = yl1 + (1.5 + doPm)*0.04;
  TLegend* pleg = new TLegend(0.15, yl1, 0.40, yl2);
  pleg->SetBorderSize(0);
  pleg->SetFillStyle(0);
  pleg->SetMargin(0.10);  // Fraction used for symbol
  pleg->SetTextSize(0.035);
  if ( doPm ) pleg->AddEntry(pgpm, "Purity monitor", "p");
  pleg->AddEntry(pg, "ROI fit", "E");
  pman->add(pleg);
  pman->addAxis();
  pman->centerAxisTitles();
  if ( doDraw ) {
    pman->draw();
  } else {
    string fnam = "plots-tau/ib5-lifetime-days" + to_string(day1) + "_" + to_string(day2) + ".png";
    cout << "Writing " << fnam << endl;
    pman->print(fnam);
  }
}

