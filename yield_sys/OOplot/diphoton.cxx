void diphoton(){

  std::vector<TString> bdtCats = {"TT", "TL", "LT", "LL"};

  std::map<int, TString> bins;
  bins[1] = "-99:-2";
  bins[2] = "-2:-1";
  bins[3] = "-1:0";
  bins[4] = "0:1";
  bins[5] = "1:2";
  bins[6] = "2:99";

  std::vector<TString> vcat;
  std::vector<float> vn;

  ifstream file;
  file.open("yy.csv");
  if( ! file.is_open())
  {
      cout<<"can not open file! "<<"yy.csv"<<endl;
      return;
  }
  char tmp[1000];
  while(!file.eof()){
    file.getline(tmp,1000);
    string line(tmp);
    size_t ptmp;

    size_t pos = line.find(','); std::cout<<line.substr(0,pos)<<std::endl;
    if(pos!=string::npos && line != "") vcat.push_back(line.substr(0,pos).data());
    else continue;

    ptmp = pos;
    pos = line.find(',', pos+1); cout<<line.substr(ptmp+1, pos-ptmp-1)<<endl;;
    //if(pos!=string::npos && line != "") vn.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
    if(line != "") vn.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
    else vn.push_back(0.);
  }

  for(auto c : bdtCats){
    TString cat = c;

    TFile *fout = new TFile(Form("diphoton_oo_hists_%s.root", cat.Data()), "recreate");
    TH1F *hoo = new TH1F("hoo", "", 6, -3, 3);

    for(auto b : bins){
      TString bname = b.second;
      int bnum = b.first;
      TString bin = TString(Form("b%i", bnum));

      float yield = 0.;
      for(int i = 0; i < vcat.size(); i++){
        if(vcat[i] == cat+"_"+bin){
          yield = vn[i];
          cout<<"match: "<<cat+"_"+bin<<" "<<yield<<endl;
        }
      }
      hoo->SetBinContent(bnum, yield);
      hoo->GetXaxis()->SetBinLabel(bnum, bname);
    }
    hoo->Write();
  }
}
