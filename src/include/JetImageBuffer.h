#ifndef JETIMAGEBUFFER_H
#define JETIMAGEBUFFER_H

#include <string>
#include <vector>

#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/Filter.hh"

#include "TFile.h"
#include "TParticle.h"
#include "TTree.h"
#include "TH2F.h"

#include "Pythia8/Pythia.h"

#include "Njettiness.hh"
#include "Nsubjettiness.hh"


#include "spdlog/spdlog.h"

class JetImageBuffer {
  public:
    JetImageBuffer(int imagesize=25, bool debug=false);
    ~JetImageBuffer();

    void Begin();
    void AnalyzeEvent(int iEvt, Pythia8::Pythia *pythia8,
                      Pythia8::Pythia *pythia_MB, int NPV, int pixels,
                      float range);

    void End();
    void DeclareBranches();
    void ResetBranches();

    void Debug(int debug) {
        fDebug = debug;
        m_console->set_level(spdlog::level::debug);
    }

    void SetOutName(const std::string &outname) {
        fOutName = outname;
    }

  private:
    int ftest;
    int fDebug;
    std::string fOutName;

    TFile *tF;
    TTree *tT;

    // Tree Vars 
    int evt_number;
    int m_NPV;

    void SetupInt(int &val, TString name);
    void SetupFloat(float &val, TString name);

    std::vector<TString> names;
    std::vector<float> pts;
    std::vector<float> ms;
    std::vector<float> etas;
    std::vector<float> nsub21s;
    std::vector<float> nsub32s;
    std::vector<int> nsubs;

    TH2F *detector;

    int MaxN;

    int m_NFilled;

    float m_LeadingEta;
    float m_LeadingPhi;
    float m_LeadingPt;
    float m_LeadingM;

    float m_LeadingEta_nopix;
    float m_LeadingPhi_nopix;
    float m_LeadingPt_nopix;
    float m_LeadingM_nopix;

    float m_SubLeadingEta;
    float m_SubLeadingPhi;

    float m_PCEta;
    float m_PCPhi;

    float m_Tau1;
    float m_Tau2;
    float m_Tau3;

    float m_Tau21;
    float m_Tau32;

    float m_Tau1_nopix;
    float m_Tau2_nopix;
    float m_Tau3_nopix;

    float m_Tau21_nopix;
    float m_Tau32_nopix;

    float m_deltaR;
    float *m_Intensity;

    std::shared_ptr<spdlog::logger> m_console = spdlog::stdout_color_mt("JetImageBuffer");
};

#endif
