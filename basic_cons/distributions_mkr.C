void distributions_mkr(){

    // Create outfile and tree
    TFile outfile("distributions.root", "recreate");
    TTree *tree = new TTree("tree","tree");
    Float_t mass;
    Float_t pz;
    Float_t theta;
    Float_t phi;
    Int_t SIG;
    Int_t BG;
    TBranch *BR_mass = tree->Branch("mass",&mass,"mass/F");
    TBranch *BR_pz = tree->Branch("pz",&pz,"pz/F");
    TBranch *BR_theta = tree->Branch("theta",&theta,"theta/F");
    TBranch *BR_phi = tree->Branch("phi",&phi,"phi/F");
    TBranch *BR_SIG = tree->Branch("SIG",&SIG,"SIG/I");
    TBranch *BR_BG = tree->Branch("BG",&BG,"BG/I");

    // Create histograms
    TH1D* massSIGhist = new TH1D("massSIGhist","massSIGhist",100,0,10);
    TH1D *massBGhist = new TH1D("massBGhist","massBGhist",100,0,10);
    TH1D *masshist = new TH1D("masshist","masshist",100,0,10);
    TH1D *pzSIGhist = new TH1D("pzSIGhist","pzSIGhist",100,-5,5);
    TH1D *pzBGhist = new TH1D("pzBGhist","pzBGhist",100,-5,5);
    TH1D *pzhist = new TH1D("pzhist","pzhist",100,-5,5);
    TH1D *thetaSIGhist = new TH1D("thetaSIGhist","thetaSIGhist",100,0,50);
    TH1D *thetaBGhist = new TH1D("thetaBGhist","thetaBGhist",100,0,50);
    TH1D *thetahist = new TH1D("thetahist","thetahist",100,0,50);
    TH1D *phiSIGhist = new TH1D("phiSIGhist","phiSIGhist",100,-30,30);
    TH1D *phiBGhist = new TH1D("phiBGhist","phiBGhist",100,-30,30);
    TH1D *phihist = new TH1D("phihist","phihist",100,-30,30);
    
    // Create distribtuions
    int num_of_evns = 1000000;
    TRandom3 randGen;
    TF1 thetaBG("thetaBG","TMath::Exp(-0.06*x)",0,50);
    TF1 phiSIG("phiSIG","TMath::Sin(x)",-30,30);
    TF1 phiBG("phiBG","0.1*TMath::Sin((x)+1.64)+2",0,15);
    TF1 massBG("massBG","x",0,10);
    
    for (int indx=0; indx<num_of_evns; indx++){
        if(indx<num_of_evns*0.1){
            mass=randGen.Gaus(5,0.75);
            massSIGhist->Fill(mass);
            pz=randGen.Gaus(-1,1);
            pzSIGhist->Fill(pz);
            theta=abs(randGen.Gaus(15,10));
            thetaSIGhist->Fill(theta);
            phi=(2*mass)+2;
            //phi=randGen.Gaus(7,2);
            //phi=phiSIG.GetRandom();
            phiSIGhist->Fill(phi);
            SIG=1;
            BG=0;
        } else{
            mass=randGen.Uniform(0,10);
            //mass=randGen.Gaus(3,5);
            massBGhist->Fill(mass);
            pz=randGen.Gaus(1,1);
            pzBGhist->Fill(pz);
            theta=thetaBG.GetRandom();
            thetaBGhist->Fill(theta);
            phi=phiBG.GetRandom();
            phiBGhist->Fill(phi);
            SIG=0;
            BG=1;
        }
        tree->Fill();
    }

    masshist->Add(massSIGhist);
    masshist->Add(massBGhist);
    pzhist->Add(pzSIGhist);
    pzhist->Add(pzBGhist);
    thetahist->Add(thetaSIGhist);
    thetahist->Add(thetaBGhist);
    phihist->Add(phiSIGhist);
    phihist->Add(phiBGhist);

    tree->Write("", TObject::kOverwrite);
    outfile.Write();
    outfile.Close();

}
