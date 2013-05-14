/*
  
  Analysis application to loop over Delphes output event files
  Currently set up with Snowmass 2013 fastsim studies in mind

  Authors:
           Gena Kukartsev
           John Stupak

  2013

 */



#include <iostream>
#include "smear.C"
#include "truthMatch.C"
#include "commonPlotting.C"
#include "TChain.h"
#include "ExRootAnalysis/ExRootTreeReader.h"
#include "TClonesArray.h"
#include "classes/DelphesClasses.h"
#include "interface/LjmetEventContent.h"



// cut definitions
int gMinLeptons = 2;
int gMaxLeptons = 1000;
int gMinJets    = 0; // all jets
int gMaxJets    = 1000; // all jets
int gMinBJets   = 0;
double gMinMet    =  0.0;
double gMinJetPt  = 20.0;
double gMaxJetEta =  3.0;
double gMinLeptonPt  = 20.0;
double gMaxLeptonEta =  3.0;
// need HT, ST, Mlb


int main(int argc, char * argv[]){

  std::string legend = "[Snowmass]: ";

  std::cout << legend << std::endl;
  std::cout << legend << "Welcome to Snowmass!" << std::endl;
  std::cout << legend << std::endl;


  std::string inputFileName = "/home/data/snowmass/AZh_14TEV_bb_1000_NoPileUp.root";


  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFileName.c_str());


  // Create object of class ExRootTreeReader
  ExRootTreeReader * treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();

  
  // Get pointers to branches used in this analysis
  TClonesArray *b_Jet = treeReader->UseBranch("Jet");
  TClonesArray *b_Electron = treeReader->UseBranch("Electron");
  TClonesArray *b_Muon = treeReader->UseBranch("Muon");
  TClonesArray *b_MET = treeReader->UseBranch("MissingET");
  TClonesArray *b_Truth = treeReader->UseBranch("Particle");

  TString outputName(inputFileName);
  outputName="test.root";
  TFile* output=new TFile(outputName,"RECREATE");


  // output tree
  //TTree * pOutputTree = new TTree("snowmass", "snowmass", 64000000);
  TTree outputTree("snowmass", "snowmass", 64000000);


  // convenient tree and histogram handling
  LjmetEventContent ec;
  ec.SetTree(&outputTree);


  // book histograms here
  ec.SetHistogram("provenance", "nevents", 100, 0, 100000);


  // create booked histograms
  std::map<std::string,std::map<std::string,LjmetEventContent::HistMetadata> > & mh = ec.GetHistMap();
  std::map<std::string,std::map<std::string,LjmetEventContent::HistMetadata> >::iterator iMod;
  std::map<std::string,LjmetEventContent::HistMetadata>::iterator iHist;
  TDirectory * rootDir = outputTree.GetDirectory();
  for (iMod=mh.begin();iMod!=mh.end();++iMod){
    for (iHist=iMod->second.begin();iHist!=iMod->second.end();++iHist){
      std::cout << legend
		<< "Creating " << iMod->first << "/"
		<< iHist->second.GetName() << std::endl;
      TH1 * _hist = new TH1F(iHist->second.GetName().c_str(),
			     iHist->second.GetName().c_str(),
			     iHist->second.GetNBins(),
			     iHist->second.GetXMin(),
			     iHist->second.GetXMax() );
      iHist->second.SetHist( _hist );
    }
  }
  rootDir->cd();


  //--------------------------------------------------------------
  //
  // Event loop
  //
  //--------------------------------------------------------------
  Int_t nevents = chain.GetEntries();
  for(Int_t entry = 0; entry < nevents; ++entry)
    {
      if(entry==maxEvents) break;
      if(entry%10000==0) std::cout << entry << std::endl;
      

      // some book keeping
      ec.SetValue("event_number", entry);
      ec.SetHistValue("provenance", "nevents", (float)entry);

      
      // Load selected branches with data from specified event
      treeReader->ReadEntry(entry);
      

      // MET
      if(b_MET->GetEntries() > 0){
	MissingET* met = (MissingET*) b_MET->At(0);
	double _met = met->MET;
	h_MET->Fill(_met);
	//ec.SetValue("met", met->MET);

	if (_met<gMinMet) continue;

      }
      else continue; // no MET??


      // jets
      int nJets=0, nBJets=0;
      int nSelJets=0, nSelBJets=0;
      for(int jetNo=0; jetNo<b_Jet->GetEntries(); jetNo++){
	Jet *jet=(Jet*) b_Jet->At(jetNo);
	if(jet->PT>jet_pT_min && fabs(jet->Eta)<jet_eta_max){
	  nJets++;

	  double _jetPt  = jet->PT;
	  double _jetEta = jet->Eta;
	  h_jet_pT ->Fill(_jetPt);
	  h_jet_eta->Fill(_jetEta);
	  
	  // jet selector
	  if (_jetPt<gMinJetPt || fabs(_jetEta)>gMaxJetEta) continue;
	  ++nSelJets;
	    
	  if(jet->BTag){
	    nBJets++;
	    h_bJet_pT->Fill(jet->PT);
	    h_bJet_eta->Fill(jet->Eta);

	  // b jet selector
	  if (_jetPt<gMinJetPt || fabs(_jetEta)>gMaxJetEta) continue;
	  ++nSelBJets;

	  }
	}
      }
      h_jet_mult->Fill(nJets);
      h_bJet_mult->Fill(nBJets);

      ec.SetValue("nSelJets", nSelJets);
      ec.SetValue("nSelBJets", nSelBJets);

      // jet cuts
      if (nSelJets<gMinJets) continue;      
      if (nSelBJets<gMinBJets) continue;      


      // electrons
      int nElectrons=0;
      int nSelElectrons=0;
      for(int electronNo=0; electronNo<b_Electron->GetEntries(); electronNo++){
	Electron *electron=(Electron*) b_Electron->At(electronNo);
	if(electron->PT>electron_pT_min && fabs(electron->Eta)<electron_eta_max){
	  nElectrons++;
	  double _electronPt  = electron->PT;
	  double _electronEta = electron->Eta;
	  h_electron_pT ->Fill(_electronPt);
	  h_electron_eta->Fill(_electronEta);

	  // electron selector
	  if (_electronPt<gMinLeptonPt || fabs(_electronEta)>gMaxLeptonEta) continue;
	  ++nSelElectrons;

	}
      }
      h_electron_mult->Fill(nElectrons);
      ec.SetValue("nSelElectrons", nSelElectrons);


      // muons
      int nMuons=0;
      int nSelMuons=0;
      for(int muonNo=0; muonNo<b_Muon->GetEntries(); muonNo++){
	Muon *muon=(Muon*) b_Muon->At(muonNo);
	if(muon->PT>muon_pT_min && fabs(muon->Eta)<muon_eta_max){
	  nMuons++;
	  double _muonPt  = muon->PT;
	  double _muonEta = muon->Eta;
	  h_muon_pT ->Fill(_muonPt);
	  h_muon_eta->Fill(_muonEta);

	  // muon selector
	  if (_muonPt<gMinLeptonPt || fabs(_muonEta)>gMaxLeptonEta) continue;
	  ++nSelMuons;

	}
      }
      h_muon_mult->Fill(nMuons);
      ec.SetValue("nSelMuons", nSelMuons);
      ec.SetValue("nSelLeptons", nSelElectrons+nSelMuons);


      // lepton cuts
      if (nSelElectrons<gMinLeptons && nSelMuons<gMinLeptons) continue;


      // MC truth
      int nGenMuons=0, nTMRMuonsIDIso=0;
      for(int genParticleNo=0; genParticleNo<b_Truth->GetEntries(); genParticleNo++){
	GenParticle *particle=(GenParticle*) b_Truth->At(genParticleNo);
	if (particle->Status==3){
	  if (abs(particle->PID)==13){
	    nGenMuons++;
	    h_genMuon_pT->Fill(particle->PT);
	    h_genMuon_eta->Fill(fabs(particle->Eta));
	    h_genMuon_eta_pT->Fill(fabs(particle->Eta),particle->PT);
	      
	    for(int muonNo=0; muonNo<b_Muon->GetEntries(); muonNo++){
	      Muon *muon=(Muon*) b_Muon->At(muonNo);
	      if(truthMatch(1,muon->Eta,muon->Phi,1,particle->Eta,particle->Phi)){
		nTMRMuonsIDIso++;
		h_TMRMuon_ID_Iso_pT->Fill(muon->PT);
		h_TMRMuon_ID_Iso_eta->Fill(fabs(muon->Eta));
		h_TMRMuon_ID_Iso_eta_pT->Fill(fabs(muon->Eta),muon->PT);
		break;  //To prevent 2 reco mu per truth mu                                                                                                                    
	      }
	    }
	  }
	}
      }

      h_genMuon_mult->Fill(nGenMuons);
      h_TMRMuon_ID_Iso_mult->Fill(nTMRMuonsIDIso);
      
      
      // fill tree branches and histograms
      ec.Fill();

    }
  

  // save histograms that are not in the event content object
  writeOutput();


  // cleanup
  delete treeReader;


  return 0;
}
