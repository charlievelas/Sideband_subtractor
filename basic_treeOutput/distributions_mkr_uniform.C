void distributions_mkr_uniform(){

    // Create outfile and tree
    TFile outfile("distributions_uniform.root", "recreate");
    TTree *tree = new TTree("tree","tree");
    Float_t mass;
    Int_t massAIndx;
    Int_t massBIndx;
    Float_t pz;
    Int_t pzAIndx;
    Int_t pzBIndx;
    TBranch *BR_mass = tree->Branch("mass",&mass,"mass/F");
    TBranch *BR_massAIndx = tree->Branch("massAIndx",&massAIndx,"massAIndx/I");
    TBranch *BR_massBIndx = tree->Branch("massBIndx",&massBIndx,"massBIndx/I");
    TBranch *BR_pz = tree->Branch("pz",&pz,"pz/F");
    TBranch *BR_pzAIndx = tree->Branch("pzAIndx",&pzAIndx,"pzAIndx/I");
    TBranch *BR_pzBIndx = tree->Branch("pzBIndx",&pzBIndx,"pzBIndx/I");

    // Create distribtuions
    int num_of_evns = 1000000;
    TRandom3 randGen;
    TH1D* massAhist = new TH1D("massAhist","massAhist",100,0,10);
    TH1D *massBhist = new TH1D("massBhist","massBhist",100,0,10);
    TH1D *massABhist = new TH1D("massABhist","massABhist",100,0,10);
    TH1D *pzAhist = new TH1D("pzAhist","pzAhist",100,-4,4);
    TH1D *pzBhist = new TH1D("pzBhist","pzBhist",100,-4,4);
    TH1D *pzABhist = new TH1D("pzABhist","pzABhist",100,-4,4);
    for (int indx=0; indx<num_of_evns; indx++){
        if(indx<num_of_evns*0.5){
            mass=randGen.Gaus(5,0.75);
            massAhist->Fill(mass);
            pz=randGen.Gaus(-1,1);
            pzAhist->Fill(pz);
	    massAIndx=1;
	    pzAIndx=1;
	    massBIndx=0;
	    pzBIndx=0;
        } else{
            mass=randGen.Uniform(0,10);
            massBhist->Fill(mass);
            pz=randGen.Gaus(1,1);
            pzBhist->Fill(pz);
	    massAIndx=0;
	    pzAIndx=0;
	    massBIndx=1;
	    pzBIndx=1;
        }
        tree->Fill();
    }

    massABhist->Add(massAhist);
    massABhist->Add(massBhist);
    pzABhist->Add(pzAhist);
    pzABhist->Add(pzBhist);

    tree->Write("", TObject::kOverwrite);
    outfile.Write();
    outfile.Close();

}
