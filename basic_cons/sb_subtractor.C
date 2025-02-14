#include <TCanvas.h>
#include <TH1F.h>
#include <TFile.h>
#include <TTree.h>

// Fit functions
// Background - Quadratic background function
double Background(double *x, double *par) {
    return par[0] + par[1]*x[0] + par[2]*x[0]*x[0];
}
// Signal - Gauss Peak function
double GaussPeak(double *x, double *par) {
    return par[0]*TMath::Gaus(x[0], par[1], par[2]);
}
// Sum of background and peak function
double fitFunction(double *x, double *par) {
    return Background(x,par) + GaussPeak(x,&par[3]);
    //&par[5] is where the guass parameters start in the length-8 'par' array containing the parameters for the background and the peak
}

void sb_subtractor(){

    /* 
       Sideband subtraction - A discriminating variable is split into sideband and signal regions. 
       By looking at another variable, the control variable, in these regions, the number of signal events 
       can be approximated and then removed. Then the background events can be removed from the signal region
       of the control region.

       The method below is from https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuidePhysicsToolsSideBandSubtraction.
    
       Method:
        - choose discriminating (e.g. mass) and control (e.g. pz) variables
        - plot mass for events in a chosen range (hist 1)
        - fit hist 1 using total PDF made up of a polynomial (for BG) and a guasss (for signal)
        - plot pz for events with mass in chosen sideband regions (hist 2)
        - plot pz for evetns with mass in chosen signal region (hist 3)
        - using BG PDF, intergrate sideband regions and signal regions
        - calculate weight which is signal intergral divided by sideband integrals
        - scale hist 2 by the weight (hist 2 scaled is essentially an estimate of BG in signal region)
        - weighted hist 2 is subtracted from hist 3 
    */

    // Load file, tree, branches and distributions
    TFile infile("distributions.root");
    TTree *tree = (TTree*)infile.Get(infile.GetListOfKeys()->At(0)->GetName());
    double mass;
    double pz;
    double theta;
    double phi;
    int SIG;
    int BG;
    tree->Branch("mass",&mass);
    tree->Branch("pz",&pz);
    tree->Branch("theta",&theta);
    tree->Branch("phi",&phi);
    tree->Branch("SIG",&SIG);
    tree->Branch("BG",&BG);

    // Define sideband and signal regions, example values included
    double sb_left_lower = 0.5;
    double sb_left_upper = 2.5;
    double sb_right_lower = 7.5;
    double sb_right_upper = 9.5;
    double sig_lower = 3;
    double sig_upper = 7;

    // Plot and fit graphs, compute weight and conduct sideband subtraction
    // Plot 0 (hist 0) - mass across the whole region
    tree->Draw("mass>>hist0(100,0,10)");
    TH1F *hist0 = (TH1F*)gDirectory->Get("hist0");
    // Plot 1 (hist 1) - pz in the mass sideband regions 
    TString hist1cond = Form("mass>%.10g && mass<%.10g || mass>%.10g && mass<%.10g",sb_left_lower,sb_left_upper,sb_right_lower,sb_right_upper);
    TString hist23cond = Form("mass>%.10g && mass<%.10g",sig_lower,sig_upper);
    //TCut hist1cutsAll = hist1cut1 && hist1cut2 || hist1cut3 && hist1cut4;
    tree->Draw("pz>>hist1(100,-5,5)",hist1cond);
    TH1F *hist1 = (TH1F*)gDirectory->Get("hist1");
    // Plot 2 (hist 2) - pz in the mass signal regions
    tree->Draw("pz>>hist2(100,-5,5)",hist23cond);
    TH1F *hist2 = (TH1F*)gDirectory->Get("hist2");
    // Plot 2 - pz sideband subtracted
    tree->Draw("pz>>hist2sub(100,-5,5)",hist23cond); 
    TH1F *hist2sub = (TH1F*)gDirectory->Get("hist2sub");
    // Fit function
    TF1 *fitFcn = new TF1("fitFcn",fitFunction,0,10,6);
    fitFcn->SetParameters(1,1,0.1,500,5,0.5);
    fitFcn->SetLineColor(kViolet);
    hist0->Fit("fitFcn","R+");
    // Signal fit function
    TF1 SIGfit("SIGfit","[0]*TMath::Gaus(x,[1],[2])",0,10);
    SIGfit.SetParameter(0,fitFcn->GetParameter(3));
    SIGfit.SetParameter(1,fitFcn->GetParameter(4));
    SIGfit.SetParameter(2,fitFcn->GetParameter(5));
    // Background fit function
    TF1 BGfit("BGfit","[0]+[1]*x+[2]*x**2",0,10);
    BGfit.SetParameter(0,fitFcn->GetParameter(0));
    BGfit.SetParameter(1,fitFcn->GetParameter(1));
    BGfit.SetParameter(2,fitFcn->GetParameter(2));
    // Weight computation
    double SIG_Int = BGfit.Integral(sig_lower,sig_upper);
    double LSB_Int = BGfit.Integral(sb_left_lower,sb_left_upper);
    double RSB_Int = BGfit.Integral(sb_right_lower,sb_right_upper);
    double sig_wgt = SIG_Int/(LSB_Int+RSB_Int);
    // Sideband subtraction
    hist2sub->Add(hist1,-sig_wgt);

    // Create output file
    TFile outfile("sb_subtractor_Hists.root", "recreate");

    // Save histograms to outfile
    hist0->Write();
    hist1->Write();
    hist2->Write();
    hist2sub->Write();

    //// Ttrees
    tree->Draw("mass>>histmass(100,0,10)");
    TH1F *histmass = (TH1F*)gDirectory->Get("histmass");
    tree->Draw("mass>>histmassA(100,0,10)","SIG==1");
    TH1F *histmassA = (TH1F*)gDirectory->Get("histmassA");
    tree->Draw("mass>>histmassB(100,0,10)","BG==1");
    TH1F *histmassB = (TH1F*)gDirectory->Get("histmassB");
    tree->Draw("pz>>histpz(100,-5,5)");
    TH1F *histpz = (TH1F*)gDirectory->Get("histpz");
    tree->Draw("pz>>histpzA(100,-5,5)","SIG==1");
    TH1F *histpzA = (TH1F*)gDirectory->Get("histpzA");
    tree->Draw("pz>>histpzB(100,-5,5)","BG==1");
    TH1F *histpzB = (TH1F*)gDirectory->Get("histpzB");
    //////// Main canvas
    TCanvas *canvas1 = new TCanvas("canvas1","canvas1",1800,900);
    canvas1->Divide(2, 2);
    //// Pad 1
    canvas1->cd(1);
    histmass->SetStats(0);
    histmass->SetLineColor(kBlack);
    histmass->SetMinimum(0);
    histmass->Draw("hist");
    histmassA->SetLineColor(kRed);
    histmassA->DrawCopy("same");
    histmassB->SetLineColor(kBlue);
    histmassB->DrawCopy("same");
    TLegend leg1(.1,.7,.3,.9,"");
    leg1.SetFillColor(0);
    leg1.AddEntry(histmass,"mass");
    leg1.AddEntry(histmassA,"mass - signal");
    leg1.AddEntry(histmassB,"mass - background");
    leg1.DrawClone("Same");
    //// Pad 2
    canvas1->cd(2);
    hist0->SetStats(0);
    hist0->Draw("hist");
    // Patterned histograms
    TH1* hist0_cln1 = (TH1*)hist0->Clone();
    hist0_cln1->GetXaxis()->SetRangeUser(sb_left_lower,sb_left_upper);
    hist0_cln1->SetFillColor(kAzure+7);
    hist0_cln1->SetFillStyle(3354);
    TH1* hist0_cln2 = (TH1*)hist0->Clone();
    hist0_cln2->GetXaxis()->SetRangeUser(sb_right_lower,sb_right_upper);
    hist0_cln2->SetFillColor(kAzure+7);
    hist0_cln2->SetFillStyle(3354);
    TH1* hist0_cln3 = (TH1*)hist0->Clone();
    hist0_cln3->GetXaxis()->SetRangeUser(sig_lower,sig_upper);
    hist0_cln3->SetFillColor(kOrange-3);
    hist0_cln3->SetFillStyle(3354);
    // Draw patterned histograms
    hist0_cln1->DrawCopy("same");
    hist0_cln2->DrawCopy("same");
    hist0_cln3->DrawCopy("same");
    // hist0
    hist0->DrawCopy("same");
    // Total fit function
    fitFcn->DrawCopy("same");
    fitFcn->SetLineColor(kViolet);
    // Signal fit function
    SIGfit.SetLineColor(kRed);
    SIGfit.DrawCopy("same");
    // Background fit function
    BGfit.SetLineColor(kBlue);
    BGfit.DrawCopy("same");
    // Sideband regions
    double sb_L_y = (hist0->GetBinContent(hist0->GetMaximumBin())-hist0->GetBinContent(sb_left_upper))*0.50 + hist0->GetBinContent(sb_left_upper); // sb_L_y is 1/4 (updwards from bin at sb_left_upper) distance betwwen bin at sb_left_upper and max y-axis coordiante
    double sb_R_y = (hist0->GetBinContent(hist0->GetMaximumBin())-hist0->GetBinContent(sb_right_upper))*0.50 + hist0->GetBinContent(sb_right_upper); // sb_L_y is 1/4 (updwards from bin at sb_left_upper) distance betwwen bin at sb_left_upper and max y-axis coordiante
    TLine *sb_L_l = new TLine(sb_left_lower,0,sb_left_lower,sb_L_y);
    sb_L_l->SetLineWidth(2);
    sb_L_l->Draw();
    TLine *sb_L_u = new TLine(sb_left_upper,0,sb_left_upper,sb_L_y);
    sb_L_u->SetLineWidth(2);
    sb_L_u->Draw();
    TLine *sb_R_l = new TLine(sb_right_lower,0,sb_right_lower,sb_R_y);
    sb_R_l->SetLineWidth(2);
    sb_R_l->Draw();
    TLine *sb_R_u = new TLine(sb_right_upper,0,sb_right_upper,sb_R_y);
    sb_R_u->SetLineWidth(2);
    sb_R_u->Draw();
    // Legend
    TLegend leg2(.1,.7,.3,.9,"");
    leg2.SetFillColor(0);
    leg2.AddEntry(histmass,"mass");
    leg2.AddEntry(fitFcn,"mass - total fit");
    leg2.AddEntry(&SIGfit,"mass - signal fit");
    leg2.AddEntry(&BGfit,"mass - background fit");
    leg2.AddEntry(hist0_cln3,"mass - signal region");
    leg2.AddEntry(hist0_cln2,"mass - sideband regions");
    leg2.DrawClone("Same");
    // Signal fit info box
    TString SIGfit_str = Form("#splitline{Signal fit info}{#splitline{#mu: %.5g}{#sigma: %.5g}}", abs(fitFcn->GetParameter(3)), abs(fitFcn->GetParameter(4)));
    TPaveText* SIGfit_txt = new TPaveText(.7,.7,.9,.9,"brNDC");
    SIGfit_txt->SetFillColor(0);
    SIGfit_txt->SetBorderSize(1);
    SIGfit_txt->SetTextSize(0.05);
    SIGfit_txt->AddText(SIGfit_str);
    SIGfit_txt->DrawClone("Same");
    // Draw hist
    hist0->SetTitle("mass - signal and sidebands");
    hist0->SetMinimum(0);
    //// Pad 3
    canvas1->cd(3);
    histpz->SetStats(0);
    histpz->SetLineColor(kBlack);
    histpz->Draw("hist");
    histpzA->SetLineColor(kRed);
    histpzA->DrawCopy("same");
    histpzB->SetLineColor(kBlue);
    histpzB->DrawCopy("same");
    TLegend leg3(.1,.7,.3,.9,"");
    leg3.SetFillColor(0);
    leg3.AddEntry(histpz,"pz");
    leg3.AddEntry(histpzA,"pz - signal");
    leg3.AddEntry(histpzB,"pz - background");
    leg3.DrawClone("Same");
    histpz->SetTitle("pz");
    // Pad 4
    canvas1->cd(4);
    TH1* histpz_cln = (TH1*)histpz->Clone();
    histpz_cln->SetTitle("pz - sideband subtracted");
    histpz_cln->SetStats(0);
    histpz_cln->Draw("hist");
    histpzA->DrawCopy("same");
    hist2sub->SetLineColor(kViolet);
    hist2sub->DrawCopy("same");
    TLegend leg4(.1,.7,.3,.9,"");
    leg4.SetFillColor(0);
    leg4.AddEntry(histpz_cln,"pz");
    leg4.AddEntry(histpzA,"pz - signal");
    leg4.AddEntry(hist2sub,"pz - sideband subtracted");
    leg4.DrawClone("Same");
    //// Print canvas
    canvas1->Print("canvas1.root");

    // LAST LINE!!!
    // All drawing, plotting, everything, must be done before the follwing line
    outfile.Close();
}
