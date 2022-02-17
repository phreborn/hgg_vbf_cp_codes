#include "../../shape_sys/sysUtils.h"

void getDataYields(){
  char *cf_cats = (char*)"../../../nom_WS/cats.cfg";
  map<TString, string> catCuts;
  getCatCuts(cf_cats, catCuts); for(auto c : catCuts) cout<<c.first<<c.second<<endl;

  ofstream of("data_observed.txt");
  //ofstream of("data_observed_SR.txt");
  if(!of){
    of.close();
    cout<<"error can't open file for record"<<endl;
  }

  for(auto cat : catCuts){
    TString catName = cat.first;
    TFile *f = new TFile(Form("/scratchfs/atlas/huirun/atlaswork/VBF_CP/WSBuilder/xmlAnaWSBuilder/run/Input/data/tree_data_OO_%s.root", catName.Data()), "read");
    TTree *t = (TTree*) f->Get("CollectionTree");
    float yield = t->GetEntries();
    //float yield = t->GetEntries("m_yy/1000>118&&m_yy/1000<132");
    of<<catName<<":"<<yield<<endl;
  }
}
