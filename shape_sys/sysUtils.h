#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <iterator>
#include <vector>
#include <stdio.h>
#include <dirent.h>


using namespace std;

// directory scanner
int fileNameFilter(const struct dirent *cur) {
    std::string str(cur->d_name);
    if (str.find("..") != std::string::npos) {
        return 0;
    }
    return 1;
}

std::vector<std::string> getDirBinsSortedPath(std::string dirPath) {
    struct dirent **namelist;
    std::vector<std::string> ret;
    int n = scandir(dirPath.c_str(), &namelist, fileNameFilter, alphasort);
    if (n < 0) {
        return ret;
    }
    for (int i = 0; i < n; ++i) {
        std::string filePath(namelist[i]->d_name);
        ret.push_back(filePath);
        free(namelist[i]);
    };
    free(namelist);
    return ret;
}

// a config file reader
static bool readConfigFile(const char * cfgfilepath, const string & key, string & value)
{
    fstream cfgFile;
    cfgFile.open(cfgfilepath);
    if( ! cfgFile.is_open())
    {
        cout<<"can not open cfg file!"<<endl;
        return false;
    }
    char tmp[1000];
    while(!cfgFile.eof())
    {
        cfgFile.getline(tmp,1000);
        string line(tmp);
        size_t pos = line.find(':');
        //if(pos==string::npos) return false;
        if(pos==string::npos) continue;
        string tmpKey = line.substr(0,pos);
        if(key==tmpKey)
        {
            value = line.substr(pos+1);
        }
    }
    return false;
}

void readList(TString file, std::vector<TString> &vec){
    fstream listFile;
    listFile.open(file);
    if( ! listFile.is_open())
    {
        cout<<"can not open list file!"<<endl;
        return false;
    }
    char tmp[1000];
    while(!listFile.eof())
    {
        listFile.getline(tmp,1000);
        string line(tmp);
        if(line!="") vec.push_back(line.data());
    }
}


TString getMCSampleName(int mcID){
  string name;
  //cout<<Form("SampleName.%d", mcID)<<endl;
  readConfigFile("../MCSamples.config", Form("SampleName.%d", mcID), name);
  while(name.find(" ")!=std::string::npos) { name.replace(name.find(" "), 1, ""); }
  return name.data();
}

TH1F *getCutFlowHist(int mcID, TFile* file){
  TString suffix = "_noDalitz_weighted";
  TString cutFlowName = Form("CutFlow_%s%s", getMCSampleName(mcID).Data(), suffix.Data());
  //cout<<cutFlowName<<endl;
  TH1F *cutFlow = (TH1F*) file->Get(cutFlowName);
  return cutFlow;
}

double getSumOfWeights(int mcID, TFile* file){
  double NxAOD = getCutFlowHist(mcID, file)->GetBinContent(1);
  double NDxAOD = getCutFlowHist(mcID, file)->GetBinContent(2);
  double WDxAOD = getCutFlowHist(mcID, file)->GetBinContent(3);

  double weightSum = WDxAOD*NxAOD/NDxAOD;
  cout<<"xAOD, DxAOD, allEvt: "<<NxAOD<<", "<<NDxAOD<<", "<<WDxAOD<<endl;
  return weightSum;
}

void getSysList(TTree *tree, TString anchorVar, std::vector<TString> &sysList){
  TObjArray *branches = tree->GetListOfBranches(); 
  
  for(int i = 0; i < branches->GetEntries(); i++){
    TBranch *branch = (TBranch *)branches->At(i);
    TString bname = branch->GetName();

    if (!bname.Contains(anchorVar)) { continue; }

    bname.ReplaceAll("."+anchorVar, "");

    if (bname.IsNull()) { continue; }
    //if (bname=="Nominal") { continue; } //cout<<bname<<endl;

    sysList.push_back(bname); cout<<bname<<endl;
  }
}

void getSysList(TString file, std::vector<TString> &sysList){
  TFile *f1 = TFile::Open(file);
  for(auto k : *f1->GetListOfKeys()) { // refer to io/loopdir11.C
    TKey *key = static_cast<TKey*>(k);
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TTree")) continue;
    TString treeName = k->GetName(); cout<<treeName<<endl;
    sysList.push_back(treeName);
  }
}

bool sysExistInFile(TString file, TString sysName){
  TFile *f1 = TFile::Open(file);
  for(auto k : *f1->GetListOfKeys()) { // refer to io/loopdir11.C
    TKey *key = static_cast<TKey*>(k);
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TTree")) continue;
    TString treeName = k->GetName(); cout<<treeName<<endl;
    if(treeName.Contains(sysName)) return true;
  }
  return false;
}

map<TString,float> lumi;

bool sysExistInAllFiles(vector<std::string> files, TString sysName){
  bool isExitInAllFiles = true;

  for(auto camp = lumi.begin(); camp != lumi.end(); camp++){
    TString fpath = "";
    for(auto f : files){
      TString filepath = f.data();
      if(f.find(camp->first) == std::string::npos) continue; // to select mc name
      fpath = filepath; cout<<"check if sys "<<sysName<<" exist in file : "<<fpath<<endl;
      if(!sysExistInFile(fpath, sysName)) isExitInAllFiles = false;
    }  
  }
  return isExitInAllFiles;
}

void getMyyHist(map<TString, TH1F*> &hists, int mcID, TString syst, std::vector<std::string> fpaths, map<TString, pair<float, float>> OObins = {{"full", make_pair(-99999, 99999)}}, bool isVBF = false, map<TString, double> d_tildes = {{"SM", 0.}}, std::map<TString, std::vector<float>> cats = {{"allBDTCats", {}}}){
  TString id = Form("%i", mcID);

for(auto cat : cats){
  for(auto bin = OObins.begin(); bin != OObins.end(); bin++){
    for(auto d = d_tildes.begin(); d != d_tildes.end(); d++){
      hists[syst+"_"+d->first+"_"+cat.first+"_"+bin->first] = new TH1F(syst+"_m_yy_"+d->first+"_"+cat.first+"_"+bin->first, "", 550, 105000,160000);
    }
  }
}

for(auto cat : cats){
  for(auto camp = lumi.begin(); camp != lumi.end(); camp++){
    cout<<camp->first<<endl;

    TString fpath = "";

    for(auto f : fpaths){
      TString filepath = f.data();
      if(f.find(camp->first) == std::string::npos || f.find(Form("%i", mcID)) == std::string::npos || f.find(cat.first) == std::string::npos) continue; // to select mc name
      fpath = filepath; cout<<fpath<<endl;
    }

    if(fpath == "") cout<<"file path not found for "<<mcID<<", "<<camp->first<<endl;

    TFile *f_w = new TFile(fpath, "read");

    double sumOfWeights = getSumOfWeights(mcID, f_w); cout<<sumOfWeights<<endl;

    TTree *tree = (TTree*) f_w->Get(syst);

    Int_t N_j_30,N_photon,cutflow,Category;
    Float_t m_yy,pT_y1,pT_y2,m_jj_30,DeltaEta_jj,Zepp,oo1,oo2,WeightDtilde1,WeightDtilde2,weight,xsec_kF_eff,total_weights;
    Bool_t isDalitz,isPassedIsolation,isPassedPID,isPassedTriggerMatch,isPassed;
  
    tree->SetBranchAddress(syst+"_catCoup_XGBoost_ttH", &Category);
    tree->SetBranchAddress(syst+"_isPassed", &isPassed);
    tree->SetBranchAddress(syst+"_m_yy", &m_yy);
    tree->SetBranchAddress(syst+"_N_j_30", &N_j_30);
    tree->SetBranchAddress(syst+"_m_jj_30", &m_jj_30);
    tree->SetBranchAddress(syst+"_DeltaEta_jj", &DeltaEta_jj);
    tree->SetBranchAddress(syst+"_Zepp", &Zepp);
    tree->SetBranchAddress(syst+"_oo1", &oo1);
    tree->SetBranchAddress(syst+"_oo2", &oo2);
    tree->SetBranchAddress(syst+"_weight_catCoup_XGBoost_ttH", &weight);
    tree->SetBranchAddress(syst+"_xsec_kF_eff", &xsec_kF_eff);
    tree->SetBranchAddress(syst+"_isDalitz", &isDalitz);

    if(isVBF){
      tree->SetBranchAddress(syst+"_WeightDtilde1", &WeightDtilde1);
      tree->SetBranchAddress(syst+"_WeightDtilde2", &WeightDtilde2);
    }
  
    Long64_t endentry = tree->GetEntries();
  
    for(int i = 0; i < endentry; i++){
      tree->GetEntry(i);
      if(i==endentry-1) cout<<endentry<<" events processed"<<endl;
  
      if(isDalitz==1||isPassed==0) continue;
      if(N_j_30<2) continue;
      if(m_jj_30/1000<400) continue;
      if(DeltaEta_jj>-2&&DeltaEta_jj<2) continue;
      if(Zepp>5||Zepp<-5) continue;
      if(Category<11||Category>14) continue;

      float w = camp->second*xsec_kF_eff*weight/sumOfWeights;
      float wd = 1.;

      for(auto bin = OObins.begin(); bin != OObins.end(); bin++){
        double b_l = bin->second.first;
        double b_r = bin->second.second;

        if(oo1<b_l || oo1>b_r) continue;

        for(auto d = d_tildes.begin(); d != d_tildes.end(); d++){
          double d_tilde = d->second;
          if(isVBF) wd = 1. + WeightDtilde1*d_tilde + WeightDtilde2*d_tilde*d_tilde;

          hists[syst+"_"+d->first+"_"+cat.first+"_"+bin->first]->Fill(m_yy, w*wd); // no campName in hist Key -> merge all camps
        }// end d_tilde
      }// end OO bin
    }// end fill
    delete f_w;
  }// end camp
}// end cat
}

vector<string> readInLines(const char * cfgfilepath){
    vector<string> readin;

    fstream cfgFile;
    cfgFile.open(cfgfilepath);
    if( ! cfgFile.is_open())
    {
        cout<<"can not open cfg file!"<<endl;
    }
    char tmp[1000];
    while(!cfgFile.eof())
    {
        cfgFile.getline(tmp,1000);
        string line(tmp);
        if(line.find("#") != string::npos) continue;
        if(line=="") continue;
        readin.push_back(line);
    }

    return readin;
}

map<TString, string> sepKeyValue(string cfg){
  map<TString, string> cats;

  vector<string> lines = readInLines(cfg.data());
  for(auto l : lines){
    int pos = l.find(":");
    string sKey = l.substr(0,pos);
    string sValues = l.substr(pos+1);
    TString tsKey = sKey.data();
    tsKey.ReplaceAll(" ", "");

    cats[tsKey] = sValues;
  }

  return cats;
}

void getCatCuts(string cfg, map<TString, string> &catCuts){
  catCuts = sepKeyValue(cfg);
}
