using namespace RooFit;

void asimovUncer(){

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
  std::vector<float> vsyst;
  std::vector<float> vstat;

  ifstream file;
  file.open("fitStat.log");
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

    ptmp = pos;
    pos = line.find(',', pos+1); cout<<line.substr(ptmp+1, pos-ptmp-1)<<endl;;
    if(line != "") vsyst.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
    else vsyst.push_back(0.);
  }

  //TFile *fsyst = new TFile("/scratchfs/atlas/huirun/atlaswork/VBF_CP/WSBuilder/xmlAnaWSBuilder/run/outIDPMu_allSys/out_m00.root", "read");
  //TFile *fstat = new TFile("/scratchfs/atlas/huirun/atlaswork/VBF_CP/WSBuilder/xmlAnaWSBuilder/run/outIDPMu_statOnly/out_m00.root", "read");

  //RooFitResult *rltsyst = (RooFitResult*)fsyst->Get("fitResult");

  for(auto c : bdtCats){
    TString cat = c;

    TFile *fin = new TFile(Form("VBF_oo_hists_m00_%s.root", cat.Data()), "read");
    TH1F *hin = (TH1F*)fin->Get("hoo");
    TH1F *hoo = (TH1F*)hin->Clone("hoo");

    TFile *fout = new TFile(Form("stat_VBF_oo_hists_%s.root", cat.Data()), "recreate");

    for(auto b : bins){
      TString bname = b.second;
      int bnum = b.first;
      TString bin = TString(Form("b%i", bnum));

      float muhat = 0.;
      float syst = 0.;
      for(int i = 0; i < vcat.size(); i++){
        if(vcat[i] == "mu_VBF_"+cat+"_"+bin){
          muhat = vn[i];
          syst = vsyst[i];
        }
      }
      cout<<"match: "<<cat+"_"+bin<<" "<<muhat<<" "<<syst<<endl;
      cout<<hin->GetBinContent(bnum)<<endl;
      hoo->SetBinError(bnum, syst/muhat*hin->GetBinContent(bnum));
      hoo->GetXaxis()->SetBinLabel(bnum, bname);
    }
    hoo->Write();
  }
}
