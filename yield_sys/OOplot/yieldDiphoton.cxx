#include "../../shape_sys/sysUtils.h"

void yieldDiphoton(){
  char *cf_cats = (char*)"../../../nom_WS/cats.cfg";
  map<TString, string> catCuts;
  getCatCuts(cf_cats, catCuts); for(auto c : catCuts) cout<<c.first<<c.second<<endl;

  TChain ch("output", "output");
  ch.Add("../../../bkgTemplate/h026_364352.diphoton_AF2_slim.root");

  ROOT::RDataFrame df(ch, {"m_yy"});

  for(auto cat : catCuts){
    TString catName = cat.first;
    string cuts = TString(cat.second.append(" && isPassed")).ReplaceAll("??", "").Data(); //cout<<cuts<<endl;
    auto df_cut = df.Filter(cuts);
    cout<<catName<<","<<*(df_cut.Sum("wt"))<<endl;
  }
}
