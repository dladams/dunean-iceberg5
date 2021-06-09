// drawLifetime.C
//
// David Adams
// March 2021
//
// Draw the signal strength.

// Extract Q vs tick with selection ssel from an adcrois tree or chain.
// Results are saved in the graph pgq and histogram phq.
TGraph* getChargeVsTick(TTree* ptree, string ssel) {
  string myname = "getChargeVsTick: ";
  using Index = unsigned int;
  string svar = "isam:qroi";
  // Build name and title.
  Index npt = ptree->Draw(svar.c_str(), ssel.c_str(), "goff");
  TGraph* pgq = new TGraph();
  for ( unsigned int ipt=0; ipt<npt; ++ipt ) {
    pgq->SetPoint(ipt, ptree->GetV1()[ipt], ptree->GetV2()[ipt]);
  }
  return pgq;
}

struct RunTime {
  using Index = unsigned int;
  Index run1 = 0;
  Index run2 = 0;
  Index tim1 = 0;
  Index tim2 = 0;
  static string timeToString(Index tim) {
    string stim = TTimeStamp(tim).AsString("s");
    return stim.substr(0, 16) + " UTC";
  }
  void addRun(Index run) {
    if ( run1 == 0 || run < run1 ) run1 = run;
    if ( run > run2 ) run2 = run;
  }
  void addTime(Index tim) {
    if ( tim1 == 0 || tim < tim1 ) tim1 = tim;
    if ( tim > tim2 ) tim2 = tim;
  }
  double xtim1() const { return tim1; }
  double xtim2() const { return tim2; }
  double xrun1() const { return run1; }
  double xrun2() const { return run2; }
  Index runWidth() const { return run2 - run1; }
  Index timeWidth() const { return tim2 - tim1; }
  double runCentral() const { return 0.5*(xrun2() + xrun1()); }
  Index timeCentral() const { return Index(0.5*(xtim2() + xtim1())+0.49999); }
  string runString() const {
    string sruns = "Run";
    if ( run2 > run1 ) sruns +=  "s";
    sruns += " " + to_string(run1);
    if ( run2 > run1 ) sruns += "-" + to_string(run2);
    return sruns;
  }
  string time1String() const { return timeToString(tim1); }
  string time2String() const { return timeToString(tim2); }
  string centralTimeString() const { return timeToString(timeCentral()); }
};

// Get time and run range.
RunTime getRunTime(TTree* ptree, string ssel) {
  string myname = "getRunTime: ";
  using Index = RunTime::Index;
  string svar = "run:time";
  // Build name and title.
  Index npt = ptree->Draw(svar.c_str(), ssel.c_str(), "goff");
  RunTime rt;
  for ( unsigned int ipt=0; ipt<npt; ++ipt ) {
    rt.addRun(ptree->GetV1()[ipt]);
    rt.addTime(ptree->GetV2()[ipt]);
  }
  return rt;
}

// Format X vs channel for drawing.
TGraph* makeDrawable(TGraph* pg, unsigned int maxpt) {
  TGraph* pgo = new TGraph(*pg);
  using Index = unsigned int;
  Index npt = pg->GetN();
  Index jpt = 0;
  for ( Index ipt=0; ipt<npt; ++ipt ) {
    if ( npt > maxpt && npt*gRandom->Rndm() > maxpt ) continue;
    double x = pg->GetX()[ipt];
    double y = pg->GetY()[ipt];
    x += gRandom->Rndm();
    pgo->SetPoint(jpt, x, y);
    ++jpt;
  }
  pgo->Set(jpt);
  return pgo;
}

// Make MPV vs. tick graph.
TGraph* getMpvVsTick(const TGraph* pgq, int nbin, int tick1, int tick2) {
  string myname = "getmpvVsTick:";
  using Index = unsigned int;
  Index ntick = tick2 - tick1;
  Index itck1 = tick1;
  Index npt = pgq->GetN();
  TPadManipulator* pman = nullptr;
  Index ipad = 0;
  Index ifil = 0;
  TGraphErrors* pgm = new TGraphErrors();
  pgm->SetMarkerStyle(5);
  //pgm->SetMarkerSize(3);
  for ( Index ibin=0; ibin<nbin; ++ibin ) {
    Index itck2 = tick1 + ((ibin+1)*ntick)/nbin;
    string sttl = "ROI charge for ticks [" + to_string(itck1) +
                  ", " + to_string(itck2) + ");Q [ke]; # ROI";
    TH1* phm = new TH1F("hmpv", sttl.c_str(), 100, 0, 50);
    phm->SetDirectory(0);
    double sum = 0.0;
    for ( Index ipt=0; ipt<npt; ++ipt ) {
      double xtck = pgq->GetX()[ipt];
      Index itck = xtck + 0.01;
      if ( itck >= itck1 && itck < itck2 ) {
        sum += xtck;
        double q = pgq->GetY()[ipt];
        phm->Fill(q);
      }
    }
    Index nent = phm->GetEntries();
    double atck = sum/nent;
    cout << myname << "Bin " << ibin << " at tick " << atck << "  entry count: " << nent << endl;
    TF1* pf = langaus(phm, false);
    double mpv = pf->GetParameter(1);
    double dmpv = pf->GetParError(1);
    pgm->SetPoint(ibin, atck, mpv);
    pgm->SetPointError(ibin, 0.0, dmpv);
    cout << myname << "       MPV = " << mpv << endl;
    if ( pman == nullptr ) {
      pman = new TPadManipulator(1400, 1000);
      pman->split(4, 4);
      ipad = 0;
    }
    pman->man(ipad++)->add(phm, "hist");
    if ( ipad == 16  || ibin+1 == nbin ) {
      string fnamo = "qbins" + to_string(ifil++) + ".png";
      cout << myname << "Writing " << fnamo << endl;
      pman->print(fnamo);
      delete pman;
      pman = nullptr;
    }
    delete phm;
    itck1 = itck2;
  }
  return pgm;
}
    

float samplingFrequency(string sper) {
  float rsamp = 0.0;
  if ( sper[0] == '5' ) rsamp = 62.5/32.0;
  if ( sper[0] == '4' ) rsamp = 2.0;
  return rsamp;
}

TF1* fitLifetimeGraph(TGraph* pgq, string slab, float fmin, float fmax, float t0fix, float taufix, float q0fix) {
  string myname = "drawLifetime: ";
  using Index = unsigned int;
  if ( pgq == nullptr ) return nullptr;
  Index npt = pgq->GetN();
  if ( npt == 0 ) return nullptr;
  float rsamp = samplingFrequency(slab);
  TF1* pf = new TF1("lfit", "[2]*exp(([0]-x)/[1])");
  pf->SetParNames("t0", "tau", "q0");
  pf->SetParameters(fmin, 1000, 1);
  pf->FixParameter(0, t0fix);
  if ( taufix ) pf->FixParameter(1, rsamp*taufix);
  if ( q0fix ) pf->FixParameter(2, q0fix);
  pgq->Fit(pf, "", "", fmin, fmax);
  pf->Print();
  //pg->Draw("AP");
  return pf;
}

//   sper: Run period, e.g. 5C22-cos3
//   scha = channel selection: x , x1 or x2
//   w1, w1 = inclusive range of allowed ROI widths
//   fopt: Fitting option
//         nofit - no fit
//         direct - chi-square fit of Q vs tick points
//         mpvNNN - fit of MPV in NNN bins
//   taufix: If nonzero, lifetime is fixed to this value
//   q0fix: If nonzero, Q0 is fixed to this value
int drawLifetime(string sper, string scha, float rmsmax, int w1, int w2, string fopt, float taufix =0.0, float q0fix =0.0, bool doDraw =false) {
  const string myname = "drawLifetime: ";
  using Index = unsigned int;
  string fnamin = "chains/adcrois-" + sper + ".txt";
  float tick0 = 661;
  float tick1 = 670;
  float tick2 = 1080;
  if ( sper[0] == '4' ) {
    tick0 = 489;
    tick1 = 500;
    tick2 = 940;
  }
  string stick1 = to_string(tick1);
  string stick2 = to_string(tick2);
  string ssel = "status==0&&isam>" + stick1 + "&&isam<" + stick2 + "&&";
  if ( scha == "x" ) {
    ssel += "channel>=800";
  } else if ( scha == "x1" ) {
    ssel += "channel>=800&&channel<1040";
  } else if ( scha == "x2" ) {
    ssel += "channel>=1040";
  } else {
    cout << myname << "ERROR: Invalid channel specifier: " << scha << endl;
    return 1;
  }
  string sfsel = scha;
  string slab = sper + " " + scha;
  if ( rmsmax > 0.0 ) {
    string sval = to_string(rmsmax);
    while ( sval.size() > 3 ) {
      Index ipos = sval.size() - 1;
      if ( sval[ipos] != '0' ) break;
      if ( sval[ipos-1] == '.' ) break;
      sval = sval.substr(0, ipos);
    }
    ssel += "&&samRms<" + sval;
    sfsel += "rms" + to_string(int(100*rmsmax+0.01));
    slab += " RMS<" + sval;
  }
  if ( w2 > w1 ) {
    string sw1 = to_string(w1);
    string sw2 = to_string(w2);
    ssel += "&&nsam>=" + sw1;
    ssel += "&&nsam<=" + sw2;
    sfsel += "w" + sw1 + "-" + sw2;
    slab += " " + sw1 + " #leq w_{ROI} #leq " + sw2;
  }
  cout << "Selection: " << ssel << endl;
  TChain* pch = makeChain(fnamin);
  TGraph* pgq = getChargeVsTick(pch, ssel);
  if ( pgq == nullptr ) return 1;
  RunTime rt = getRunTime(pch, ssel);
  string rtlab = rt.runString() + " " + rt.centralTimeString();
  Index npt = pgq->GetN();
  if ( npt == 0 ) return 2;
  TGraph* pgqDraw = makeDrawable(pgq, 200000);
  float rsamp = samplingFrequency(slab);
  // Fit lifetime.
  float tau = 0.0;
  float dtau = 0.0;
  float q0 = 0.0;
  float dq0 = 0.0;
  float csq = 0.0;
  int ndf = 0;
  string sfopt = fopt + "fit";
  TF1* pf = nullptr;
  TGraph* pgm = nullptr;
  if ( fopt == "direct" ) {
    cout << myname << "Doing direct fit of lifetime." << endl;
    pf = fitLifetimeGraph(pgq, slab, tick1, tick2, tick0, taufix, q0fix);
  } else if ( fopt.substr(0,3) == "mpv" ) {
    Index nbin = 0;
    istringstream ssnbin(fopt.substr(3));
    ssnbin >> nbin;
    cout << myname << "MPV nbin: " << nbin << endl;
    pgm = getMpvVsTick(pgq, nbin, tick1, tick2);
    pgqDraw->SetMarkerColor(16);
    pf = fitLifetimeGraph(pgm, slab, tick1, tick2, tick0, taufix, q0fix);
  } else {
    cout << myname << "No fit." << endl;
    sfopt = "nofit";
  }
  if ( pf != nullptr ) {
    tau = pf->GetParameter(1)/rsamp;
    dtau = pf->GetParError(1)/rsamp;
    q0 = pf->GetParameter(2);
    dq0 = pf->GetParError(2);
    csq = pf->GetChisquare();
    ndf = pf->GetNDF();
    cout << myname << " tau: " << tau << " +/- " << dtau << " us" << endl;
  }
  // Make text report.
  int tim = rt.timeCentral();
  int dtim = rt.timeWidth()/2;
  string sfout = "reports-tau/" + sper + ".txt";
  ofstream fout(sfout.c_str());
  fout << sper << endl;
  fout << npt << endl;
  fout << tim << " " << dtim << endl;
  fout << tau << " " << dtau << endl;
  fout << csq << " " << ndf << endl;
  fout << q0 << " " << dq0 << endl;
  fout << tick0 << endl;
  // Make plot labels;
  vector<string> slabs;
  ostringstream ssout;
  ssout.precision(0);
  ssout.str("");
  ssout << "N_{ROI}: " << fixed << npt;
  slabs.push_back(ssout.str());
  if ( tau > 0.0 ) {
    ssout.str("");
    ssout << "#tau: " << fixed << tau;
    if ( dtau > 0.0 ) ssout << " #pm " << dtau;
    ssout << " #mus";
    slabs.push_back(ssout.str());
  }
  if ( q0 > 0.0 ) {
    ssout.str("");
    ssout.precision(2);
    ssout << "Q_{0}: " << q0;
    if ( dq0 > 0.0 ) ssout << " #pm " << dq0;
    ssout << " ke";
    slabs.push_back(ssout.str());
  }
  if ( csq > 0.0 ) {
    ssout.str("");
    ssout.precision(1);
    ssout << "#chi^{2}/N_{DOF}: " << csq << "/" << ndf;
    slabs.push_back(ssout.str());
  }
  // Create plot.
  TPadManipulator* ppad = doDraw ? new TPadManipulator(1000,  700)
                                 : new TPadManipulator(1400, 1000);
  string sttl = "Lifetime " + fopt + " fit for " + slab + "; Tick; Q [ke]";
  TH2* pha = new TH2F("ha", sttl.c_str(), 10, tick1, tick2, 100, 0, 100);
  pha->SetStats(0);
  ppad->add(pha, "axis");
  ppad->add(pgqDraw, "P");
  if ( pgm != nullptr ) {
    if ( ! doDraw ) pgm->SetMarkerSize(2);
    else pgm->SetMarkerSize(1.5);
    ppad->add(pgm, "P");
  }
  ppad->addAxis();
  ppad->centerAxisTitles();
  ppad->setRangeY(0.0, 60.0);
  ppad->setGridY(0);
  float ylab = 0.80;
  float dylab = 0.045;
  for ( string slab : slabs ) {
    TLatex* ptxt = new TLatex(0.7, ylab, slab.c_str());
    ptxt->SetNDC();
    ptxt->SetTextFont(42);
    ptxt->SetTextSize(0.035);
    ppad->add(ptxt);
    ylab -= dylab;
  }
  ppad->setLabel(rtlab);
  ppad->getLabelObject().SetTextSize(0.03);
  cout << myname << rtlab << endl;
  if ( doDraw ) {
    ppad->draw();
  } else {
    // Build output file name.
    string fnamo = "plots-tau/tau_" + sper + "_sel" + sfsel + "_" + sfopt;
    if ( taufix > 0.0 ) fnamo += "_fixt" + to_string(int(taufix + 0.01));
    if ( q0fix > 0.0 ) fnamo += "_fixq" + to_string(int(10*q0fix + 0.01));
    fnamo += ".png";
    cout << myname << "Writing " << fnamo << endl;
    ppad->print(fnamo);
  }
  return 0;
}
