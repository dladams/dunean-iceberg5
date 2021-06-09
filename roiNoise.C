int gainIndex(string sper) {
  return stoi(sper.substr(2,1));
}

float ampGain(int igain) {
  if ( igain == 0 ) return 4.7;
  if ( igain == 1 ) return 7.8;
  if ( igain == 2 ) return 14.0;
  if ( igain == 3 ) return 25.0;
  return 0.0;
}

string ampGainString(int igain) {
  if ( igain == 0 ) return "4.7";
  if ( igain == 1 ) return "7.8";
  if ( igain == 2 ) return "14.0";
  if ( igain == 3 ) return "25.0";
  return "";
}

float ampShaping(int ishap) {
  if ( ishap == 0 ) return 0.5;
  if ( ishap == 1 ) return 1.0;
  if ( ishap == 2 ) return 2.0;
  if ( ishap == 3 ) return 3.0;
  return 0.0;
}

string ampShapingString(int ishap) {
  if ( ishap == 0 ) return "0.5";
  if ( ishap == 1 ) return "1.0";
  if ( ishap == 2 ) return "2.0";
  if ( ishap == 3 ) return "3.0";
  return "";
}

int shapingIndex(string sper) {
  return stoi(sper.substr(3,1));
}

float aOverH(int ishap) {
  float shap = ishap == 0 ? 0.5 : float(ishap);
  shap *= 1.1;
  float sampling = 1.9531;   // Iceberg 5 and beyond
  float kfac = 1.269*sampling*shap;
  return kfac;
}

vector<float> roiNoisePeriod(string sper, int draw =0, bool quiet =true) {
  string tfnam = "chains/adcrois-" + sper + ".txt";
  TTree* ptree = makeChain(tfnam, quiet);
  vector<float> enois;
  if ( ptree == nullptr || ptree->GetEntries() == 0 ) {
    cout << "Unable to open tree in " << tfnam << endl;
    return enois;
  }
  int ishap = shapingIndex(sper);
  float shap = ishap == 0 ? 0.5 : ishap;
  float noiseSamMax = 0.3/shap;
  float noiseIntMax = 2.0;
  vector<string> hnams = {"hsamc", "hsami", "hintc", "hinti"};
  vector<string> httls = {
    "Sample noise for " + sper + " collection; Noise [e]; Entries",
    "Sample noise for " + sper + " induction; Noise [e]; Entries",
    "Integrated noise for " + sper + " collection; Noise [e]; Entries",
    "Integrated noise for " + sper + " induction; Noise [e]; Entries"
  };
  vector<string> svars = {"nsgRms>>hsamc", "nsgRms>>hsami", "nsgRms50>>hintc", "nsgRms50>>hinti"};
  vector<string> ssels = {
    "status==0&&channel>=800&&samRms<0.8&&nsgRms>0.0",
    "status==0&&channel<800&&samRms<0.4&&nsgRms>0.0",
    "status==0&&channel>=800&&samRms<0.8&&nsgRms50>0.0",
    "status==0&&channel<800&&samRms<0.4&&nsgRms50>0.0"
  };
  vector<float> xmaxs = {noiseSamMax, noiseSamMax, noiseIntMax, noiseIntMax};
  for ( unsigned int ih=0; ih<4; ++ih ) {
    string hnam = hnams[ih];
    string httl = httls[ih];
    string svar = svars[ih];
    string ssel = ssels[ih];
    float xmax = xmaxs[ih];
    TH1* ph = dynamic_cast<TH1*>(gDirectory->Get(hnam.c_str()));
    if ( ph == nullptr ) {
      ph = new TH1F(hnam.c_str(), httl.c_str(), 100, 0, xmax);
    }
    ptree->Draw(svar.c_str(), ssel.c_str(), "goff");
    //if ( draw ) {
    //  new TCanvas;
    //  ph->Draw();
    //}
    enois.push_back(1000*ph->GetMean());
  }
  return enois;
}
  
void roiNoiseVsGain() {
  const string myname = "roiNoiseVsGain: ";
  ifstream fin("runperiods.txt");
  string sper;
  vector<int> precs = {1, 1, 0, 0, 0, 0};
  int wper = 11;
  int wshap = 3;
  int wgain = 3;
  vector<int> wids = {9, 8, 8, 7};
  cout << "                        Noise [e]" << endl;
  cout << setw(wper+wgain+wshap+32) << "Sample       Integrated" << endl;
  cout << setw(wper) << "Run period"  << setw(wgain) << "ig" << setw(wshap) << "is";
  vector<string> labs = {"Col.", "Ind.", "Col.", "Ind."};
  for ( unsigned int ih=0; ih<4; ++ih ) cout << setw(wids[ih]) << labs[ih];
  cout << endl;
  using GraphVector = vector<TGraph*>;
  using GraphVV = vector<GraphVector>;
  using GraphVVV = vector<GraphVV>;
  GraphVVV gras(3, GraphVV(2, GraphVector(4, nullptr)));  // gras[sample,integrated][col,ind][ishap]
  GraphVector& samcGras = gras[0][0];
  GraphVector& samiGras = gras[0][1];
  GraphVector& intcGras = gras[1][0];
  GraphVector& intiGras = gras[1][1];
  GraphVector& enccGras = gras[2][0];
  GraphVector& enciGras = gras[2][1];
  while ( fin >> sper ) {
    //cout << sper << endl;
    int igain = gainIndex(sper);
    int ishap = shapingIndex(sper);
    float kfac = aOverH(ishap);
    if ( ishap < 0 || ishap > 3 ) {
      cout << myname << "ERROR: Period " << sper << " has invalid shaping index " << ishap << endl;
      continue;
    }
    cout << setw(wper) << sper << setw(wgain) << igain << setw(wshap) << ishap;;
    vector<float> enois = roiNoisePeriod(sper);
    for ( unsigned int ih=0; ih<6; ++ih ) {
      float enoi = ih < 4 ? enois[ih] : kfac*enois[ih-4];
      if ( ih < 4 ) {
        int prec = precs[ih];
        cout << setw(wids[ih]) << std::fixed << setprecision(prec) << enoi;
      }
      float x = ampGain(igain);
      TGraph*& pgra = gras[ih/2][ih%2][ishap];
      if ( pgra == nullptr ) pgra = new TGraph;
      pgra->SetPoint(pgra->GetN(), x, enoi);
    }
    cout << endl;
  }
  TGraph* pgra = gras[1][0][2];
  cout << "Graph size: " << pgra->GetN() << endl;
  pgra->SetMarkerStyle(2);
  pgra->Draw("AP");
  LineColors lc;
  vector<int> shapCols = {lc.green(), lc.blue(), lc.red(), lc.brown()};
  TH2* phai = new TH2F("hai", "Iceberg 5 50-sample integrated noise; Preamp gain [mV/fC]; Noise [e]", 26, 0, 26, 10, 10, 1200);
  TH2* phas = new TH2F("has", "Iceberg 5 sample noise; Preamp gain [mV/fC]; Noise [e]", 26, 0, 26, 10, 10, 1200);
  TH2* phae = new TH2F("has", "Iceberg 5 ENC; Preamp gain [mV/fC]; Noise [e]", 26, 0, 26, 10, 10, 1200);
  phai->SetStats(0);
  phas->SetStats(0);
  phae->SetStats(0);
  int xpix = 1400;
  int ypix = 1000;
  TPadManipulator intMan(xpix, ypix);
  intMan.add(phai, "AXIS");
  TPadManipulator samMan(xpix, ypix);
  samMan.add(phas, "AXIS");
  TPadManipulator encMan(xpix, ypix);
  encMan.add(phae, "AXIS");
  vector<string> legLabels;
  vector<TGraph*> legGraphs;
  for ( int isha=0; isha<4; ++isha ) {
    int col = shapCols[isha];
    TGraph* pgraic = intcGras[isha];
    TGraph* pgraii = intiGras[isha];
    TGraph* pgrasc = samcGras[isha];
    TGraph* pgrasi = samiGras[isha];
    TGraph* pgraec = enccGras[isha];
    TGraph* pgraei = enciGras[isha];
    cout << myname << "Shaping " << isha << " col/ind sizes: " << pgraic->GetN() << ", " << pgraic->GetN() << endl;
    if ( pgraic == nullptr ) continue;
    pgraic->SetMarkerStyle(2);
    pgraii->SetMarkerStyle(5);
    pgrasc->SetMarkerStyle(2);
    pgrasi->SetMarkerStyle(5);
    pgraec->SetMarkerStyle(2);
    pgraei->SetMarkerStyle(5);
    for ( TGraph* pgra : {pgraic, pgraii, pgrasc, pgrasi, pgraec, pgraei} ) {
      pgra->SetMarkerColor(col);
      pgra->SetMarkerSize(2.0);
    }
    intMan.add(pgraic, "p");
    intMan.add(pgraii, "p");
    samMan.add(pgrasc, "p");
    samMan.add(pgrasi, "p");
    encMan.add(pgraec, "p");
    encMan.add(pgraei, "p");
    legGraphs.push_back(pgraic);
    legGraphs.push_back(pgraii);
    string slab = (isha == 0 ? string("0.5") : to_string(isha)) + " #mus";
    string slabc = slab + " collection";
    string slabi = slab + " induction";
    legLabels.push_back(slabc);
    legLabels.push_back(slabi);
  }
  // Create legend.
  float yleg1 = 0.12;
  float yleg2 = yleg1 + 0.045*(legLabels.size()+0.5);
  TLegend* pleg = new TLegend(0.12, yleg1, 0.45, yleg2);
  pleg->SetFillStyle(0);
  pleg->SetBorderSize(0);
  pleg->SetMargin(0.10);  // Fraction used for symbol
  for ( int igra=0; igra<legGraphs.size(); ++igra ) {
    pleg->AddEntry(legGraphs[igra], legLabels[igra].c_str(), "p");
  }
  // Plot integrated noise.
  intMan.add(pleg);
  intMan.setRangeX(0, 26);
  intMan.setRangeY(0, 1200);
  intMan.addAxis();
  intMan.setGridY();
  string fnout = "noiseVsGainInt.png";
  intMan.print(fnout);
  cout << myname << "Printed " << fnout << endl;
  // Plot sample noise.
  float dyleg = 0.40;
  pleg->SetY1(yleg1 + dyleg);
  pleg->SetY2(yleg2 + dyleg);
  samMan.add(pleg);
  samMan.setRangeX(0, 26);
  samMan.setRangeY(0, 300);
  samMan.addAxis();
  samMan.setGridY();
  fnout = "noiseVsGainSam.png";
  samMan.print(fnout);
  cout << myname << "Printed " << fnout << endl;
  // Plot ENC.
  dyleg = 0.0;
  pleg->SetY1(yleg1 + dyleg);
  pleg->SetY2(yleg2 + dyleg);
  encMan.add(pleg);
  encMan.setRangeX(0, 26);
  encMan.setRangeY(0, 400);
  encMan.addAxis();
  encMan.setGridY();
  fnout = "noiseVsGainEnc.png";
  encMan.print(fnout);
  cout << myname << "Printed " << fnout << endl;
}

void roiNoiseVsShaping() {
  const string myname = "roiNoiseVsShaping: ";
  ifstream fin("runperiods.txt");
  string sper;
  vector<int> precs = {1, 1, 0, 0, 0, 0};
  int wper = 11;
  int wshap = 3;
  int wgain = 3;
  vector<int> wids = {9, 8, 8, 7};
  cout << "                        Noise [e]" << endl;
  cout << setw(wper+wgain+wshap+32) << "Sample       Integrated" << endl;
  cout << setw(wper) << "Run period"  << setw(wgain) << "ig" << setw(wshap) << "is";
  vector<string> labs = {"Col.", "Ind.", "Col.", "Ind."};
  for ( unsigned int ih=0; ih<4; ++ih ) cout << setw(wids[ih]) << labs[ih];
  cout << endl;
  using GraphVector = vector<TGraph*>;
  using GraphVV = vector<GraphVector>;
  using GraphVVV = vector<GraphVV>;
  GraphVVV gras(3, GraphVV(2, GraphVector(4, nullptr)));  // gras[sample,integrated][col,ind][ishap]
  GraphVector& samcGras = gras[0][0];
  GraphVector& samiGras = gras[0][1];
  GraphVector& intcGras = gras[1][0];
  GraphVector& intiGras = gras[1][1];
  GraphVector& enccGras = gras[2][0];
  GraphVector& enciGras = gras[2][1];
  while ( fin >> sper ) {
    //cout << sper << endl;
    int igain = gainIndex(sper);
    int ishap = shapingIndex(sper);
    float kfac = aOverH(ishap);
    if ( igain < 0 || igain > 3 ) {
      cout << myname << "ERROR: Period " << sper << " has invalid gain index " << igain << endl;
      continue;
    }
    cout << setw(wper) << sper << setw(wgain) << igain << setw(wshap) << ishap;;
    vector<float> enois = roiNoisePeriod(sper);
    for ( unsigned int ih=0; ih<6; ++ih ) {
      float enoi = ih < 4 ? enois[ih] : kfac*enois[ih-4];
      if ( ih < 4 ) {
        int prec = precs[ih];
        cout << setw(wids[ih]) << std::fixed << setprecision(prec) << enoi;
      }
      float x = ampShaping(ishap);
      TGraph*& pgra = gras[ih/2][ih%2][igain];
      if ( pgra == nullptr ) pgra = new TGraph;
      pgra->SetPoint(pgra->GetN(), x, enoi);
    }
    cout << endl;
  }
  TGraph* pgra = gras[1][0][2];
  cout << "Graph size: " << pgra->GetN() << endl;
  pgra->SetMarkerStyle(2);
  pgra->Draw("AP");
  LineColors lc;
  vector<int> shapCols = {lc.green(), lc.blue(), lc.red(), lc.brown()};
  float xmax = 3.2;
  TH2* phai = new TH2F("hai", "Iceberg 5 50-sample integrated noise; Shaping time [#mus]; Noise [e]", 26, 0, xmax, 10, 10, 1200);
  TH2* phas = new TH2F("has", "Iceberg 5 sample noise; Shaping time [#mus]; Noise [e]", 26, 0, 26, 10, 10, 1200);
  TH2* phae = new TH2F("has", "Iceberg 5 ENC; Shaping time [#mus]; Noise [e]", 26, 0, 26, 10, 10, 1200);
  phai->SetStats(0);
  phas->SetStats(0);
  phae->SetStats(0);
  int xpix = 1400;
  int ypix = 1000;
  TPadManipulator intMan(xpix, ypix);
  intMan.add(phai, "AXIS");
  TPadManipulator samMan(xpix, ypix);
  samMan.add(phas, "AXIS");
  TPadManipulator encMan(xpix, ypix);
  encMan.add(phae, "AXIS");
  vector<string> legLabels;
  vector<TGraph*> legGraphs;
  for ( int igai=0; igai<4; ++igai ) {
    int col = shapCols[igai];
    TGraph* pgraic = intcGras[igai];
    TGraph* pgraii = intiGras[igai];
    TGraph* pgrasc = samcGras[igai];
    TGraph* pgrasi = samiGras[igai];
    TGraph* pgraec = enccGras[igai];
    TGraph* pgraei = enciGras[igai];
    cout << myname << "Shaping " << igai << " col/ind sizes: " << pgraic->GetN() << ", " << pgraic->GetN() << endl;
    if ( pgraic == nullptr ) continue;
    pgraic->SetMarkerStyle(2);
    pgraii->SetMarkerStyle(5);
    pgrasc->SetMarkerStyle(2);
    pgrasi->SetMarkerStyle(5);
    pgraec->SetMarkerStyle(2);
    pgraei->SetMarkerStyle(5);
    for ( TGraph* pgra : {pgraic, pgraii, pgrasc, pgrasi, pgraec, pgraei} ) {
      pgra->SetMarkerColor(col);
      pgra->SetMarkerSize(2.0);
    }
    intMan.add(pgraic, "p");
    intMan.add(pgraii, "p");
    samMan.add(pgrasc, "p");
    samMan.add(pgrasi, "p");
    encMan.add(pgraec, "p");
    encMan.add(pgraei, "p");
    legGraphs.push_back(pgraic);
    legGraphs.push_back(pgraii);
    string slab = ampGainString(igai) + " mV/fC";
    string slabc = slab + " collection";
    string slabi = slab + " induction";
    legLabels.push_back(slabc);
    legLabels.push_back(slabi);
  }
  // Create legend.
  float yleg1 = 0.12;
  float yleg2 = yleg1 + 0.045*(legLabels.size()+0.5);
  float xleg1 = 0.12;
  float xleg2 = 0.45;
  TLegend* pleg = new TLegend(xleg1, yleg1, xleg2, yleg2);
  pleg->SetFillStyle(0);
  pleg->SetBorderSize(0);
  pleg->SetMargin(0.10);  // Fraction used for symbol
  for ( int igra=0; igra<legGraphs.size(); ++igra ) {
    pleg->AddEntry(legGraphs[igra], legLabels[igra].c_str(), "p");
  }
  // Plot integrated noise.
  intMan.add(pleg);
  intMan.setRangeX(0, xmax);
  intMan.setRangeY(0, 1200);
  intMan.addAxis();
  intMan.setGridY();
  string fnout = "noiseVsShapingInt.png";
  intMan.print(fnout);
  cout << myname << "Printed " << fnout << endl;
  // Plot sample noise.
  float dxleg = 0.0;
  pleg->SetX1(xleg1 + dxleg);
  pleg->SetX2(xleg2 + dxleg);
  dxleg = 0.60;
  pleg->SetX1(xleg1 + dxleg);
  pleg->SetX2(xleg2 + dxleg);
  float dyleg = 0.40;
  pleg->SetY1(yleg1 + dyleg);
  pleg->SetY2(yleg2 + dyleg);
  samMan.add(pleg);
  samMan.setRangeX(0, xmax);
  samMan.setRangeY(0, 300);
  samMan.addAxis();
  samMan.setGridY();
  fnout = "noiseVsShapingSam.png";
  samMan.print(fnout);
  cout << myname << "Printed " << fnout << endl;
  // Plot ENC.
  dxleg = 0.0;
  pleg->SetX1(xleg1 + dxleg);
  pleg->SetX2(xleg2 + dxleg);
  dyleg = 0.0;
  pleg->SetY1(yleg1 + dyleg);
  pleg->SetY2(yleg2 + dyleg);
  encMan.add(pleg);
  encMan.setRangeX(0, xmax);
  encMan.setRangeY(0, 400);
  encMan.addAxis();
  encMan.setGridY();
  fnout = "noiseVsShapingEnc.png";
  encMan.print(fnout);
  cout << myname << "Printed " << fnout << endl;
}

void roiNoise(int iopt =1) {
  const string myname = "roiNoise: ";
  if ( iopt == 1 ) return roiNoiseVsGain();
  if ( iopt == 2 ) return roiNoiseVsShaping();
  cout << myname << "Invalid option: " << iopt << endl;
}
