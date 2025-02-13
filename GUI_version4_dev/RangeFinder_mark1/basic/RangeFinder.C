void RangeFinder(){

    // Load file, tree, branches and distributions
    TFile infile("distributions_RF.root");
    TTree *tree = (TTree*)infile.Get(infile.GetListOfKeys()->At(0)->GetName());
    Float_t mass=0;
    Float_t pz=0;
    Float_t theta=0;
    Float_t phi=0;
    Int_t Dist=0;
    Int_t Anom=0;
    tree->SetBranchAddress("mass",&mass);
    tree->SetBranchAddress("pz",&pz);
    tree->SetBranchAddress("theta",&theta);
    tree->SetBranchAddress("phi",&phi);
    tree->SetBranchAddress("Dist",&Dist);
    tree->SetBranchAddress("Anom",&Anom);

    tree->GetEvent(0);
    cout << mass << endl;

    // RangeFinder main part
    // RF works by calculating the cumulative distribution (sumulative sum for each bin) and then selecting bins 99% of data
    // Store data in vector
    vector<Float_t> allMass;
    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEvent(i);
        allMass.push_back(mass);
        //cout << mass << endl;
    }
    // Sort data
    sort(allMass.begin(), allMass.end());
    // Calculate 1st percentile
    size_t indx1 = static_cast<size_t>(0.5/100.0*allMass.size());
    double p1 = allMass.at(indx1);
    // Calculate 99th percentile
    size_t indx99 = static_cast<size_t>(99/100.0*allMass.size());
    double p99 = allMass.at(indx99);

    cout << "1st percentile: " << p1 << endl;
    cout << "99th percentile: " << p99 << endl;

    // Create outfile and tree
    TFile outfile("RF_output.root", "recreate");
    TTree *treeOut = new TTree("treeOut","treeOut");    
    Float_t massOut;
    Float_t pzOut;
    Float_t thetaOut;
    Float_t phiOut;
    Int_t DistOut;
    Int_t AnomOut;
    TBranch *BR_mass = treeOut->Branch("massOut",&massOut,"massOut/F");
    TBranch *BR_pz = treeOut->Branch("pzOut",&pzOut,"pzOut/F");
    TBranch *BR_theta = treeOut->Branch("thetaOut",&thetaOut,"thetaOut/F");
    TBranch *BR_phi = treeOut->Branch("phiOut",&phiOut,"phiOut/F");
    TBranch *BR_Dist = treeOut->Branch("DistOut",&DistOut,"DistOut/F");
    TBranch *BR_Anom = treeOut->Branch("AnomOut",&AnomOut,"AnomOut/F");

    // Fill output tree with data between 0.5 and 100th percentile
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        if (mass >= p1){ //&& mass <= p99) {
            massOut = mass;
            treeOut->Fill();
        }
    }

    treeOut->Write();
    outfile.Close();
    infile.Close();


}
