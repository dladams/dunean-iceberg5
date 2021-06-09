TChain* makeChain(string sfin, bool quiet =false) {
  ifstream fin(sfin.c_str());
  string fnam;
  TChain* pcha = new TChain("adcrois");
  while ( fin ) {
    fin >> fnam;
    if ( ! quiet ) cout << fnam << endl;
    pcha->Add(fnam.c_str());
  }
  return pcha;
}
