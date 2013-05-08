#include "smear.C"
#include "truthMatch.C"
#include "commonPlotting.C"

plotDelphes3(TString inputName){

  gSystem->Load("libDelphes");

  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputName.Data());
  
  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();
  
  // Get pointers to branches used in this analysis
  TClonesArray *b_Jet = treeReader->UseBranch("Jet");
  TClonesArray *b_Electron = treeReader->UseBranch("Electron");
  TClonesArray *b_Muon = treeReader->UseBranch("Muon");
  TClonesArray *b_MET = treeReader->UseBranch("MissingET");
  TClonesArray *b_Truth = treeReader->UseBranch("Particle");

  TString outputName(inputName);
  outputName="test.root";
  TFile* output=new TFile(outputName,"RECREATE");

  // Loop over all events
  for(Int_t entry = 0; entry < numberOfEntries; ++entry)
    {
      if(entry==maxEvents) break;
      if(entry%10000==0)cout<<entry<<endl;

      // Load selected branches with data from specified event
      treeReader->ReadEntry(entry);

      if(b_MET->GetEntries() > 0){
	MissingET* met = (MissingET*) b_MET->At(0);
	h_MET->Fill(met->MET);
      }

      int nJets=0, nBJets=0;
      for(int jetNo=0; jetNo<b_Jet->GetEntries(); jetNo++){
	Jet *jet=(Jet*) b_Jet->At(jetNo);
	if(jet->PT>jet_pT_min && fabs(jet->Eta)<jet_eta_max){
	  nJets++;
	  h_jet_pT->Fill(jet->PT);
	  h_jet_eta->Fill(jet->Eta);
	    
	  if(jet->BTag){
	    nBJets++;
	    h_bJet_pT->Fill(jet->PT);
	    h_bJet_eta->Fill(jet->Eta);
	  }
	}
      }
      h_jet_mult->Fill(nJets);
      h_bJet_mult->Fill(nBJets);

      int nElectrons=0;
      for(int electronNo=0; electronNo<b_Electron->GetEntries(); electronNo++){
	Electron *electron=(Electron*) b_Electron->At(electronNo);
	if(electron->PT>electron_pT_min && fabs(electron->Eta)<electron_eta_max){
	  nElectrons++;
	  h_electron_pT->Fill(electron->PT);
	  h_electron_eta->Fill(electron->Eta);
	}
      }
      h_electron_mult->Fill(nElectrons);

      int nMuons=0;
      for(int muonNo=0; muonNo<b_Muon->GetEntries(); muonNo++){
	Muon *muon=(Muon*) b_Muon->At(muonNo);
	if(muon->PT>muon_pT_min && fabs(muon->Eta)<muon_eta_max){
	  nMuons++;
	  h_muon_pT->Fill(muon->PT);
	  h_muon_eta->Fill(muon->Eta);
	}
      }
      h_muon_mult->Fill(nMuons);

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
    }
  
  writeOutput();
}
