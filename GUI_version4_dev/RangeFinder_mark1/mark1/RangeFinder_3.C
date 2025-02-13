void RangeFinder_3(string infile_1, string infile_2){

    // Open file and get tree and branches
    const char *infile_name_1 = infile_1.c_str();

    TFile f_1(infile_name_1);

    TTree *tree_1 = (TTree*)f_1.Get(f_1.GetListOfKeys()->At(0)->GetName());

    // Create a map to store the branch names and the corresponding data pointers
    std::map<std::string, void*> branches;
    std::map<std::string, std::string> branchTypes;

    // Get the list of branches in the tree
    TObjArray* branchList = tree_1->GetListOfBranches();

    // Loop through all branches and set the addresses dynamically
    for (int i = 0; i < branchList->GetEntries(); i++) {
        TBranch* branch = (TBranch*)branchList->At(i);
        std::string branchName = branch->GetName();
        cout << "branch: " << branchName << endl;

        // Get type of the branch using the leaf type
        const char* branchType = branch->GetLeaf(branchName.c_str())->GetTypeName();
        std::string branchType_str = branchType;

        // Checking the type of the branch by name or some other criteria
        if (branchType_str == "Int_t" || branchType_str == "int") {
            int* branchValue = new int;
            branches[branchName] = (void*)branchValue;
            branchTypes[branchName] = "int";
            tree_1->SetBranchAddress(branchName.c_str(), branchValue);
        } else if (branchType_str == "Double_t" || branchType_str == "double") {
            double* branchValue = new double;
            branches[branchName] = (void*)branchValue;
            branchTypes[branchName] = "double";
            tree_1->SetBranchAddress(branchName.c_str(), branchValue);
        } else if (branchType_str == "Float_t" || branchType_str == "float") {
            float* branchValue = new float;
            branches[branchName] = (void*)branchValue;
            branchTypes[branchName] = "float";
            tree_1->SetBranchAddress(branchName.c_str(), branchValue);
        } else if (branchType_str == "String_t" || branchType_str == "string") {
            string* branchValue = new string;
            branches[branchName] = (void*)branchValue;
            branchTypes[branchName] = "string";
            tree_1->SetBranchAddress(branchName.c_str(), branchValue);
        }
    }

    // Make new file and tree
    const char *infile_name_2 = infile_2.c_str();

    TFile f_2(infile_name_2,"recreate");

    TTree *newTree = tree_1->CloneTree(0);  // Clone the tree with zero entries

    // Look at chosen branch
    auto br = branches.find("MissMass_el");
    double br_val;
    int br_entries = tree_1->GetEntries();
    double var;
    double* data_var;
    vector<Float_t> allVar;    

    // Save all events in chosen branch
    for (int i2 = 0; i2 < br_entries; i2++) {

        // Progress counter
        fprintf (stderr, "Saving entires from chosen branch: %d of %d\r events", i2, br_entries);
        fflush (stderr);
        // Save data from branch to vector
        tree_1->GetEvent(i2);
        if (br != branches.end()) {
            data_var = static_cast<double*>(br->second);
            var = (double)*data_var;
        }

        // Save data to vector
        allVar.push_back(var);   
    }
    fprintf(stderr, "\n");  // New line after progress completion

    // Order vector
    sort(allVar.begin(), allVar.end());
    // Calculate lowest 0.5 percentile
    size_t indx1 = static_cast<size_t>(0.5/100.0*allVar.size());
    double p1 = allVar.at(indx1);
    
    // Fill new tree if data in chosen branch is not in bottom 0.5 percentile (bottom 0.5% of data)
    for (int i3 = 0; i3 < br_entries; i3++) {
        // Save data from branch to vector
        tree_1->GetEvent(i3);
        if (br != branches.end()) {
            data_var = static_cast<double*>(br->second);
            var = (double)*data_var;
        }
        // Checking data and filling
        tree_1->GetEvent(i3);
        if (allVar.at(indx1)>=p1){
            // Progress counter
            fprintf (stderr, "Filling tree: %d\r events", i3);
            fflush (stderr);
            newTree->Fill();
        }
    }
    fprintf(stderr, "\n");  // New line after progress completion

    newTree->Write("", TObject::kOverwrite); // save only the new version of the tree
    f_1.Close();
}
