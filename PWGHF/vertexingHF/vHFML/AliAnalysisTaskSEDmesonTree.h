#ifndef ALIANALYSISTASKSEDMESONTREE_H
#define ALIANALYSISTASKSEDMESONTREE_H

/* Copyright(c) 1998-2020, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//*************************************************************************
// \class AliAnalysisTaskSEDmesonTree
// \brief Analysis task to produce trees of D-meson candidates for ML analyses
// \authors:
// F. Grosa, fabrizio.grosa@cern.ch
/////////////////////////////////////////////////////////////

#include <THnSparse.h>

#include "AliLog.h"
#include "AliAnalysisTaskSE.h"
#include "AliAODVertex.h"
#include "AliAODEvent.h"
#include "AliAODRecoDecayHF.h"
#include "AliRDHFCuts.h"
#include "AliHFMLVarHandler.h"
#include "AliAnalysisVertexingHF.h"
#include "AliAODMCHeader.h"
#include "AliNormalizationCounter.h"
#include "AliHFMLResponse.h"

class AliAnalysisTaskSEDmesonTree : public AliAnalysisTaskSE
{
public:

    enum
    {
        kD0toKpi = 0,
        kDplustoKpipi,
        kDstartoD0pi
    };

    AliAnalysisTaskSEDmesonTree();
    AliAnalysisTaskSEDmesonTree(const char *name, int fDecChannel, AliRDHFCuts *analysiscuts, bool createMLtree);
    virtual ~AliAnalysisTaskSEDmesonTree();

    void SetReadMC(bool readMC = true)                                                            {fReadMC = readMC;}
    void SetAODMismatchProtection(int opt = 0)                                                    {fAODProtection = opt;}
    void SetAnalysisCuts(AliRDHFCuts *cuts)                                                       {fRDCuts = cuts;}
    void SetUseFinePtBinsForSparse(bool useFineBins = true)                                       {fUseFinPtBinsForSparse = useFineBins;}
    void SetFillNSparseAcceptanceLevel(bool fill = true)                                          {fFillAcceptanceLevel = fill;}
    /// methods for ML tree creation
    void SetCreateMLTree(bool flag = true)                                                        {fCreateMLtree = flag;}
    void SetMLTreePIDopt(int opt)                                                                 {fPIDopt = opt;} // default AliHFMLVarHandler::kNsigmaDetAndCombPID
    void SetMLTreeAddTrackVar(bool flag = true)                                                   {fAddSingleTrackVar = flag;}
    void SetKeepOnlyBkgFromHIJING(bool keeponlyhijing = true)                                     {fKeepOnlyBkgFromHIJING = keeponlyhijing;}

    void SetDecayChannel(bool dec = kD0toKpi)
    {
        fDecChannel = dec;
        switch(fDecChannel)
        {
            case kD0toKpi:
                fPdgD = 421;
                break;
            case kDplustoKpipi:
                fPdgD = 411;
                break;
            case kDstartoD0pi:
                fPdgD = 413;
                break;
        }
    }

    // methods for ML tree creation
    void SetFillOnlySignalInMLtree(bool opt = true)
    {
        if (fReadMC)
            fFillOnlySignal = opt;
        else
        {
            if (opt)
                AliError("fReadMC has to be true");
        }
    }
    void EnableMLTreeEvtSampling(float fractokeep, unsigned long long seed, int option=0)
    {
        fEnableEvtSampling = true;
        fFracEvtToKeep = fractokeep;
        fSeedSampling = seed;
        fOptionSampling = option;
    }
    void EnableMLTreeCandSampling(float fractokeep, float maxptsampling)
    {
        fEnableCandSampling = true;
        fFracCandToKeep = fractokeep;
        fMaxCandPtSampling = maxptsampling;
    }

    /// methods for ML application
    void SetDoMLApplication(bool flag = true, bool isMultiClass = false)                          {fApplyML = flag; fMultiClass = isMultiClass;}
    void SetMLConfigFile(std::string path = "")                                                   {fConfigPath = path;}
    void SetMLBinsForSparse(int nbins = 300, double min = 0.85, double max = 1.)                  {fNMLBins[0] = nbins; fMLOutputMin[0] = min; fMLOutputMax[0] = max;}
    void SetMultiClassMLBinsForSparse(int nbinsBkg = 100, int nbinsPrompt = 100, int nbinsFD = 100,
                                      double minBkg = 0., double maxBkg = 1.,
                                      double minPrompt = 0., double maxPrompt = 1.,
                                      double minFD = 0., double maxFD = 1.)
    {
        fNMLBins[0] = nbinsBkg; fNMLBins[1] = nbinsPrompt; fNMLBins[2] = nbinsFD;
        fMLOutputMin[0] = minBkg; fMLOutputMin[1] = minPrompt; fMLOutputMin[2] = minFD;
        fMLOutputMax[0] = maxBkg; fMLOutputMax[1] = maxPrompt; fMLOutputMax[2] = maxFD;
    }

    // Implementation of interface methods
    virtual void UserCreateOutputObjects();
    virtual void LocalInit();
    virtual void UserExec(Option_t *option);

private:
    enum
    {
        knVarForSparseAcc    = 2,
        knVarForSparseAccFD  = 3,
        knVarForSparseReco   = 6,
        knVarForSparseRecoFD = 7
    };

    AliAnalysisTaskSEDmesonTree(const AliAnalysisTaskSEDmesonTree &source);
    AliAnalysisTaskSEDmesonTree &operator=(const AliAnalysisTaskSEDmesonTree &source);

    int IsCandidateSelected(AliAODRecoDecayHF *&dMeson, AliAnalysisVertexingHF *vHF, bool &unsetVtx, bool &recVtx, AliAODVertex *&origownvtx);
    void FillMCGenAccHistos(TClonesArray *arrayMC, AliAODMCHeader *mcHeader);
    bool CheckDaugAcc(TClonesArray *arrayMC, int nProng, int *labDau);
    void CreateEffSparses();
    void CreateRecoSparses();

    AliAODEvent* fAOD = nullptr;                                                /// AOD event

    TList *fOutput = nullptr;                                                   //!<! list send on output slot 0
    TH1F *fHistNEvents = nullptr;                                               //!<! hist. for No. of events
    AliNormalizationCounter *fCounter = nullptr;                                //!<! Counter for normalization
    THnSparseF *fnSparseMC[2] = {nullptr, nullptr};                             //!<! THnSparse for MC
                                                                                ///[0]: Acc step prompt D
                                                                                ///[1]: Acc step FD D
    AliHFMLVarHandler *fMLhandler = nullptr;                                    //!<! object to handle ML tree creation and filling
    TTree *fMLtree = nullptr;                                                   //!<! tree with candidates for ML

    int fDecChannel = kD0toKpi;                                                 /// channel to analyse
    int fPdgD = 421;                                                            /// pdg code of the D meson
    bool fReadMC = false;                                                       /// flag for access to MC
    bool  fFillAcceptanceLevel = true;                                          /// flag for filling true reconstructed D at acceptance level (see FillMCGenAccHistos)
    int fAODProtection = 0;                                                     /// flag to activate protection against AOD-dAOD mismatch.
                                                                                /// -1: no protection,  0: check AOD/dAOD nEvents only,  1: check AOD/dAOD nEvents + TProcessID names
    TList *fListCuts = nullptr;                                                 /// list of cuts
    AliRDHFCuts *fRDCuts = nullptr;                                             /// Cuts for Analysis
    bool fUseFinPtBinsForSparse = true;                                         /// flag to fill pt axis of sparse with 0.1 GeV/c wide bins
    bool fKeepOnlyBkgFromHIJING = false;                                        /// flag to keep background from Hijing only

    // ML tree creation
    bool fCreateMLtree = true;
    int fPIDopt = AliHFMLVarHandler::kNsigmaDetAndCombPID;                      /// option for PID variables
    bool fAddSingleTrackVar = true;                                             /// option to store single track variables
    bool fFillOnlySignal = true;                                                /// option to store only signal when using MC
    bool fEnableEvtSampling = true;                                             /// flag to apply event sampling
    float fFracEvtToKeep = 1.1;                                                 /// fraction of events to be kept by event sampling
    unsigned long fSeedSampling = 0;                                            /// seed for event sampling
    int fOptionSampling = 0;                                                    /// option for event sampling (0: keeps events with random < fracToKeep, 1: keep events with random > 1-fracToKeep)
    bool fEnableCandSampling = true;                                            /// flag to apply candidate sampling
    float fFracCandToKeep = 1.1;                                                /// fraction of candidates to be kept by sampling
    float fMaxCandPtSampling = 0.;                                              /// maximun candidate pt to apply sampling
    bool fAddNtrkl = false;                                                     /// flag to add number of tracklets in the tree
    bool fAddCentr = false;                                                     /// flag to add centrality percentile in the tree
    std::string fCentEstimator = "V0M";                                         /// centrality estimator for tree
                    
    // ML tree application
    THnSparseF* fnSparseReco[4] = {nullptr, nullptr, nullptr, nullptr};         //!<! THnSparse for reco candidates
    bool fApplyML = false;                                                      /// flag to enable ML application
    bool fMultiClass = false;                                                   /// flag to enable multi-class models (Bkg, Prompt, FD)
    std::string fConfigPath = "";                                               /// path to ML config file
    AliHFMLResponse* fMLResponse = nullptr;                                     //!<! object to handle ML response
    int fNMLBins[3] = {1000, 100, 100};                                         /// number of bins for ML output axis in THnSparse
    double fMLOutputMin[3] = {0., 0., 0.};                                      /// min for ML output axis in THnSparse
    double fMLOutputMax[3] = {1., 1., 1.};                                      /// max for ML output axis in THnSparse

    /// \cond CLASSIMP
    ClassDef(AliAnalysisTaskSEDmesonTree, 3); /// AliAnalysisTaskSE for production of D-meson trees
                                               /// \endcond
};

#endif
