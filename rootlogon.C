{
  string loadfcl = "load-ib5.fcl";
  if ( gSystem->Getenv("ROOTFCL") != nullptr ) {
    loadfcl = gSystem->Getenv("ROOTFCL");
  }
  cout << "Fcl loaded from " << loadfcl << endl;
  gROOT->ProcessLine("ArtServiceHelper::load(loadfcl)");
  gROOT->ProcessLine("DuneToolManager* ptm = DuneToolManager::instance(loadfcl)");
  //gInterpreter->AddIncludePath("/dune/data2/users/dladams/data/protodune/proc/install/dev01/dunenoise");
  //gROOT->ProcessLine("#include \"art/Utilities/make_tool.h\"");
  //gROOT->ProcessLine("#include \"dunenoise/ApaWirePlot.h\"");
  //gROOT->ProcessLine(".L $DUNENOISE_DIR/root/ibPlotWires.C");
  //gROOT->ProcessLine(".L $DUNENOISE_DIR/root/ibDrawWireNoise.C");
  gROOT->ProcessLine(".L decorateNoiseVsTime.C");
  //gROOT->ProcessLine(".L fixNoiseVsChanPlot.C");
  gROOT->ProcessLine(".L drawRois.C");
  gROOT->ProcessLine(".L makeChain.C");
  gROOT->ProcessLine(".L drawSigstren.C");
  gROOT->ProcessLine(".L langaus.C");
  gROOT->ProcessLine(".L drawLifetime.C");
  gROOT->ProcessLine(".L $DUNECECALIB_DIR/root/calplots.C");
  gROOT->ProcessLine(".L $DUNECECALIB_DIR/root/drawGainDist.C");
}
