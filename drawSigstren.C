// drawSigstren.C
//
// David Adams
// March 2021
//
// Draw the signal strength.

int drawSigstren(TTree* ptree, string svarin ="qroi", string ssel ="", string outpre ="", string slabIn ="", string sflab ="") {
  string myname = "drawSigstren: ";
  using Index = unsigned int;
  float tmin = 0.0;
  float tmax = 2184;
  Index cmin = 0;
  Index cmax = 0;
  Index nt = tmax - tmin;
  // Extract variable and action fields.
  string sxmax;
  string stau;
  string st0;
  vector<string> sacts = StringManipulator(svarin, false).split(",");
  string svar = sacts[0];
  for ( unsigned int iact=1; iact<sacts.size(); ++iact ) {
    string sact = sacts[iact];
    if ( sact.substr(0,4) == "xmax" ) sxmax = sact.substr(4);
    else if ( sact.substr(0,3) == "tau" ) stau = sact.substr(3);
    else if ( sact.substr(0,5) == "tzero" ) st0 = sact.substr(5);
    else {
      cout << myname << "ERROR: Invalid action: " << sact << endl;
      return 1;
    }
  }
  // Fetch run list.
  Index npt = ptree->Draw("run", ssel.c_str(), "goff");
  if ( npt == 0 ) {
    npt = ptree->Draw("run", "", "goff");
    if ( npt == 0 ) {
      cout << myname << "WARNING: No ROIs found." << endl;
      return 2;
    } else {
      cout << myname << "WARNING: No ROIs found for " << ssel << endl;
    }
  }
  cout << myname << "Selected ROI count: " << npt << endl;
  set<Index> runs;
  for ( Index ient=0; ient<npt; ++ient ) {
    runs.insert(ptree->GetV1()[ient]);
  }
  string sruns = "No runs";
  string sfruns = "noruns";
  if ( runs.size() ) {
    Index run1 = *runs.begin();
    Index run2 = *runs.rbegin();
    string srun1 = to_string(run1);
    string srun2 = to_string(run2);
    if ( run2 != run1 ) {
      sruns = "runs " + srun1 + "-" + srun2;
      sfruns = "runs" + srun1 + "-" + srun2;
    } else {
      sruns = "run " + srun1;
      sfruns = "run" + srun1;
    }
  }
  // Build lifetime correction.
  string scorfac;
  if ( stau.size() ) {
    if ( st0.size() == 0 ) {
      cout << myname << "ERROR: Please specify t0 with tzeroTTT" << endl;
      return 1;
    }
    scorfac = "exp((isam-" + st0 + ")/" + stau + ".0)";
  }
  // Build name and title.
  string hnam = "hsgs";
  string sttl;
  string sdesc;
  string sxlab = "ROI charge [ke]";
  int nx = 100;
  double xmin= 0.0;
  double xmax;
  bool showQmean = false;
  if ( svar == "qroi" ) {
    sttl = "Signal strength";
    if ( stau.size() ) {
      svar = svar + "*" + scorfac;
      sttl = "Lifetime corrected signal strength";
    }
    sdesc = "sigstren";
    sxlab = "ROI charge [ke]";
    nx = 100;
    xmax = 100.0;
    showQmean = true;
  } else if ( svar.substr(0,4) == "hmax" ) {
    sttl = "Signal height";
    if ( stau.size() ) {
      svar = svar + "*" + scorfac;
      sttl = "Lifetime corrected signal height";
    }
    sdesc = "sigheight";
    sxlab = "ROI height [ke]";
    nx = 100;
    xmax = 10.0;
    if ( svar.size() > 5 && svar[4] == ':' ) {
      string srem = svar.substr(5);
      svar = "hmax";
      istringstream sin(srem);
      sin >> xmax;
    }
    showQmean = true;
  } else if ( svar == "isam" ) {
    sttl = "Signal time";
    sdesc = "sigtick";
    sxlab = "ROI time sample [tick]";
    nx = 100;
    xmin =  400.0;
    xmax = 1400.0;
  } else {
    cout << myname << "ERROR: Invalid vaiable specifier: " << svar << endl;
    return 1;
  }
  cout << myname << "Plot variable: " << svar << endl;
  if ( sxmax.size() ) {
    StringManipulator smxmax(sxmax, true);
    xmax = smxmax.toFloat(xmax);
  }
  sttl += " " + sruns;
  if ( ssel.size() ) sttl += " (" + ssel + ")";
  sttl += ";" + sxlab + "; Number of ROIs";
  //TH1* phsgn = new TH1F(hnam.c_str(), sttl.c_str(), 75, 0, 150);
  TH1* phsgn = new TH1F(hnam.c_str(), sttl.c_str(), nx, xmin, xmax);
  phsgn->SetStats(0);
  phsgn->SetLineWidth(2);
  string dvar = svar + ">>" + hnam;
  npt = ptree->Draw(dvar.c_str(), ssel.c_str(), "goff");
  float xmean = phsgn->GetMean();
  int nroi = phsgn->GetEntries();
  cout << myname << "Mean signal strength: " << xmean << endl;
  vector<string> slabs;
  // Create labels.
  if ( slabIn.size() ) slabs.push_back(slabIn);
  slabs.push_back("N_{ROI}: " + to_string(nroi));
  if ( showQmean ) {
    ostringstream sslab;
    sslab.precision(1);
    sslab << "<Q> : " << fixed << xmean << " ke";
    slabs.push_back(sslab.str());
  }
  if ( stau.size() ) {
    slabs.push_back("#tau_{e} = " + stau + " ticks");
  }
  // Create plot.
  TPadManipulator* ppad = new TPadManipulator(1400, 1000);
  ppad->add(phsgn, "hist");
  ppad->addAxis();
  ppad->centerAxisTitles();
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
  // Build output file name.
  string sfsel = ssel;
  StringManipulator sm(sfsel, false);
  sm.replace(">=", "ge");
  sm.replace(">", "gt");
  sm.replace("<=", "le");
  sm.replace("<", "lt");
  sm.replace("==", "eq");
  sm.replace(".", "p");
  sm.replace("&&", "-and-");
  string fnam = outpre + sdesc + sflab + "_" + sfruns;
  if ( sfsel.size() ) fnam += "_" + sfsel;
  if ( stau.size() ) fnam += "_tau" + stau;
  if ( sxmax.size() ) fnam += "_xmax" + sxmax;
  fnam += ".png";
  cout << myname << "Writing " << fnam << endl;
  ppad->print(fnam);
  return 0;
}

int drawSigstren(string sdir, string svar ="qroi", string ssel ="", string sflab ="") {
  string myname = "drawSigstren: ";
  string sfin = sdir + "/adcrois.root";
  TFile* pfil = TFile::Open(sfin.c_str());
  if ( pfil == nullptr || ! pfil->IsOpen() ) {
    cout << myname << "Unable to find tree file: " << sfin << endl;
    return 1;
  }
  TTree* ptre = dynamic_cast<TTree*>(pfil->Get("adcrois"));
  if ( pfil == nullptr || ! pfil->IsOpen() ) {
    cout << myname << "Unable to find tree." << sfin << endl;
    return 2;
  }
  return drawSigstren(ptre, svar, ssel, sdir + "/", "", sflab);
}

int drawSigstrenChain(string slab, string svar, string ssel ="") {
  string sfin = "chains/adcrois-" + slab + ".txt";
  string myname = "drawSigstren: ";
  TChain* pchn = makeChain(sfin);
  cout << myname << "Chain file name: " << sfin << endl;
  unsigned int nent = pchn->GetEntries();
  cout << myname << "Entry count: " << nent << endl;
  string sflab = "-" + slab;
  if ( nent ) return drawSigstren(pchn, svar, ssel, "plotout/", "Iceberg " + slab, sflab);
  return 1;
}
