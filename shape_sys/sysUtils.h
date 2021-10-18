#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <iterator>
#include <vector>
#include <stdio.h>
#include <dirent.h>

#include <TTreeFormula.h>
#include "TMath.h"
#include "TStopwatch.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TChain.h"
#include "TROOT.h"
#include "THStack.h"
#include "TLegend.h"
#include "TF1.h"

#include "TMath.h"
#include "TStopwatch.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TChain.h"
#include "TROOT.h"
#include "THStack.h"
#include "TLegend.h"
#include "TF1.h"
#include "TLorentzVector.h"

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "ROOT/RDF/RInterface.hxx"
#include "TCanvas.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TStyle.h"

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
        return;
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

string baseCuts = "";
string blindCut = "";

void getMyyHist(map<TString, TH1F*> &hists, int mcID, TString syst, std::vector<std::string> fpaths, map<TString, pair<float, float>> OObins = {{"full", make_pair(-99999, 99999)}}, bool isVBF = false, map<TString, double> d_tildes = {{"SM", 0.}}, std::map<TString, std::string> cats = {{"allBDTCats", ""}}){
  TString id = Form("%i", mcID);

  string baseCuts_sys = TString(baseCuts.data()).ReplaceAll("??", syst+"_").Data(); cout<<endl<<baseCuts_sys<<endl;

  map<TString, TH1F*> hists_tmp;

  for(auto cat : cats){
    for(auto d = d_tildes.begin(); d != d_tildes.end(); d++){
      hists[syst+"_"+d->first+"_"+cat.first] = new TH1F(syst+"_m_yy_"+d->first+"_"+cat.first, "", 550, 105000,160000);
      for(auto camp : lumi){
         hists_tmp[camp.first+"_"+syst+"_"+d->first+"_"+cat.first] = new TH1F(syst+"_m_yy_"+camp.first+"_"+d->first+"_"+cat.first, "", 550, 105000,160000);
      }
    }
  }

  for(auto camp = lumi.begin(); camp != lumi.end(); camp++){
    cout<<camp->first<<endl;

    TString fpath = "";

    for(auto f : fpaths){
      TString filepath = f.data();
      if(f.find(camp->first) == std::string::npos || f.find(Form("%i", mcID)) == std::string::npos) continue; // to select mc name
      fpath = filepath; cout<<fpath<<endl;
    }

    if(fpath == "") cout<<"file path not found for "<<mcID<<", "<<camp->first<<endl;

    TFile *f_w = new TFile(fpath, "read");

    double sumOfWeights = getSumOfWeights(mcID, f_w); cout<<sumOfWeights<<endl;

    TChain ch(syst, syst);
    ch.Add(fpath);

    ROOT::RDataFrame df(ch, {Form("%s_m_yy", syst.Data())});

    auto df_base = df.Filter(baseCuts_sys);

    float campLumi = camp->second;
    auto df_wt = df_base.Define("wt", [&sumOfWeights, &campLumi](float xsec, float weight, float wjvt, float wfjvt){ return (float) (campLumi*xsec*weight*wjvt*wfjvt/sumOfWeights); }, {Form("%s_xsec_kF_eff", syst.Data()), Form("%s_weight", syst.Data()), Form("%s_weightJvt_30", syst.Data()), Form("%s_weightFJvt_30", syst.Data())});

    for(auto cat : cats){
      string catCuts_sys = TString(cat.second.data()).ReplaceAll("??", syst+"_").Data();
      auto df_cat = df_wt.Filter(catCuts_sys);
      for(auto d = d_tildes.begin(); d != d_tildes.end(); d++){
        double d_tilde = d->second;
        TString tsCampSysDtildeCat = camp->first+"_"+syst+"_"+d->first+"_"+cat.first;
        auto df_wd = df_cat.Define("wd", [&d_tilde, &isVBF](float w1){ return (float) (isVBF? (1. + w1*d_tilde) : 1.); }, {Form("%s_WeightDtilde1", syst.Data())});

        df_wd.Foreach([&hists_tmp, &tsCampSysDtildeCat](float m_yy, float wt, float wd){ hists_tmp[tsCampSysDtildeCat]->Fill(m_yy, wt*wd); }, {Form("%s_m_yy", syst.Data()), "wt", "wd"});
      }// end d_tilde
    }// end cat
    //}// end fill
    delete f_w;
  }// end camp

  for(auto cat : cats){
    for(auto d : d_tildes){
      TString tsSysDtildeCat = syst+"_"+d.first+"_"+cat.first;
      hists[tsSysDtildeCat]->Clear();
      for(auto camp : lumi){
        hists[tsSysDtildeCat]->Add(hists_tmp[camp.first+"_"+tsSysDtildeCat]);
      }
    }
  }
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
