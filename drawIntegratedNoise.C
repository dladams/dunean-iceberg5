// drawIntegratedNoise.C
//
// David Adams
// April 2021
//
// Make plots of mean integrated noise vs. width using
// a ROI tree with noise as input.

TGraph* graphIntegratedNoise(TTree* ptre) {
  using Index = unsigned int;
  string ssel = "status==0&&channel>=800&&samRms<0.8";
  vector<Index> iwids = {1, 10, 20, 30, 40, 50, 60, 80, 100};
  TGraph* pgn = new TGraph;
  pgn->SetMarkerStyle(2);
  TH1* phn = new TH1F("hn", "Noise", 200, 0, 2.0);
  for ( Index iwid : iwids ) {
    string svar = "nsgRms";
    if ( iwid > 1 ) {
      string swid = to_string(iwid);
      //while ( swid.size() < 3 ) swid = "0" + swid;
      svar += swid;
    }
    svar += ">>hn";
    Index nsel = ptre->Draw(svar.c_str(), ssel.c_str(), "goff");
    if ( nsel > 100 ) {
      double noi = 1000*phn->GetMean();
      pgn->SetPoint(pgn->GetN(), iwid, noi);
    }
  }
  return pgn;
}

TPadManipulator* drawIntegratedNoise(string sfilpat, string srun, string ssprocs, bool doDraw=false) {
  const string myname = "drawIntegratednoise: ";
  using Index = unsigned int;
  StringManipulator smprocs(ssprocs);
  vector<string> sprocs = smprocs.split(",");
  vector<TGraph*> grns;
  vector<string> slabs;
  LineColors lc;
  vector<int> mrks = {68, 70, 88, 91};
  vector<int> cols = {lc.blue(), lc.green(), lc.blue(), lc.green()};
  Index igra = 0;
  string srunf = srun;
  while ( srunf.size() < 6 ) srunf = "0" + srunf;
  for ( string sproc : sprocs ) {
    StringManipulator smfil(sfilpat, true);
    smfil.replace("%REC%", sproc);
    smfil.replace("%RUN%", srunf);
    string sfil = smfil.str();
    TFile* pfil = TFile::Open(sfil.c_str(), "READ");
    if ( pfil == nullptr || ! pfil->IsOpen() ) {
      cout << myname << "Unable to open " << sfil << endl;
      return nullptr;
    }
    TTree* ptre = dynamic_cast<TTree*>(pfil->Get("adcrois"));
    if ( ptre == nullptr ) {
      cout << myname << "Unable to find tree adcrois" << endl;
      return nullptr;
    }
    TGraph* pgn = graphIntegratedNoise(ptre);
    if ( pgn != nullptr && pgn->GetN() ) {
      pgn->SetMarkerStyle(mrks[igra]);
      pgn->SetMarkerColor(cols[igra]);
      pgn->SetLineColor(cols[igra]);
      pgn->SetMarkerSize((doDraw?1.5:3.0));
      grns.push_back(pgn);
      pgn->SetLineStyle(2);
      string slab = sproc;
      if ( slab == "cal" ) slab = "calibrated";
      if ( slab == "tai" ) slab = "Tail removed";
      if ( slab == "ped" ) slab = "PUP";
      if ( slab == "cnr" ) slab = "CNR";
      if ( slab == "pnr" ) slab = "PUP + CNR";
      slabs.push_back(slab);
    }
    ++igra;
  }
  string sttl = "Integrated noise vs. width for Iceberg run " + srun + "; Number of samples; Noise [e]";
  TPadManipulator* pman = doDraw ? new TPadManipulator(1050, 750)
                                 : new TPadManipulator(1400, 1000);
  /// Create axis.
  TH2F* pha = new TH2F("ha", sttl.c_str(), 102, 0, 102, 120, 0, 1200);
  pha->SetStats(0);
  // Create legend.
  float yl1 = 0.15;
  float yl2 = yl1 + (grns.size()+0.5)*0.04;
  TLegend* pleg = new TLegend(0.70, yl1, 0.87, yl2);
  pleg->SetBorderSize(0);
  pleg->SetFillStyle(0);
  pleg->SetMargin(0.20);  // Fraction used for symbol
  pleg->SetTextSize(0.035);
  pman->add(pleg);
  pman->addAxis();
  pman->centerAxisTitles();
  pman->add(pha);
  Index ngra = grns.size();
  for ( Index igra=0; igra<ngra; ++igra ) {
    TGraph* pg = grns[igra];
    string slab = slabs[igra];
    pman->add(pg, "pl");
    pleg->AddEntry(pg, slab.c_str(), "P");
  }
  pman->add(pleg);
  pman->addAxis();
  pman->centerAxisTitles();
  pman->setGridY();
  if ( doDraw ) {
    pman->draw();
  } else {
    string fnout = "noiseint-run" + srunf + ".png";
    cout << myname << "Writing " << fnout << endl;
    pman->print(fnout);
  }
  return pman;
}
