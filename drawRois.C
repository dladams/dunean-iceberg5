// drawRois.C
//
// David Adams
// March 2021
//
// Draw the ROIs (channel vs. tick) in the ROI tree.

int drawRois(TTree* ptree, string ssel ="", string sview ="x") {
  string myname = "drawRois: ";
  using Index = unsigned int;
  float tmin = 0.0;
  float tmax = 2184;
  Index cmin = 0;
  Index cmax = 0;
  Index nt = tmax - tmin;
  if ( sview == "x" ) {
    cmin = 800;
    cmax = 1280;
  } else if ( sview == "uv" ) {
    cmin = 0;
    cmax = 800;
  } else {
    cout << myname << "Invalid view: " << sview << endl;
    return 1;
  }
  // Fetch run list.
  Index npt = ptree->Draw("run", ssel.c_str(), "goff");
  if ( npt == 0 ) {
    cout << "No ROIs found for " << ssel << endl;
    return 2;
  }
  cout << "Selected ROI count: " << npt << endl;
  set<Index> runs;
  for ( Index ient=0; ient<npt; ++ient ) {
    runs.insert(ptree->GetV1()[ient]);
  }
  Index run1 = *runs.begin();
  Index run2 = *runs.rbegin();
  string srun1 = to_string(run1);
  string srun2 = to_string(run2);
  string sruns, sfruns;
  if ( run2 != run1 ) {
    sruns = "runs " + srun1 + "-" + srun2;
    sfruns = "runs" + srun1 + "-" + srun2;
  } else {
    sruns = "run " + srun1;
    sfruns = "run" + srun1;
  }
  // Build name and title.
  string hnam = "hroi" + sview;
  string sttl = "ROIs " + sruns;
  if ( ssel.size() ) sttl += " (" + ssel + ")";
  sttl += "; tick; Channel";
  TH2* phax = new TH2F(hnam.c_str(), sttl.c_str(), 10, tmin, tmax, 10, cmin, cmax);
  phax->SetStats(0);
  npt = ptree->Draw("isam:channel", ssel.c_str(), "goff");
  if ( npt == 0 ) {
    cout << "NROIs found for " << ssel << endl;
    return 2;
  }
  TGraph* pg = new TGraph(npt, ptree->GetV1(), ptree->GetV2());
  pg->SetMarkerStyle(20);
  pg->SetMarkerSize(0.3);
  bool show = false;
  if ( show ) {
    phax->Draw();
    pg->Draw("P");
  }
  TPadManipulator* ppad = new TPadManipulator(1400, 1000);
  ppad->add(phax);
  ppad->add(pg, "P");
  ppad->addAxis();
  ppad->centerAxisTitles();
  string sfsel = ssel;
  StringManipulator sm(sfsel, false);
  sm.replace(">=", "ge");
  sm.replace(">", "gt");
  sm.replace("<=", "le");
  sm.replace("<", "lt");
  sm.replace("==", "eq");
  sm.replace(".", "p");
  string fnam = "roicht-" + sview + "_" + sfruns;
  if ( sfsel.size() ) fnam += "_" + sfsel;
  fnam += ".png";
  cout << myname << "Writing " << fnam << endl;
  ppad->print(fnam);
  return 0;
}
