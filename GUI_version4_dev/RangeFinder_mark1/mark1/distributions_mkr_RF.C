void distributions_mkr_RF(){

    // Create outfile and tree
    TFile outfile("distributions_RF.root", "recreate");
    TTree *tree = new TTree("tree","tree");
    Float_t mass;
    Float_t pz;
    Float_t theta;
    Float_t phi;
    Int_t Dist;
    Int_t Anom;
    TBranch *BR_mass = tree->Branch("mass",&mass,"mass/F");
    TBranch *BR_pz = tree->Branch("pz",&pz,"pz/F");
    TBranch *BR_theta = tree->Branch("theta",&theta,"theta/F");
    TBranch *BR_phi = tree->Branch("phi",&phi,"phi/F");
    TBranch *BR_Dist = tree->Branch("Dist",&Dist,"Dist/I");
    TBranch *BR_Anom = tree->Branch("Anom",&Anom,"Anom/I");

    // Create Histogram, Dist refers to the regular distributions and Anom to the anomalies
    TH1D* massDistHist = new TH1D("massDistHist","massDistHist",100,0,10);
    TH1D *massAnomHist = new TH1D("massAnomHist","massAnomHist",100,0,10);
    TH1D *massHist = new TH1D("massHist","massHist",100,0,10);
    TH1D *pzDistHist = new TH1D("pzDistHist","pzDistHist",100,-5,5);
    TH1D *pzAnomHist = new TH1D("pzAnomHist","pzAnomHist",100,-5,5);
    TH1D *pzHist = new TH1D("pzHist","pzHist",100,-5,5);
    TH1D *thetaDistHist = new TH1D("thetaDistHist","thetaDistHist",100,0,50);
    TH1D *thetaAnomHist = new TH1D("thetaAnomHist","thetaAnomHist",100,0,50);
    TH1D *thetaHist = new TH1D("thetaHist","thetaHist",100,0,50);
    TH1D *phiDistHist = new TH1D("phiDistHist","phiDistHist",100,-30,30);
    TH1D *phiAnomHist = new TH1D("phiAnomHist","phiAnomHist",100,-30,30);
    TH1D *phiHist = new TH1D("phiHist","phiHist",100,-30,30);
    
    // Create distribtuions
    int num_of_evns = 1000000;
    TRandom3 randGen;
    TF1 thetaAnom("thetaAnom","TMath::Exp(-0.06*x)",0,50);
    TF1 phiDist("phiDist","TMath::Sin(x)",-30,30);
    TF1 phiAnom("phiAnom","TMath::Sin(x+1.64)",-30,30);
    //TF1 massAnom("massAnom","x",0,10);
    
    for (int indx=0; indx<num_of_evns; indx++){
        if(indx<num_of_evns*0.001){
            // Add anomalies
            mass=randGen.Uniform(-400,-200);
            massAnomHist->Fill(mass);
            pz=randGen.Gaus(1,1);
            pzAnomHist->Fill(pz);
            theta=randGen.Uniform(-400,-200);
            thetaAnomHist->Fill(theta);
            phi=phiAnom.GetRandom();
            phiAnomHist->Fill(phi);
            Dist=0;
            Anom=1;
        } else{
            // Add distributions
            //mass=randGen.Uniform(0,10);
            mass=randGen.Gaus(5,0.75);
            massDistHist->Fill(mass);
            pz=randGen.Gaus(-1,1);
            pzDistHist->Fill(pz);
            theta=abs(randGen.Gaus(15,10));
            thetaDistHist->Fill(theta);
            phi=phiDist.GetRandom();
            phiDistHist->Fill(phi);
            Dist=1;
            Anom=0;
        }
        tree->Fill();
    }

    massHist->Add(massDistHist);
    massHist->Add(massAnomHist);
    pzHist->Add(pzDistHist);
    pzHist->Add(pzAnomHist);
    thetaHist->Add(thetaDistHist);
    thetaHist->Add(thetaAnomHist);
    phiHist->Add(phiDistHist);
    phiHist->Add(phiAnomHist);

    tree->Write("", TObject::kOverwrite);
    outfile.Write();
    outfile.Close();

}
