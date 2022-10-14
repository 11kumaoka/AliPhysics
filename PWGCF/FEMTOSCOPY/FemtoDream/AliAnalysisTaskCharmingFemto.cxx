#include "AliAnalysisTaskCharmingFemto.h"

#include "yaml-cpp/yaml.h"

#include "AliAnalysisManager.h"
#include "AliInputEventHandler.h"
#include "AliMultSelection.h"
#include "AliPIDResponse.h"
#include "AliRDHFCutsDplustoKpipi.h"
#include "AliRDHFCutsDStartoKpipi.h"
#include "AliAODRecoDecayHF.h"
#include "AliAODRecoDecayHF3Prong.h"
#include "AliAODRecoCascadeHF.h"
#include "AliHFMLResponse.h"
#include "AliAODHandler.h"
#include "AliHFMLResponseDplustoKpipi.h"
#include "AliHFMLResponseDstartoD0pi.h"
#include "AliAnalysisTaskSECharmHadronMLSelector.h"
#include "AliMLModelHandler.h"
#include "TDatabasePDG.h"
#include "TRandom.h"

static float dummyfloat;
static std::vector<int> dummyvector;
static int dummyint;

// todo check Dstar soft pion
static std::vector<float> dplus_invmass;
static std::vector<float> dplus_pt;
static std::vector<float> dplus_eta;
static std::vector<int> dplus_origin;
static std::vector<std::vector<int>> dplus_daus;
static std::vector<float> dplus_bkg_score;
static std::vector<float> dplus_prompt_score;

static std::vector<float> dminus_invmass;
static std::vector<float> dminus_pt;
static std::vector<float> dminus_eta;
static std::vector<int> dminus_origin;
static std::vector<std::vector<int>> dminus_daus;
static std::vector<float> dminus_bkg_score;
static std::vector<float> dminus_prompt_score;

static std::vector<float> protons_eta;
static std::vector<float> protons_pt;
static std::vector<float> protons_nsigtpc;
static std::vector<float> protons_nsigtof;
static std::vector<float> protons_ncls;
static std::vector<float> protons_ncrossed;
static std::vector<float> protons_dcaz;
static std::vector<float> protons_dcaxy;

static std::vector<float> antiprotons_eta;
static std::vector<float> antiprotons_pt;
static std::vector<float> antiprotons_nsigtpc;
static std::vector<float> antiprotons_nsigtof;
static std::vector<float> antiprotons_ncls;
static std::vector<float> antiprotons_ncrossed;
static std::vector<float> antiprotons_dcaz;
static std::vector<float> antiprotons_dcaxy;

ClassImp(AliAnalysisTaskCharmingFemto)

    //____________________________________________________________________________________________________
    AliAnalysisTaskCharmingFemto::AliAnalysisTaskCharmingFemto()
    : AliAnalysisTaskSE("AliAnalysisTaskCharmingFemto"),
      fInputEvent(nullptr),
      fEvent(nullptr),
      fEvtCuts(nullptr),
      fProtonTrack(nullptr),
      fTrackCutsPartProton(nullptr),
      fTrackCutsPartAntiProton(nullptr),
      fConfig(nullptr),
      fPairCleaner(nullptr),
      fPartColl(nullptr),
      fIsMC(false),
      fUseMCTruthReco(false),
      fIsMCtruth(false),
      fUseTree(false),
      fIsLightweight(false),
      fTrigger(AliVEvent::kINT7),
      fSystem(kpp13TeV),
      fCheckProtonSPDHit(false),
      fTrackBufferSize(2500),
      fDmesonPDGs{},
      fLightPDG(0),
      fGTI(nullptr),
      fQA(nullptr),
      fEvtHistList(nullptr),
      fTrackCutHistList(nullptr),
      fTrackCutHistMCList(nullptr),
      fAntiTrackCutHistList(nullptr),
      fAntiTrackCutHistMCList(nullptr),
      fDChargedHistList(nullptr),
      fResultList(nullptr),
      fResultQAList(nullptr),
      fHistBuddyplusEtaVsp(nullptr),
      fHistBuddyminusEtaVsp(nullptr),
      fHistDplusInvMassPt(nullptr),
      fHistDplusInvMassPtSel(nullptr),
      fHistDplusEta(nullptr),
      fHistDplusPhi(nullptr),
      fHistDplusChildPt(),
      fHistDplusChildEta(),
      fHistDplusChildPhi(),
      fHistDplusMCPDGPt(nullptr),
      fHistDplusMCPtRes(nullptr),
      fHistDplusMCPhiRes(nullptr),
      fHistDplusMCThetaRes(nullptr),
      fHistDplusMCOrigin(nullptr),
      fHistDplusEtaVsp(nullptr),
      fHistDminusInvMassPt(nullptr),
      fHistDminusInvMassPtSel(nullptr),
      fHistDminusEta(nullptr),
      fHistDminusPhi(nullptr),
      fHistDminusChildPt(),
      fHistDminusChildEta(),
      fHistDminusChildPhi(),
      fHistDminusMCPDGPt(nullptr),
      fHistDminusMCPtRes(nullptr),
      fHistDminusMCPhiRes(nullptr),
      fHistDminusMCThetaRes(nullptr),
      fHistDminusMCOrigin(nullptr),
      fHistDminusEtaVsp(nullptr),
      fDoDorigPlots(false),
      fHistDplusMCtruthmotherPDG(nullptr),
      fHistDplusMCtruthQuarkOrigin(nullptr),
      fHistDminusMCtruthmotherPDG(nullptr),
      fHistDminusMCtruthQuarkOrigin(nullptr),
      fDecChannel(kDplustoKpipi),
      fRDHFCuts(nullptr),
      fAODProtection(0),
      fMassSelectionType(kSignal),
      fNSigmaMass(2.),
      fNSigmaOffsetSideband(5.),
      fLowerMassSelection(0.),
      fUpperMassSelection(999.),
      fSidebandWidth(0.2),
      fLowerDstarRemoval(1.992),
      fUpperDstarRemoval(2.028),
      fMCBeautyRejection(false),
      fMCBeautyScalingFactor(1.),
      fUseTrueDOnly(false),
      fInvMassCutLow(0.),
      fInvMassCutHigh(0.),
      fBuddypTlow(0.),
      fBuddypThigh(0.),
      fBuddyeta(0.),
      fBuddyOrigin(0),
      fDmesonOrigin(0),
      fApplyML(false),
      fConfigPath(""),
      fMLResponse(nullptr),
      fDependOnMLSelector(false),
      fPtLimsML{},
      fMLScoreCuts{},
      fMLOptScoreCuts{} {}

//____________________________________________________________________________________________________
AliAnalysisTaskCharmingFemto::AliAnalysisTaskCharmingFemto(const char *name,
                                                           const bool isMC,
                                                           const bool isMCtruth,
                                                           const bool useTree)
    : AliAnalysisTaskSE(name),
      fInputEvent(nullptr),
      fEvent(nullptr),
      fEvtCuts(nullptr),
      fProtonTrack(nullptr),
      fTrackCutsPartProton(nullptr),
      fTrackCutsPartAntiProton(nullptr),
      fConfig(nullptr),
      fPairCleaner(nullptr),
      fPartColl(nullptr),
      fIsMC(isMC),
      fUseMCTruthReco(false),
      fIsMCtruth(isMCtruth),
      fUseTree(useTree),
      fIsLightweight(false),
      fTrigger(AliVEvent::kINT7),
      fSystem(kpp13TeV),
      fCheckProtonSPDHit(false),
      fTrackBufferSize(2500),
      fDmesonPDGs{},
      fLightPDG(0),
      fGTI(nullptr),
      fQA(nullptr),
      fEvtHistList(nullptr),
      fTrackCutHistList(nullptr),
      fTrackCutHistMCList(nullptr),
      fAntiTrackCutHistList(nullptr),
      fAntiTrackCutHistMCList(nullptr),
      fDChargedHistList(nullptr),
      fResultList(nullptr),
      fResultQAList(nullptr),
      fHistBuddyplusEtaVsp(nullptr),
      fHistBuddyminusEtaVsp(nullptr),
      fHistDplusInvMassPt(nullptr),
      fHistDplusInvMassPtSel(nullptr),
      fHistDplusEta(nullptr),
      fHistDplusPhi(nullptr),
      fHistDplusChildPt(),
      fHistDplusChildEta(),
      fHistDplusChildPhi(),
      fHistDplusMCPDGPt(nullptr),
      fHistDminusInvMassPt(nullptr),
      fHistDminusInvMassPtSel(nullptr),
      fHistDplusMCPtRes(nullptr),
      fHistDplusMCPhiRes(nullptr),
      fHistDplusMCThetaRes(nullptr),
      fHistDplusMCOrigin(nullptr),
      fHistDplusEtaVsp(nullptr),
      fHistDminusEta(nullptr),
      fHistDminusPhi(nullptr),
      fHistDminusChildPt(),
      fHistDminusChildEta(),
      fHistDminusChildPhi(),
      fHistDminusMCPDGPt(nullptr),
      fHistDminusMCPtRes(nullptr),
      fHistDminusMCPhiRes(nullptr),
      fHistDminusMCThetaRes(nullptr),
      fHistDminusMCOrigin(nullptr),
      fHistDminusEtaVsp(nullptr),
      fDoDorigPlots(false),
      fHistDplusMCtruthmotherPDG(nullptr),
      fHistDplusMCtruthQuarkOrigin(nullptr),
      fHistDminusMCtruthmotherPDG(nullptr),
      fHistDminusMCtruthQuarkOrigin(nullptr),
      fDecChannel(kDplustoKpipi),
      fRDHFCuts(nullptr),
      fAODProtection(0),
      fMassSelectionType(kSignal),
      fNSigmaMass(2.),
      fNSigmaOffsetSideband(5.),
      fLowerMassSelection(0.),
      fUpperMassSelection(999.),
      fSidebandWidth(0.2),
      fLowerDstarRemoval(1.992),
      fUpperDstarRemoval(2.028),
      fMCBeautyRejection(false),
      fMCBeautyScalingFactor(1.),
      fUseTrueDOnly(false),
      fInvMassCutLow(0.),
      fInvMassCutHigh(0.),
      fBuddypTlow(0.),
      fBuddypThigh(0.),
      fBuddyeta(0.),
      fBuddyOrigin(0),
      fDmesonOrigin(0),
      fApplyML(false),
      fConfigPath(""),
      fMLResponse(nullptr),
      fDependOnMLSelector(false),
      fPtLimsML{},
      fMLScoreCuts{},
      fMLOptScoreCuts{} {
  DefineInput(0, TChain::Class());
  DefineOutput(1, TList::Class());
  DefineOutput(2, TList::Class());
  DefineOutput(3, TList::Class());
  DefineOutput(4, TList::Class());
  DefineOutput(5, TList::Class());
  DefineOutput(6, TList::Class());
  DefineOutput(7, TList::Class());
  switch(fDecChannel){ // save cut object for HF particle
    case kDplustoKpipi:
    {
      DefineOutput(8, AliRDHFCutsDplustoKpipi::Class());
      break;
    }
    case kDstartoKpipi:
    {
      DefineOutput(8, AliRDHFCutsDStartoKpipi::Class());
      break;
    }
    default:
    {
      AliFatal("Invalid HF channel.");
      break;
    }
  }

  int nOutput = 9;
  if (fIsMC) {
    DefineOutput(nOutput++, TList::Class());
    DefineOutput(nOutput++, TList::Class());
  }
  if (fUseTree) {
    DefineOutput(nOutput++, TTree::Class());
    DefineOutput(nOutput++, TTree::Class());
    DefineOutput(nOutput++, TTree::Class());
    DefineOutput(nOutput++, TTree::Class());
    DefineOutput(nOutput++, TTree::Class());
    DefineOutput(nOutput++, TTree::Class());
    DefineOutput(nOutput++, TTree::Class());
    DefineOutput(nOutput++, TTree::Class());
  }
}

//____________________________________________________________________________________________________
AliAnalysisTaskCharmingFemto::~AliAnalysisTaskCharmingFemto() {
  delete fPartColl;
  delete fPairCleaner;
  delete fProtonTrack;
  delete fRDHFCuts;

  if(fApplyML && fMLResponse) {
    delete fMLResponse;
  }
}

//____________________________________________________________________________________________________
void AliAnalysisTaskCharmingFemto::LocalInit()
{
    // Initialization
    switch(fDecChannel) {
      case kDplustoKpipi:
      {
        AliRDHFCutsDplustoKpipi* copyCutDplus = new AliRDHFCutsDplustoKpipi(*(static_cast<AliRDHFCutsDplustoKpipi*>(fRDHFCuts)));
        PostData(8, copyCutDplus);
        break;
    }
      case kDstartoKpipi:
      {
        AliRDHFCutsDStartoKpipi* copyCutDstar = new AliRDHFCutsDStartoKpipi(*(static_cast<AliRDHFCutsDStartoKpipi*>(fRDHFCuts)));
        PostData(8, copyCutDstar);
        break;
    }
  }
  return;
}

//____________________________________________________________________________________________________
void AliAnalysisTaskCharmingFemto::UserExec(Option_t * /*option*/) {
  fInputEvent = static_cast<AliAODEvent*>(InputEvent());
  TClonesArray *fArrayMCAOD = nullptr;
  if (fIsMC) {
    fMCEvent = MCEvent();
    fArrayMCAOD = dynamic_cast<TClonesArray *>(fInputEvent->FindListObject(AliAODMCParticle::StdBranchName()));
  }

  // PREAMBLE - CHECK EVERYTHING IS THERE
  if (!fInputEvent) {
    AliError("No Input event");
    return;
  }

  // Protection against the mismatch of candidate TRefs:
  // Check if AOD and corresponding deltaAOD files contain the same number of events.
  // In case of discrepancy the event is rejected.
  if(fAODProtection >= 0) {
    // Protection against different number of events in the AOD and deltaAOD
    // In case of discrepancy the event is rejected.
    int matchingAODdeltaAODlevel = AliRDHFCuts::CheckMatchingAODdeltaAODevents();
    if (matchingAODdeltaAODlevel < 0 || (matchingAODdeltaAODlevel == 0 && fAODProtection == 1)) {
      // AOD/deltaAOD trees have different number of entries || TProcessID do not match while it was required
      return;
    }
  }

  // GET HF CANDIDATE ARRAY
  TClonesArray *arrayHF = nullptr;
  int absPdgMom = 0;
  TString mesonName = "";
  if(!fInputEvent && AODEvent() && IsStandardAOD()) {
    // In case there is an AOD handler writing a standard AOD, use the AOD
    // event in memory rather than the input (ESD) event.
    fInputEvent = dynamic_cast<AliAODEvent*>(AODEvent());
    // in this case the braches in the deltaAOD (AliAOD.VertexingHF.root)
    // have to taken from the AOD event hold by the AliAODExtension
    AliAODHandler* aodHandler = (AliAODHandler*)((AliAnalysisManager::GetAnalysisManager())->GetOutputEventHandler());
    if(aodHandler->GetExtensions()) {
      AliAODExtension *ext = (AliAODExtension*)aodHandler->GetExtensions()->FindObject("AliAOD.VertexingHF.root");
      AliAODEvent *aodFromExt = ext->GetAOD();
      switch(fDecChannel) {
        case kDplustoKpipi:
          absPdgMom = 411;
          mesonName = "Dplus";
          arrayHF = dynamic_cast<TClonesArray*>(aodFromExt->GetList()->FindObject("Charm3Prong"));
          break;
        case kDstartoKpipi:
          absPdgMom = 413;
          mesonName = "DStar";
          arrayHF = dynamic_cast<TClonesArray*>(aodFromExt->GetList()->FindObject("Dstar"));
          break;
      }
    }
  }
  else if(fInputEvent){
    switch(fDecChannel) {
      case kDplustoKpipi:
        absPdgMom = 411;
        mesonName = "Dplus";
        arrayHF = dynamic_cast<TClonesArray*>(fInputEvent->GetList()->FindObject("Charm3Prong"));
        break;
      case kDstartoKpipi:
        absPdgMom = 413;
        mesonName = "DStar";
        arrayHF = dynamic_cast<TClonesArray*>(fInputEvent->GetList()->FindObject("Dstar"));
        break;
    }
  }

  if(!arrayHF) {
    AliError("Branch not found!\n");
    return;
  }

  if (fIsMC && !fMCEvent) {
    AliError("No MC event");
    return;
  }

  if (fIsMC && !fArrayMCAOD) {
    AliError("No MC AOD array");
    return;
  }

  if (!fEvtCuts) {
    AliError("Event Cuts missing");
    return;
  }

  if (!fTrackCutsPartProton || !fTrackCutsPartAntiProton) {
    AliError("Proton Cuts missing");
    return;
  }

  // Reset the pair cleaner
  fPairCleaner->ResetArray();

  // EVENT SELECTION
  fEvent->SetEvent(fInputEvent);
  if (!fEvtCuts->isSelected(fEvent))
    return;

  if(fCheckProtonSPDHit) { // force usage of global tracks since TPC only tracks have no ITS hits
    fTrackCutsPartProton->SetFilterBit(96);
    fTrackCutsPartAntiProton->SetFilterBit(96);
  }

  // PROTON SELECTION
  ResetGlobalTrackReference();
  for (int iTrack = 0; iTrack < fInputEvent->GetNumberOfTracks(); ++iTrack) {
    AliAODTrack *track = static_cast<AliAODTrack *>(fInputEvent->GetTrack(
        iTrack));
    if (!track) {
      AliFatal("No Standard AOD");
      return;
    }
    StoreGlobalTrackReference(track);
  }
  static std::vector<AliFemtoDreamBasePart> protons;
  static std::vector<AliFemtoDreamBasePart> antiprotons;
  static std::vector<AliFemtoDreamBasePart> dplus;
  static std::vector<AliFemtoDreamBasePart> dminus;
  protons.clear();
  antiprotons.clear();
  dplus.clear();
  dminus.clear();

  dplus_invmass.clear();
  dplus_pt.clear();
  dplus_eta.clear();
  dplus_origin.clear();
  dplus_daus.clear();
  dplus_bkg_score.clear();
  dplus_prompt_score.clear();
  dminus_invmass.clear();
  dminus_pt.clear();
  dminus_eta.clear();
  dminus_origin.clear();
  dminus_daus.clear();
  dminus_bkg_score.clear();
  dminus_prompt_score.clear();

  // light flavor + CC
  protons_nsigtpc.clear();
  protons_nsigtof.clear();
  protons_nsigtpc.clear();
  protons_nsigtof.clear();
  protons_eta.clear();
  protons_pt.clear();
  protons_ncls.clear();
  protons_ncrossed.clear();
  protons_dcaz.clear();
  protons_dcaxy.clear();
  antiprotons_nsigtpc.clear();
  antiprotons_nsigtof.clear();
  antiprotons_nsigtpc.clear();
  antiprotons_nsigtof.clear();
  antiprotons_eta.clear();
  antiprotons_pt.clear();
  antiprotons_ncls.clear();
  antiprotons_ncrossed.clear();
  antiprotons_dcaz.clear();
  antiprotons_dcaxy.clear();

  fFemtoPair.mult = fEvent->GetMultiplicity();
  fFemtoPair.vz = fEvent->GetZVertex();

  fProtonTrack->SetGlobalTrackInfo(fGTI, fTrackBufferSize);

  for (int iTrack = 0; iTrack < fInputEvent->GetNumberOfTracks(); ++iTrack) {
    AliAODTrack *track = static_cast<AliAODTrack *>(fInputEvent->GetTrack(
        iTrack));

    if (fCheckProtonSPDHit) {
      if (!track->HasPointOnITSLayer(0) && !track->HasPointOnITSLayer(1)) {
        continue;
      }
    }
    float DmesonBuddyMass =
      TDatabasePDG::Instance()->GetParticle(fTrackCutsPartProton->GetPDGCode())->Mass();

    fProtonTrack->SetTrack(track);
    fProtonTrack->SetInvMass(DmesonBuddyMass);

    int mcpdg = 0;
    AliAODMCParticle *mcPart = nullptr;
    if (fUseMCTruthReco && track->GetLabel() >= 0){ // Fake tracks have label < 0. Reject them.

      mcPart = (AliAODMCParticle *)fArrayMCAOD->At(track->GetLabel());
      if(mcPart){
        mcpdg = mcPart->GetPdgCode();
      }
    }

    AliPID::EParticleType buddyParticle;
    if (fLightPDG == 211) {
      buddyParticle = AliPID::kPion;
    } else if (fLightPDG == 321) {
      buddyParticle = AliPID::kKaon;
    } else {
      AliFatal("buddy not implemented!");
    }
    if (fTrackCutsPartProton->isSelected(fProtonTrack)) {
      if (fUseMCTruthReco && (mcpdg == fTrackCutsPartProton->GetPDGCode()) && mcPart && SelectBuddyOrigin(mcPart)){
        if (fUseTree) {
           
          protons_nsigtpc.push_back(fProtonTrack->GetnSigmaTPC(buddyParticle));
          protons_nsigtof.push_back(fProtonTrack->GetnSigmaTOF(buddyParticle));
          protons_eta.push_back(fProtonTrack->GetEta()[0]);
          protons_pt.push_back(fProtonTrack->GetPt());
          protons_ncls.push_back(fProtonTrack->GetNClsTPC());
          protons_ncrossed.push_back(fProtonTrack->GetTPCCrossedRows());
          protons_dcaxy.push_back(fProtonTrack->GetDCAXYProp());
          protons_dcaz.push_back(fProtonTrack->GetDCAZProp());
        }
        fProtonTrack->SetDCAXY(fProtonTrack->GetDCAXYProp());
        fProtonTrack->SetDCAZ(fProtonTrack->GetDCAZProp());
        fProtonTrack->SetNCrossedRows(fProtonTrack->GetTPCCrossedRows());
        fProtonTrack->SetNCls(fProtonTrack->GetNClsTPC());
        fProtonTrack->SetNSigTPC(fProtonTrack->GetnSigmaTPC(buddyParticle));
        fProtonTrack->SetNSigTOF(fProtonTrack->GetnSigmaTOF(buddyParticle));
        protons.push_back(*fProtonTrack);
      }
      else if (!fIsMCtruth && !fUseMCTruthReco) {
        if (fUseTree) {
          protons_nsigtpc.push_back(fProtonTrack->GetnSigmaTPC(buddyParticle));
          protons_nsigtof.push_back(fProtonTrack->GetnSigmaTOF(buddyParticle));
          protons_eta.push_back(fProtonTrack->GetEta()[0]);
          protons_pt.push_back(fProtonTrack->GetPt());
          protons_ncls.push_back(fProtonTrack->GetNClsTPC());
          protons_ncrossed.push_back(fProtonTrack->GetTPCCrossedRows());
          protons_dcaxy.push_back(fProtonTrack->GetDCAXYProp());
          protons_dcaz.push_back(fProtonTrack->GetDCAZProp());
        }

        fProtonTrack->SetDCAXY(fProtonTrack->GetDCAXYProp());
        fProtonTrack->SetDCAZ(fProtonTrack->GetDCAZProp());
        fProtonTrack->SetNCrossedRows(fProtonTrack->GetTPCCrossedRows());
        fProtonTrack->SetNCls(fProtonTrack->GetNClsTPC());
        fProtonTrack->SetNSigTPC(fProtonTrack->GetnSigmaTPC(buddyParticle));
        fProtonTrack->SetNSigTOF(fProtonTrack->GetnSigmaTOF(buddyParticle));
        protons.push_back(*fProtonTrack);
        fHistBuddyplusEtaVsp->Fill(fProtonTrack->GetMomentum().Mag(), fProtonTrack->GetEta()[0]);
      }
    }
    if (fTrackCutsPartAntiProton->isSelected(fProtonTrack)) {
      if(fUseMCTruthReco && (mcpdg == fTrackCutsPartAntiProton->GetPDGCode()) && mcPart && SelectBuddyOrigin(mcPart)) {
        if (fUseTree) {
          antiprotons_nsigtpc.push_back(fProtonTrack->GetnSigmaTPC(buddyParticle));
          antiprotons_nsigtof.push_back(fProtonTrack->GetnSigmaTOF(buddyParticle));
          antiprotons_eta.push_back(fProtonTrack->GetEta()[0]);
          antiprotons_pt.push_back(fProtonTrack->GetPt());
          antiprotons_ncls.push_back(fProtonTrack->GetNClsTPC());
          antiprotons_ncrossed.push_back(fProtonTrack->GetTPCCrossedRows());
          antiprotons_dcaxy.push_back(fProtonTrack->GetDCAXYProp());
          antiprotons_dcaz.push_back(fProtonTrack->GetDCAZProp());
        }
        
        fProtonTrack->SetDCAXY(fProtonTrack->GetDCAXYProp());
        fProtonTrack->SetDCAZ(fProtonTrack->GetDCAZProp());
        fProtonTrack->SetNCrossedRows(fProtonTrack->GetTPCCrossedRows());
        fProtonTrack->SetNCls(fProtonTrack->GetNClsTPC());
        fProtonTrack->SetNSigTPC(fProtonTrack->GetnSigmaTPC(buddyParticle));
        fProtonTrack->SetNSigTOF(fProtonTrack->GetnSigmaTOF(buddyParticle));
        antiprotons.push_back(*fProtonTrack);
      }
      else if (!fIsMCtruth && !fUseMCTruthReco) {
        if (fUseTree) {
          antiprotons_nsigtpc.push_back(fProtonTrack->GetnSigmaTPC(buddyParticle));
          antiprotons_nsigtof.push_back(fProtonTrack->GetnSigmaTOF(buddyParticle));
          antiprotons_eta.push_back(fProtonTrack->GetEta()[0]);
          antiprotons_pt.push_back(fProtonTrack->GetPt());
          antiprotons_ncls.push_back(fProtonTrack->GetNClsTPC());
          antiprotons_ncrossed.push_back(fProtonTrack->GetTPCCrossedRows());
          antiprotons_dcaxy.push_back(fProtonTrack->GetDCAXYProp());
          antiprotons_dcaz.push_back(fProtonTrack->GetDCAZProp());
        }

        fProtonTrack->SetDCAXY(fProtonTrack->GetDCAXYProp());
        fProtonTrack->SetDCAZ(fProtonTrack->GetDCAZProp());
        fProtonTrack->SetNCrossedRows(fProtonTrack->GetTPCCrossedRows());
        fProtonTrack->SetNCls(fProtonTrack->GetNClsTPC());
        fProtonTrack->SetNSigTPC(fProtonTrack->GetnSigmaTPC(buddyParticle));
        fProtonTrack->SetNSigTOF(fProtonTrack->GetnSigmaTOF(buddyParticle));
        antiprotons.push_back(*fProtonTrack);
        fHistBuddyminusEtaVsp->Fill(fProtonTrack->GetMomentum().Mag(), fProtonTrack->GetEta()[0]);
      }
    }
  }

  auto trackCutsDdau = fRDHFCuts->GetTrackCuts();
  float ptMin, ptMax, etaMin, etaMax;
  trackCutsDdau->GetPtRange(ptMin, ptMax);
  trackCutsDdau->GetEtaRange(etaMin, etaMax);
  float* DmesonPtBins = fRDHFCuts->GetPtBinLimits();
  float DmesonPtMin=DmesonPtBins[0];
  float DmesonPtMax=DmesonPtBins[fRDHFCuts->GetNPtBins()];

  if (fIsMCtruth) {
    int noPart = fArrayMCAOD->GetEntriesFast();

    for (int iPart = 1; iPart < noPart; iPart++) {
      AliAODMCParticle *mcPart = (AliAODMCParticle *)fArrayMCAOD->At(iPart);
      if (!(mcPart)) {
        std::cout << "NO MC particle" << std::endl;
        continue;
      }

      if (mcPart->GetLabel() < 0) {
        continue;
      }

      AliFemtoDreamBasePart part;

      int mcpdg = mcPart->GetPdgCode();
      double pt = mcPart->Pt();
      double eta = mcPart->Eta();
      if (mcpdg == fTrackCutsPartProton->GetPDGCode()) {
        if ((pt < fBuddypThigh && pt > fBuddypTlow) && (eta > -fBuddyeta && eta < fBuddyeta)) {
          if(SelectBuddyOrigin(mcPart)){
            part.SetMCParticleRePart(mcPart);
            part.SetID(mcPart->GetLabel());
            part.SetIDTracks(mcPart->GetLabel());
            protons.push_back(part);

            if (fUseTree) {
              protons_eta.push_back(mcPart->Eta());
              protons_pt.push_back(mcPart->Pt());
            }
          }
        }
      }

      if (mcpdg == fTrackCutsPartAntiProton->GetPDGCode()) {
        if ((pt < fBuddypThigh && pt > fBuddypTlow) && (eta > -fBuddyeta && eta < fBuddyeta)) {
          if(SelectBuddyOrigin(mcPart)){
            part.SetMCParticleRePart(mcPart);
            part.SetID(mcPart->GetLabel());
            part.SetIDTracks(mcPart->GetLabel());
            antiprotons.push_back(part);
            
            if (fUseTree) {
              antiprotons_eta.push_back(mcPart->Eta());
              antiprotons_pt.push_back(mcPart->Pt());
            }
          }
        }
      }

      if ((std::abs(mcpdg)) == absPdgMom && pt>DmesonPtMin && pt<DmesonPtMax ){
        if (fDecChannel == kDstartoKpipi){
          // select the correct decay channel

          if (std::abs(mcpdg) != 413 || mcPart->GetNDaughters() != 2 ) {
            continue;
          }
          auto *dau1 = (AliAODMCParticle *)fArrayMCAOD->At(mcPart->GetDaughterLabel(0));
          auto *dau2 = (AliAODMCParticle *)fArrayMCAOD->At(mcPart->GetDaughterLabel(1));

          AliAODMCParticle *D0meson;
          AliAODMCParticle *softPion;
          if (std::abs(dau1->GetPdgCode()) == 421){
            D0meson = dau1;
            softPion = dau2;
          } else if (std::abs(dau2->GetPdgCode()) == 421) {
            D0meson = dau2;
            softPion = dau1;
          } else {
            continue; // reject D* -> D+pi0
          }

          // select kinem of soft pion
          if (
              softPion->Pt() < ptMin   ||
              softPion->Pt() > ptMax   ||
              softPion->Eta() < etaMin ||
              softPion->Eta() > etaMax
              ){
            continue;
          }

          // select the correct decay channel
          if (D0meson->GetNDaughters() != 2 ) {
            continue;
          }

          auto *D0Dau1 = (AliAODMCParticle *)fArrayMCAOD->At(D0meson->GetDaughterLabel(0));
          auto *D0Dau2 = (AliAODMCParticle *)fArrayMCAOD->At(D0meson->GetDaughterLabel(1));

          AliAODMCParticle *kaonFromD0;
          AliAODMCParticle *pionFromD0;

          if (std::abs(D0Dau1->GetPdgCode()) == 321 && std::abs(D0Dau2->GetPdgCode()) == 211){
            kaonFromD0 = D0Dau1;
            pionFromD0 = D0Dau2;
          } else if (std::abs(D0Dau1->GetPdgCode()) == 211 && std::abs(D0Dau2->GetPdgCode()) == 321) {
            kaonFromD0 = D0Dau2;
            pionFromD0 = D0Dau1;
          } else {
            continue; // reject D0 not decaying into Kpi
          }

          // select D0daughters' kinematics
          if (
            kaonFromD0->Pt()  < ptMin  ||
            kaonFromD0->Pt()  > ptMax  ||
            kaonFromD0->Eta() < etaMin ||
            kaonFromD0->Eta() > etaMax ||
            pionFromD0->Pt()  < ptMin  ||
            pionFromD0->Pt()  > ptMax  ||
            pionFromD0->Eta() < etaMin ||
            pionFromD0->Eta() > etaMax
          ){
            continue;
          }

          // fill histos
          printf("labelll: %d\n", mcPart->GetLabel());
          if (!SelectDmesonOrigin(fArrayMCAOD, mcPart))
            continue;

          part.SetMCParticleRePart(mcPart);

          part.SetIDTracks(softPion->GetLabel());
          part.SetIDTracks(kaonFromD0->GetLabel());
          part.SetIDTracks(pionFromD0->GetLabel());

          // part.SetDauLabels({softPion->GetLabel(), kaonFromD0->GetLabel(), pionFromD0->GetLabel()});
          if (mcpdg == 413) {
            dplus.push_back(part);
            if (fUseTree) {
              dplus_invmass.push_back(part.GetInvMass());
              dplus_pt.push_back(part.GetPt());
              dplus_eta.push_back(part.GetEta()[0]);
              dplus_origin.push_back(part.GetParticleOrigin());
              dplus_daus.push_back({softPion->GetLabel(), kaonFromD0->GetLabel(), pionFromD0->GetLabel()});

            }
          } else if (mcpdg == -413){
            dminus.push_back(part);
            if (fUseTree) {
              dminus_invmass.push_back(part.GetInvMass());
              dminus_pt.push_back(part.GetPt());
              dminus_eta.push_back(part.GetEta()[0]);
              dminus_origin.push_back(part.GetParticleOrigin());
              dminus_daus.push_back({softPion->GetLabel(), kaonFromD0->GetLabel(), pionFromD0->GetLabel()});
            }
          }
          if(fDoDorigPlots){
            FillMCtruthPDGDmeson(fArrayMCAOD, mcPart);
            FillMCtruthQuarkOriginDmeson(fArrayMCAOD, mcPart);
          }
        }
      }

      if ((std::abs(mcpdg)) == absPdgMom && (fDecChannel == kDplustoKpipi)) {
        if((pt>DmesonPtMin) && (pt<DmesonPtMax)) {
          int NDDaughters=(const int)mcPart->GetNDaughters();
          if (NDDaughters == 3) {
            int labelFirstDau = mcPart->GetDaughterLabel(0);
            int labelThirdDau = mcPart->GetDaughterLabel(1);
            int labelSecondDau = labelThirdDau-1;

            AliAODMCParticle *D1 = (AliAODMCParticle *)fArrayMCAOD->At(labelFirstDau);
            AliAODMCParticle *D2 = (AliAODMCParticle *)fArrayMCAOD->At(labelSecondDau);
            AliAODMCParticle *D3 = (AliAODMCParticle *)fArrayMCAOD->At(labelThirdDau);

            std::vector<int> pdgvec = {D1->GetPdgCode(), D2->GetPdgCode(), D3->GetPdgCode()};
            std::vector<double> pTvec = {D1->Pt(), D2->Pt(), D3->Pt()};
            std::vector<double> etavec = {D1->Eta(), D2->Eta(), D3->Eta()};
            int counter = 0;

            for (int i = 0; i<NDDaughters; i++) {
              if ((pTvec.at(i) < ptMax && pTvec.at(i) > ptMin) && (etavec.at(i) > etaMin && etavec.at(i) < etaMax)) {
                counter++;
              }
            }

            if(counter==3){
              std::sort(pdgvec.begin(), pdgvec.end());
              if ((pdgvec.at(0)==-321) && (pdgvec.at(1)==211) && (pdgvec.at(2)==211) && (mcpdg == 411)) {
                if (SelectDmesonOrigin(fArrayMCAOD, mcPart)) {
                  part.SetMCParticleRePart(mcPart);
                  part.SetIDTracks(labelFirstDau);
                  part.SetIDTracks(labelSecondDau);
                  part.SetIDTracks(labelThirdDau);
                  // part.SetDauLabels({labelFirstDau, labelSecondDau, labelThirdDau});
                  dplus.push_back(part);

                  if (fUseTree) {
                    dplus_invmass.push_back(part.GetInvMass());
                    dplus_pt.push_back(part.GetPt());
                    dplus_eta.push_back(part.GetEta()[0]);
                    dplus_origin.push_back(part.GetParticleOrigin());
                    dplus_daus.push_back({labelFirstDau, labelSecondDau, labelThirdDau});
                  }
                  if(fDoDorigPlots){
                    FillMCtruthPDGDmeson(fArrayMCAOD, mcPart);
                    FillMCtruthQuarkOriginDmeson(fArrayMCAOD, mcPart);
                  }
                }
                
              }
              if ((pdgvec.at(0)==-211) && (pdgvec.at(1)==-211) && (pdgvec.at(2)==321) && (mcpdg == -411) ) {
                if (SelectDmesonOrigin(fArrayMCAOD, mcPart)) {
                  part.SetMCParticleRePart(mcPart);
                  part.SetIDTracks(labelFirstDau);
                  part.SetIDTracks(labelSecondDau);
                  part.SetIDTracks(labelThirdDau);
                  // part.SetDauLabels({labelFirstDau, labelSecondDau, labelThirdDau});
                  dminus.push_back(part);
                  if (fUseTree) {
                    dminus_invmass.push_back(part.GetInvMass());
                    dminus_pt.push_back(part.GetPt());
                    dminus_eta.push_back(part.GetEta()[0]);
                    dminus_origin.push_back(part.GetParticleOrigin());
                    dminus_daus.push_back({labelFirstDau, labelSecondDau, labelThirdDau});
                  }
                  if(fDoDorigPlots){
                    FillMCtruthPDGDmeson(fArrayMCAOD, mcPart);
                    FillMCtruthQuarkOriginDmeson(fArrayMCAOD, mcPart);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // D MESON SELECTION
  int nCand = arrayHF->GetEntriesFast();

  // check if the train includes the common ML selector for the given charm-hadron species
  AliAnalysisTaskSECharmHadronMLSelector *taskMLSelect = nullptr;
  std::vector<int> chHadIdx{};
  std::vector<std::vector<double> > scoresFromMLSelector{};
  if(fDependOnMLSelector) {
    taskMLSelect = dynamic_cast<AliAnalysisTaskSECharmHadronMLSelector*>(AliAnalysisManager::GetAnalysisManager()->GetTask(Form("MLSelector%s", mesonName.Data())));
    if(!taskMLSelect) {
      AliFatal("ML Selector not present in train and ML models not compiled!");
      return;
    }

    chHadIdx = taskMLSelect->GetSelectedCandidates();
    scoresFromMLSelector = taskMLSelect->GetMLSCores();
  }
  else {
    for (int iCand = 0; iCand < nCand; iCand++) {
      chHadIdx.push_back(iCand);
      scoresFromMLSelector.push_back({});
    }
  }

  // needed to initialise PID response
  fRDHFCuts->IsEventSelected(fInputEvent);

  AliAODRecoDecayHF *dMeson = nullptr;
  AliAODRecoDecayHF *dMesonWithVtx = nullptr;
  for (size_t iCand = 0; iCand < chHadIdx.size(); iCand++) {

    dMeson = dynamic_cast<AliAODRecoDecayHF*>(arrayHF->UncheckedAt(chHadIdx[iCand]));
    if(fDecChannel == kDstartoKpipi){
      dMesonWithVtx = dynamic_cast<AliAODRecoDecayHF*>(((AliAODRecoCascadeHF *)dMeson)->Get2Prong());
    } else {
      dMesonWithVtx = dMeson;
    }

    bool unsetVtx = false;
    bool recVtx = false;
    AliAODVertex *origOwnVtx = nullptr;

    int pdgDplusDau[3] = {321, 211, 211};
    int isSelected = 1;
    if(fUseMCTruthReco){
      

      TClonesArray *fArrayMCAOD = dynamic_cast<TClonesArray *>(
      fInputEvent->FindListObject(AliAODMCParticle::StdBranchName()));
      
      
      int dMesonLabel;
      if (fDecChannel == kDplustoKpipi) {
        dMesonLabel = dMeson->MatchToMC(411, fArrayMCAOD, 3, pdgDplusDau);
      }
      else if (fDecChannel == kDstartoKpipi){
        int pdgD0Dau[2] = {321, 211};
        int pdgDstarDau[2] = {421, 211};
        dMesonLabel = dynamic_cast<const AliAODRecoCascadeHF *>(dMeson)->MatchToMC(413, 421, pdgDstarDau, pdgD0Dau, fArrayMCAOD, false);
      } else {
        AliFatal("Decay channel not implemented. Exit!");
      }
      if(dMesonLabel < 0){
        continue;
      }
      AliAODMCParticle *mcPart = (AliAODMCParticle *)fArrayMCAOD->At(dMesonLabel);
      if(mcPart->Pt() < DmesonPtMin || mcPart->Pt() > DmesonPtMax) {
        continue;
      }
      if(!SelectDmesonOrigin(fArrayMCAOD, mcPart)) {
        continue;
      }

      if (fApplyML) {
        isSelected = IsCandidateSelected(dMeson, dMesonWithVtx, absPdgMom, unsetVtx, recVtx, origOwnVtx, scoresFromMLSelector[iCand]);
      }
    } else {
      isSelected = IsCandidateSelected(dMeson, dMesonWithVtx, absPdgMom, unsetVtx, recVtx, origOwnVtx, scoresFromMLSelector[iCand]);
    }
    
    if(!isSelected) {
      if (unsetVtx) {
        dMesonWithVtx->UnsetOwnPrimaryVtx();
      }
      if (recVtx) {
        fRDHFCuts->CleanOwnPrimaryVtx(dMesonWithVtx, fInputEvent, origOwnVtx);
      }
      continue;
    }

    double mass;
    if(fDecChannel != kDstartoKpipi) {
      mass = dMeson->InvMass(fDmesonPDGs.size(), &fDmesonPDGs[0]);
    } else {
      mass = dynamic_cast<AliAODRecoCascadeHF *>(dMeson)->DeltaInvMass();
    }

    if (dMeson->Charge() > 0) {
      fHistDplusInvMassPt->Fill(dMeson->Pt(), mass);
    } else {
      fHistDminusInvMassPt->Fill(dMeson->Pt(), mass);
    }


    if( MassSelection(mass, dMeson->Pt(), absPdgMom) ) {
      if (dMeson->Charge() > 0) {
        AliFemtoDreamBasePart dplusCand(dMeson, fInputEvent, absPdgMom, fDmesonPDGs);
        if (fIsMC && fMCBeautyRejection
            && dplusCand.GetParticleOrigin()
                == AliFemtoDreamBasePart::kBeauty) {
          if (gRandom->Uniform() > fMCBeautyScalingFactor)
            continue;
        }
        if (fIsMC && fUseTrueDOnly
            && std::abs(dplusCand.GetMCPDGCode()) != absPdgMom) {
          continue;
        }
        if (!fIsMCtruth) {
          if (fApplyML) {
            dplusCand.SetBkgScore(scoresFromMLSelector[iCand][0]);
            dplusCand.SetPromptScore(scoresFromMLSelector[iCand][1]);
          } else {
            dplusCand.SetBkgScore(-1);
            dplusCand.SetPromptScore(-1);
          }
          dplus.push_back(dplusCand);
        }
        if (!fIsLightweight) {
          fHistDplusInvMassPtSel->Fill(dMeson->Pt(), mass);
          fHistDplusEta->Fill(dMeson->Eta());
          fHistDplusPhi->Fill(dMeson->Phi());
          fHistDplusEtaVsp->Fill(dMeson->P(), dMeson->Eta());
          if (fIsMC) {
            fHistDplusMCPDGPt->Fill(dplusCand.GetPt(),
                                    dplusCand.GetMotherPDG());
            fHistDplusMCOrigin->Fill(dplusCand.GetPt(),
                                     dplusCand.GetParticleOrigin());
            if (dplusCand.GetMCPDGCode() != 0) {
              fHistDplusMCPtRes->Fill(dplusCand.GetPt() - dplusCand.GetMCPt(),
                                      dplusCand.GetPt());
              fHistDplusMCPhiRes->Fill(
                  dplusCand.GetPhi().at(0) - dplusCand.GetMCPhi().at(0),
                  dplusCand.GetPt());
              fHistDplusMCThetaRes->Fill(
                  dplusCand.GetTheta().at(0) - dplusCand.GetMCTheta().at(0),
                  dplusCand.GetPt());
            }
          }
          for (unsigned int iChild = 0; iChild < fDmesonPDGs.size(); iChild++) {
            AliAODTrack *track;
            if (fDecChannel != kDstartoKpipi || iChild == 0) {
              track = (AliAODTrack *) dMeson->GetDaughter(iChild);
            } else {
              track = (AliAODTrack *) dMesonWithVtx->GetDaughter(iChild-1); //D0<-D* daughters
            }
            fHistDplusChildPt[iChild]->Fill(track->Pt());
            fHistDplusChildEta[iChild]->Fill(track->Eta());
            fHistDplusChildPhi[iChild]->Fill(track->Phi());
          }
        }

        if(!fIsMCtruth){
          dplus_invmass.push_back(dplusCand.GetInvMass());
          dplus_pt.push_back(dplusCand.GetPt());
          dplus_eta.push_back(dplusCand.GetEta()[0]);
          dplus_origin.push_back(dplusCand.GetParticleOrigin());
          dplus_daus.push_back(dplusCand.GetIDTracks());
          if (fApplyML) {
            dplus_bkg_score.push_back(scoresFromMLSelector[iCand][0]);
            dplus_prompt_score.push_back(scoresFromMLSelector[iCand][1]);
          }
        }
        
      } else {
        AliFemtoDreamBasePart dminusCand(dMeson, fInputEvent, absPdgMom, fDmesonPDGs);
        if (fIsMC && fMCBeautyRejection && dminusCand.GetParticleOrigin() == AliFemtoDreamBasePart::kBeauty) {
          if (gRandom->Uniform() > fMCBeautyScalingFactor) continue;
        }
        if (fIsMC && fUseTrueDOnly && std::abs(dminusCand.GetMCPDGCode()) != absPdgMom) {
          continue;
        }
        if (!fIsMCtruth){
          if (fApplyML) {
            dminusCand.SetBkgScore(scoresFromMLSelector[iCand][0]);
            dminusCand.SetPromptScore(scoresFromMLSelector[iCand][1]);
          } else {
            dminusCand.SetBkgScore(-1);
            dminusCand.SetPromptScore(-1);
          }
          dminus.push_back(dminusCand);
        }
        if (!fIsLightweight) {
          fHistDminusInvMassPtSel->Fill(dMeson->Pt(), mass);
          fHistDminusEta->Fill(dMeson->Eta());
          fHistDminusPhi->Fill(dMeson->Phi());
          fHistDminusEtaVsp->Fill(dMeson->P(), dMeson->Eta());

          if (fIsMC) {
            fHistDminusMCPDGPt->Fill(dminusCand.GetPt(), dminusCand.GetMotherPDG());
            fHistDminusMCOrigin->Fill(dminusCand.GetPt(), dminusCand.GetParticleOrigin());
            if (dminusCand.GetMCPDGCode() != 0) {
              fHistDminusMCPtRes->Fill(dminusCand.GetPt() - dminusCand.GetMCPt(), dminusCand.GetPt());
              fHistDminusMCPhiRes->Fill(dminusCand.GetPhi().at(0) - dminusCand.GetMCPhi().at(0), dminusCand.GetPt());
              fHistDminusMCThetaRes->Fill(dminusCand.GetTheta().at(0) - dminusCand.GetMCTheta().at(0),
                                          dminusCand.GetPt());
            }
          }
          for (unsigned int iChild = 0; iChild < fDmesonPDGs.size(); iChild++) {
            AliAODTrack *track;
            if (fDecChannel != kDstartoKpipi || iChild == 0) {
              track = (AliAODTrack *) dMeson->GetDaughter(iChild);
            } else {
              track = (AliAODTrack *) dMesonWithVtx->GetDaughter(iChild-1); //D0<-D* daughters
            }
            fHistDminusChildPt[iChild]->Fill(track->Pt());
            fHistDminusChildEta[iChild]->Fill(track->Eta());
            fHistDminusChildPhi[iChild]->Fill(track->Phi());
          }
        }
        if (!fIsMCtruth && fUseTree) {
          dminus_invmass.push_back(dminusCand.GetInvMass());
          dminus_pt.push_back(dminusCand.GetPt());
          dminus_eta.push_back(dminusCand.GetEta()[0]);
          dminus_origin.push_back(dminusCand.GetParticleOrigin());
          dminus_daus.push_back(dminusCand.GetIDTracks());
          if(fApplyML) {
            dminus_bkg_score.push_back(scoresFromMLSelector[iCand][0]);
            dminus_prompt_score.push_back(scoresFromMLSelector[iCand][1]);
          }
        }
      }
    }

    if (unsetVtx) {
      dMeson->UnsetOwnPrimaryVtx();
    }
    if (recVtx) {
      fRDHFCuts->CleanOwnPrimaryVtx(dMeson, fInputEvent, origOwnVtx);
    }
  }


  // for (auto dpl : dplus){
  //   auto idtrks = dpl.GetIDTracks();

  //   for (auto id:idtrks){
  //     AliAODMCParticle *mcDau = (AliAODMCParticle *)fArrayMCAOD->At(id);

//   }
  // }
  // PAIR CLEANING AND FEMTO


  // if (fUseTree && false) {
  //   if (dplus.size() > 0 || dminus.size() > 0) {
  //     printf("len: %d - protons\n", protons.size());
  //     printf("len: %d - protons_nsigtpc\n", protons_nsigtpc.size());
  //     printf("len: %d - protons_nsigtof\n", protons_nsigtof.size());
  //     printf("len: %d - protons_eta\n", protons_eta.size());
  //     printf("len: %d - protons_pt\n", protons_pt.size());
  //     printf("len: %d - protons_ncls\n", protons_ncls.size());
  //     printf("len: %d - protons_ncrossed\n", protons_ncrossed.size());
  //     printf("len: %d - protons_dcaz\n", protons_dcaz.size());
  //     printf("len: %d - protons_dcaxy\n", protons_dcaxy.size());
  //     printf("len: %d - antiprotons\n", antiprotons.size());
  //     printf("len: %d - antiprotons_nsigtpc\n", antiprotons_nsigtpc.size());
  //     printf("len: %d - antiprotons_nsigtof\n", antiprotons_nsigtof.size());
  //     printf("len: %d - antiprotons_eta\n", antiprotons_eta.size());
  //     printf("len: %d - antiprotons_pt\n", antiprotons_pt.size());
  //     printf("len: %d - antiprotons_ncls\n", antiprotons_ncls.size());
  //     printf("len: %d - antiprotons_ncrossed\n", antiprotons_ncrossed.size());
  //     printf("len: %d - antiprotons_dcaz\n", antiprotons_dcaz.size());
  //     printf("len: %d - antiprotons_dcaxy\n\n\n", antiprotons_dcaxy.size());
  //     printf("len: %d - dplus\n", dplus.size());
  //     printf("len: %d - dplus_pt\n", dplus_pt.size());
  //     printf("len: %d - dplus_eta\n", dplus_eta.size());
  //     printf("len: %d - dplus_origin\n", dplus_origin.size());
  //     printf("len: %d - dplus_daus\n", dplus_daus.size());
  //     printf("len: %d - dplus_bkg_score\n", dplus_bkg_score.size());
  //     printf("len: %d - dplus_prompt_score\n", dplus_prompt_score.size());
  //     printf("len: %d - dplus_invmass\n", dplus_invmass.size());
  //     printf("len: %d - dminus\n", dminus.size());
  //     printf("len: %d - dminus_pt\n", dminus_pt.size());
  //     printf("len: %d - dminus_eta\n", dminus_eta.size());
  //     printf("len: %d - dminus_origin\n", dminus_origin.size());
  //     printf("len: %d - dminus_daus\n", dminus_daus.size());
  //     printf("len: %d - dminus_bkg_score\n", dminus_bkg_score.size());
  //     printf("len: %d - dminus_prompt_score\n", dminus_prompt_score.size());
  //   }
  // }

  // for (auto &dm: dplus) {
  //   auto orig = dm.GetParticleOrigin();
  //   printf("orig -- %d\n", orig);
  //   if (orig == AliFemtoDreamBasePart::kPhysPrimary)
  //     dm.SetUse(false);
  //   else
  //     dm.SetUse(true);
  // }

  // set event properties
  for (auto &dmeson : dplus) {
    dmeson.SetMult(fEvent->GetMultiplicity());
    dmeson.SetZVtx(fEvent->GetZVertex());
  }
  
  for (auto &dmeson : dplus) {
    int m = dmeson.GetMult();
  }
  for (auto &dmeson : dminus) {
    dmeson.SetMult(fEvent->GetMultiplicity());
    dmeson.SetZVtx(fEvent->GetZVertex());
  }
  
  for (auto &proton : protons) {
    proton.SetMult(fEvent->GetMultiplicity());
    proton.SetZVtx(fEvent->GetZVertex());
  }
  
  for (auto &proton : antiprotons) {
    proton.SetMult(fEvent->GetMultiplicity());
    proton.SetZVtx(fEvent->GetZVertex());
  }

  fPairCleaner->CleanTrackAndDecay(&protons, &dplus, 0, true);
  fPairCleaner->CleanTrackAndDecay(&protons, &dminus, 1, true);
  fPairCleaner->CleanTrackAndDecay(&antiprotons, &dplus, 2, true);
  fPairCleaner->CleanTrackAndDecay(&antiprotons, &dminus, 3, true);

  fPairCleaner->StoreParticle(protons);
  fPairCleaner->StoreParticle(antiprotons);

  fPairCleaner->StoreParticle(dplus);
  fPairCleaner->StoreParticle(dminus);

  if (fUseTree) {
    fPartColl->SetEvent(fPairCleaner->GetCleanParticles(), fEvent, fPairTreeSE, fPairTreeME);

  } else {
    fPartColl->SetEvent(fPairCleaner->GetCleanParticles(), fEvent);
  }

  // flush the data
  PostData(1, fQA);
  PostData(2, fEvtHistList);
  PostData(3, fTrackCutHistList);
  PostData(4, fAntiTrackCutHistList);
  PostData(5, fDChargedHistList);
  PostData(6, fResultList);
  PostData(7, fResultQAList);

  int nOutput = 9;
  if (fIsMC) {
    PostData(nOutput++, fTrackCutHistMCList);
    PostData(nOutput++, fAntiTrackCutHistMCList);
  }
  
  if (fUseTree) {
    for (auto pair : *fPairTreeSE) PostData(nOutput++, pair.second);
    for (auto pair : *fPairTreeME) PostData(nOutput++, pair.second);
  }
}

//____________________________________________________________________________________________________
void AliAnalysisTaskCharmingFemto::ResetGlobalTrackReference() {
  // see AliFemtoDreamAnalysis for details
  for (int i = 0; i < fTrackBufferSize; i++) {
    fGTI[i] = 0;
  }
}

//____________________________________________________________________________________________________
void AliAnalysisTaskCharmingFemto::StoreGlobalTrackReference(
    AliAODTrack *track) {
  const int trackID = track->GetID();
  if (trackID < 0) {
    return;
  }

  if (trackID >= fTrackBufferSize) {
    printf("Warning: track ID too big for buffer: ID: %d, buffer %d\n", trackID,
           fTrackBufferSize);
    return;
  }

  if (fGTI[trackID]) {
    if ((!track->GetFilterMap()) && (!track->GetTPCNcls())) {
      return;
    }
    if (fGTI[trackID]->GetFilterMap() || fGTI[trackID]->GetTPCNcls()) {
      printf("Warning! global track info already there!");
      printf("         TPCNcls track1 %u track2 %u",
             (fGTI[trackID])->GetTPCNcls(), track->GetTPCNcls());
      printf("         FilterMap track1 %u track2 %u\n",
             (fGTI[trackID])->GetFilterMap(), track->GetFilterMap());
    }
  }
  (fGTI[trackID]) = track;
}

//____________________________________________________________________________________________________
void AliAnalysisTaskCharmingFemto::UserCreateOutputObjects() {
  if (fUseTree) {
    fPairTreeSE = new std::map <std::pair<int, int>, TTree*>();
    fPairTreeSE->insert({{0, 2}, new TTree("tSE_pp", "tSE_pp")});
    fPairTreeSE->insert({{1, 3}, new TTree("tSE_mm", "tSE_mm")});
    fPairTreeSE->insert({{0, 3}, new TTree("tSE_mp", "tSE_mp")});
    fPairTreeSE->insert({{1, 2}, new TTree("tSE_pm", "tSE_pm")});

    fPairTreeME = new std::map <std::pair<int, int>, TTree*>();
    fPairTreeME->insert({{0, 2}, new TTree("tME_pp", "tME_pp")});
    fPairTreeME->insert({{1, 3}, new TTree("tME_mm", "tME_mm")});
    fPairTreeME->insert({{0, 3}, new TTree("tME_mp", "tME_mp")});
    fPairTreeME->insert({{1, 2}, new TTree("tME_pm", "tME_pm")});

    for (auto tree : *fPairTreeSE) {
      // event
      tree.second->Branch("mult", &dummyint);
      tree.second->Branch("vz", &dummyfloat);

      // pair
      tree.second->Branch("kStar", &dummyfloat);

      
      // heavy particle
      tree.second->Branch("heavy_invmass", &dummyfloat);
      tree.second->Branch("heavy_pt", &dummyfloat);
      tree.second->Branch("heavy_eta", &dummyfloat);
      tree.second->Branch("heavy_origin", &dummyint);
      tree.second->Branch("heavy_daus", &dummyvector);
      tree.second->Branch("heavy_bkg_score", &dummyfloat);
      tree.second->Branch("heavy_prompt_score", &dummyfloat);

      // light particle
      tree.second->Branch("light_pt", &dummyfloat);
      tree.second->Branch("light_eta", &dummyfloat);
      tree.second->Branch("light_nsigtpc", &dummyfloat);
      tree.second->Branch("light_nsigtof", &dummyfloat);
      tree.second->Branch("light_ncls", &dummyint);
      tree.second->Branch("light_ncrossed", &dummyint);
      tree.second->Branch("light_dcaz", &dummyfloat);
      tree.second->Branch("light_dcaxy", &dummyfloat);
    }

    for (auto tree : *fPairTreeME) {
      // event
      tree.second->Branch("mult", &dummyint);
      tree.second->Branch("vz", &dummyfloat);

      // pair
      tree.second->Branch("kStar", &dummyfloat);


      // struct dummyStruct {
      //   std::vector<int> dummyVector;
      // };

      // dummyStruct myDummyStruct;
      // // heavy
      tree.second->Branch("heavy_invmass", &dummyfloat);
      tree.second->Branch("heavy_pt", &dummyfloat);
      tree.second->Branch("heavy_eta", &dummyfloat);
      tree.second->Branch("heavy_origin", &dummyint);
      tree.second->Branch("heavy_daus", &dummyvector);
      tree.second->Branch("heavy_bkg_score", &dummyfloat);
      tree.second->Branch("heavy_prompt_score", &dummyfloat);
      // light
      tree.second->Branch("light_eta", &dummyfloat);
      tree.second->Branch("light_pt", &dummyfloat);
      tree.second->Branch("light_nsigtpc", &dummyfloat);
      tree.second->Branch("light_nsigtof", &dummyfloat);
      tree.second->Branch("light_ncls", &dummyint);
      tree.second->Branch("light_ncrossed", &dummyint);
      tree.second->Branch("light_dcaz", &dummyfloat);
      tree.second->Branch("light_dcaxy", &dummyfloat);
    }
  }

  fGTI = new AliAODTrack *[fTrackBufferSize];

  fEvent = new AliFemtoDreamEvent(true, !fIsLightweight, fTrigger);

  fProtonTrack = new AliFemtoDreamTrack();
  fProtonTrack->SetUseMCInfo(fIsMC);

  fPairCleaner = new AliFemtoDreamPairCleaner(4, 0,
                                              fConfig->GetMinimalBookingME());
  fPartColl = new AliFemtoDreamPartCollection(fConfig,
                                              fConfig->GetMinimalBookingME());

  fQA = new TList();
  fQA->SetName("QA");
  fQA->SetOwner(kTRUE);

  if (fEvtCuts) {
    fEvtCuts->InitQA();
    if (fEvent->GetEvtCutList() && !fIsLightweight) {
      fQA->Add(fEvent->GetEvtCutList());
    }
    if (fEvtCuts->GetHistList() && !fIsLightweight) {
      fEvtHistList = fEvtCuts->GetHistList();
    } else {
      fEvtHistList = new TList();
      fEvtHistList->SetName("EvtCuts");
      fEvtHistList->SetOwner(true);
    }
  } else {
    AliWarning("Event cuts are missing! \n");
  }

  if (!fConfig->GetMinimalBookingME() && fPairCleaner
      && fPairCleaner->GetHistList()) {
    fQA->Add(fPairCleaner->GetHistList());
  }

  fTrackCutsPartProton->Init("Proton");
  // necessary for the non-min booking case
  fTrackCutsPartProton->SetName("Proton");

  if (fTrackCutsPartProton && fTrackCutsPartProton->GetQAHists()) {
    fTrackCutHistList = fTrackCutsPartProton->GetQAHists();
    fHistBuddyplusEtaVsp = new TH2F("fHistEtaVsp",
                             ";p;#eta", 200, 0, 20, 100, -1, 1);
    fTrackCutHistList->Add(fHistBuddyplusEtaVsp);
    if (fIsMC && fTrackCutsPartProton->GetMCQAHists()
        && fTrackCutsPartProton->GetIsMonteCarlo()) {
      fTrackCutHistMCList = fTrackCutsPartProton->GetMCQAHists();
    }
  }

  fTrackCutsPartAntiProton->Init("Anti-proton");
  // necessary for the non-min booking case
  fTrackCutsPartAntiProton->SetName("Anti-proton");

  if (fTrackCutsPartAntiProton && fTrackCutsPartAntiProton->GetQAHists()) {
    fAntiTrackCutHistList = fTrackCutsPartAntiProton->GetQAHists();
    fHistBuddyminusEtaVsp = new TH2F("fHistEtaVsp",
                             ";p;#eta", 200, 0, 20, 100, -1, 1);
    fAntiTrackCutHistList->Add(fHistBuddyminusEtaVsp);
    if (fIsMC && fTrackCutsPartAntiProton->GetMCQAHists()
        && fTrackCutsPartAntiProton->GetIsMonteCarlo()) {
      fAntiTrackCutHistMCList = fTrackCutsPartAntiProton->GetMCQAHists();
    }
  }

  // Eventually we might put this in a separate class but for the moment let's just leave it floating around here
  fDChargedHistList  = new TList();
  fDChargedHistList->SetName("DChargedQA");
  fDChargedHistList->SetOwner(true);

  TString nameD;
  TString nameDminus;
  TString nameInvMass;
  Int_t InvMassBins;
  Double_t LowerInvMass;
  Double_t UpperInvMass;
  switch (fDecChannel) {
    case kDplustoKpipi:
      nameD = "Dplus";
      nameDminus = "Dminus";
      nameInvMass = "#it{M}_{K#pi#pi}";
      InvMassBins = 100;
      LowerInvMass = 1.77;
      UpperInvMass = 1.97;
      break;
    case kDstartoKpipi:
      nameD = "Dstar";
      nameDminus = "Dstarminus";
      nameInvMass = "#it{M}_{K#pi#pi} - #it{M}(K#pi)";
      InvMassBins = 500;
      LowerInvMass = 0.138;
      UpperInvMass = 0.160;
      break;
  }

  fHistDplusInvMassPt = new TH2F(
      TString::Format("fHist%sInvMassPt", nameD.Data()),
                                 TString::Format("; #it{p}_{T} (GeV/#it{c}); %s (GeV/#it{c}^{2})", nameInvMass.Data()),
                                 100, 0, 10, InvMassBins, LowerInvMass, UpperInvMass);
  fDChargedHistList->Add(fHistDplusInvMassPt);
  if (!fIsLightweight) {
    fHistDplusInvMassPtSel = new TH2F(
        TString::Format("fHist%sInvMassPtSel", nameD.Data()),
        TString::Format("; #it{p}_{T} (GeV/#it{c}); %s (GeV/#it{c}^{2})", nameInvMass.Data()),
        100, 0, 10, InvMassBins, LowerInvMass, UpperInvMass);
    fDChargedHistList->Add(fHistDplusInvMassPtSel);
    fHistDplusEta = new TH1F(TString::Format("fHist%sEta", nameD.Data()),
                             ";#eta; Entries", 100, -1, 1);
    fDChargedHistList->Add(fHistDplusEta);
    fHistDplusPhi = new TH1F(TString::Format("fHist%sPhi", nameD.Data()),
                             ";#phi; Entries", 100, 0., 2. * TMath::Pi());
    fDChargedHistList->Add(fHistDplusPhi);
    fHistDplusEtaVsp = new TH2F(TString::Format("fHist%sEtaVsp", nameD.Data()),
                             ";p;#eta", 200, 0, 20, 100, -1, 1);
    fDChargedHistList->Add(fHistDplusEtaVsp);

    if (fIsMC) {
      fHistDplusMCPDGPt = new TH2F(
          TString::Format("fHist%sMCPDGPt", nameD.Data()),
          "; #it{p}_{T} (GeV/#it{c}); PDG Code mother",
          250, 0, 25, 5000, 0, 5000);
      fHistDplusMCPtRes = new TH2F(
          TString::Format("fHist%sMCPtRes", nameD.Data()),
          "; #it{p}_{T, rec} - #it{p}_{T, gen} (GeV/#it{c}); #it{p}_{T} (GeV/#it{c})",
                                   101, -0.5, 0.5, 100, 0, 10);
      fHistDplusMCPhiRes = new TH2F(
          TString::Format("fHist%sMCPhiRes", nameD.Data()),
          "; #phi_{rec} - #phi_{gen}; #it{p}_{T} (GeV/#it{c})", 101, -0.025, 0.025,
          100, 0, 10);
      fHistDplusMCThetaRes = new TH2F(
          TString::Format("fHist%sMCThetaRes", nameD.Data()),
          "; #theta_{rec} - #theta_{gen}; #it{p}_{T} (GeV/#it{c})", 101, -0.025,
          0.025, 100, 0, 10);
      fHistDplusMCOrigin = new TH2F(TString::Format("fHist%sMCOrigin", nameD.Data()),
                                    "; #it{p}_{T} (GeV/#it{c}); Origin", 100, 0,
                                    10, 7, 0, 7);
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(1, "kPhysPrimary");
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(2, "kWeak");
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(3, "kMaterial");
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(4, "kFake");
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(5, "kContamination");
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(6, "kUnknown");
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(7, "kBeauty");
      fDChargedHistList->Add(fHistDplusMCPDGPt);
      fDChargedHistList->Add(fHistDplusMCPtRes);
      fDChargedHistList->Add(fHistDplusMCPhiRes);
      fDChargedHistList->Add(fHistDplusMCThetaRes);
      fDChargedHistList->Add(fHistDplusMCOrigin);
    }
  }

  //MCTruth Histos

  if (fIsMC && fIsMCtruth) {
      
      fHistDplusMCtruthmotherPDG = new TH2F(
          TString::Format("fHist%sMCPDGPt_MCtruth", nameD.Data()),
          "; #it{p}_{T} (GeV/#it{c}); PDG Code mother",
          250, 0, 25, 5000, 0, 5000);

    fHistDplusMCtruthQuarkOrigin = new TH2F(TString::Format("fHist%sMCQuarkOrigin_MCtruth", nameD.Data()),
                                    "; #it{p}_{T} (GeV/#it{c}); Origin", 100, 0,
                                    10, 3, 0.5, 3.5);
    fHistDplusMCtruthQuarkOrigin->GetYaxis()->SetBinLabel(1, "Charm");
    fHistDplusMCtruthQuarkOrigin->GetYaxis()->SetBinLabel(2, "Beauty");
    fHistDplusMCtruthQuarkOrigin->GetYaxis()->SetBinLabel(3, "else");

    fDChargedHistList->Add(fHistDplusMCtruthmotherPDG);
    fDChargedHistList->Add(fHistDplusMCtruthQuarkOrigin);

      fHistDminusMCtruthmotherPDG = new TH2F(
          TString::Format("fHist%sMCPDGPt_MCtruth", nameDminus.Data()),
          "; #it{p}_{T} (GeV/#it{c}); PDG Code mother",
          250, 0, 25, 5000, 0, 5000);

    fHistDminusMCtruthQuarkOrigin = new TH2F(TString::Format("fHist%sMCQuarkOrigin_MCtruth", nameDminus.Data()),
                                    "; #it{p}_{T} (GeV/#it{c}); Origin", 100, 0,
                                    10, 3, 0.5, 3.5);
    fHistDminusMCtruthQuarkOrigin->GetYaxis()->SetBinLabel(1, "Charm");
    fHistDminusMCtruthQuarkOrigin->GetYaxis()->SetBinLabel(2, "Beauty");
    fHistDminusMCtruthQuarkOrigin->GetYaxis()->SetBinLabel(3, "else");

    fDChargedHistList->Add(fHistDminusMCtruthmotherPDG);
    fDChargedHistList->Add(fHistDminusMCtruthQuarkOrigin);
  }

  fHistDminusInvMassPt = new TH2F(
      TString::Format("fHist%sInvMassPt", nameDminus.Data()),
                                  TString::Format("; #it{p}_{T} (GeV/#it{c}); %s (GeV/#it{c}^{2})", nameInvMass.Data()),
                                  100, 0, 10, InvMassBins, LowerInvMass, UpperInvMass);
  fDChargedHistList->Add(fHistDminusInvMassPt);
  if (!fIsLightweight) {
    fHistDminusInvMassPtSel = new TH2F(
        TString::Format("fHist%sInvMassPtSel", nameDminus.Data()),
        TString::Format("; #it{p}_{T} (GeV/#it{c}); %s (GeV/#it{c}^{2})", nameInvMass.Data()),
        100, 0, 10, InvMassBins, LowerInvMass, UpperInvMass);
    fDChargedHistList->Add(fHistDminusInvMassPtSel);
    fHistDminusEta = new TH1F(TString::Format("fHist%sEta", nameDminus.Data()),
                              ";#eta; Entries", 100, -1, 1);
    fDChargedHistList->Add(fHistDminusEta);
    fHistDminusPhi = new TH1F(TString::Format("fHist%sPhi", nameDminus.Data()),
                              ";#phi; Entries", 100, 0., 2. * TMath::Pi());
    fDChargedHistList->Add(fHistDminusPhi);
    fHistDminusEtaVsp = new TH2F(TString::Format("fHist%sEtaVsp", nameD.Data()),
                             ";p;#eta", 2000, 0, 20, 400, -2, 2);
    fDChargedHistList->Add(fHistDminusEtaVsp);

    if (fIsMC) {
      fHistDminusMCPDGPt = new TH2F(
          TString::Format("fHist%sMCPDGPt", nameDminus.Data()),
          "; #it{p}_{T} (GeV/#it{c}); PDG Code mother",
          250, 0, 25, 5000, 0, 5000);
      fHistDminusMCPtRes = new TH2F(
          TString::Format("fHist%sMCPtRes", nameDminus.Data()),
          "; #it{p}_{T, rec} - #it{p}_{T, gen} (GeV/#it{c}); #it{p}_{T} (GeV/#it{c})",
                                    101, -0.5, 0.5, 100, 0, 10);
      fHistDminusMCPhiRes = new TH2F(
          TString::Format("fHist%sMCPhiRes", nameDminus.Data()),
          "; #phi_{rec} - #phi_{gen}; #it{p}_{T} (GeV/#it{c})", 101, -0.025, 0.025,
          100, 0, 10);
      fHistDminusMCThetaRes = new TH2F(
          TString::Format("fHist%sMCThetaRes", nameDminus.Data()),
          "; #theta_{rec} - #theta_{gen}; #it{p}_{T} (GeV/#it{c})", 101, -0.025,
          0.025, 100, 0, 10);
      fHistDminusMCOrigin = new TH2F(TString::Format("fHist%sMCOrigin", nameDminus.Data()),
                                     "; #it{p}_{T} (GeV/#it{c}); Origin", 100,
                                     0, 10, 7, 0, 7);
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(1, "kPhysPrimary");
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(2, "kWeak");
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(3, "kMaterial");
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(4, "kFake");
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(5, "kContamination");
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(6, "kUnknown");
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(7, "kBeauty");
      fDChargedHistList->Add(fHistDminusMCPDGPt);
      fDChargedHistList->Add(fHistDminusMCPtRes);
      fDChargedHistList->Add(fHistDminusMCPhiRes);
      fDChargedHistList->Add(fHistDminusMCThetaRes);
      fDChargedHistList->Add(fHistDminusMCOrigin);
    }
  }


  if (!fIsLightweight) {
    std::vector<TString> nameVec;
    if (fDecChannel == kDplustoKpipi) {
      nameVec = { {"K", "Pi1", "Pi2"}};
    } else if (fDecChannel == kDstartoKpipi) {
      nameVec = { {"Pi1", "K", "Pi2"}};
    }
    for (unsigned int iChild = 0; iChild < fDmesonPDGs.size(); ++iChild) {
      fHistDplusChildPt[iChild] = new TH1F(
          TString::Format("fHist%sChildPt_%s", nameD.Data(), nameVec.at(iChild).Data()),
                   "; #it{p}_{T} (GeV/#it{c}); Entries", 250, 0, 25);
      fHistDplusChildEta[iChild] = new TH1F(
          TString::Format("fHist%sChildEta_%s", nameD.Data(), nameVec.at(iChild).Data()),
          "; #eta; Entries", 100, -1, 1);
      fHistDplusChildPhi[iChild] = new TH1F(
          TString::Format("fHist%sChildPhi_%s", nameD.Data(), nameVec.at(iChild).Data()),
          "; #phi; Entries", 100, 0, 2. * TMath::Pi());
      fDChargedHistList->Add(fHistDplusChildPt[iChild]);
      fDChargedHistList->Add(fHistDplusChildEta[iChild]);
      fDChargedHistList->Add(fHistDplusChildPhi[iChild]);
      fHistDminusChildPt[iChild] = new TH1F(
          TString::Format("fHist%sChildPt_%s", nameDminus.Data(), nameVec.at(iChild).Data()),
                   "; #it{p}_{T} (GeV/#it{c}); Entries", 250, 0, 25);
      fHistDminusChildEta[iChild] = new TH1F(
          TString::Format("fHist%sChildEta_%s", nameDminus.Data(), nameVec.at(iChild).Data()),
                   "; #eta; Entries", 100, -1, 1);
      fHistDminusChildPhi[iChild] = new TH1F(
          TString::Format("fHist%sChildPhi_%s", nameDminus.Data(), nameVec.at(iChild).Data()),
                   "; #phi; Entries", 100, 0, 2. * TMath::Pi());
      fDChargedHistList->Add(fHistDminusChildPt[iChild]);
      fDChargedHistList->Add(fHistDminusChildEta[iChild]);
      fDChargedHistList->Add(fHistDminusChildPhi[iChild]);
    }
  }


  if (fPartColl && fPartColl->GetHistList()) {
    fResultList = fPartColl->GetHistList();
  }
  if (!fConfig->GetMinimalBookingME() && fPartColl && fPartColl->GetQAList()) {
    fResultQAList = fPartColl->GetQAList();
  } else {
    fResultQAList = new TList();
    fResultQAList->SetName("ResultsQA");
    fResultQAList->SetOwner(true);
  }

  //ML model
  if(fApplyML) {
    if(!fDependOnMLSelector) {
      switch(fDecChannel) {
        case kDplustoKpipi:
            fMLResponse = new AliHFMLResponseDplustoKpipi("DplustoKpipiMLResponse", "DplustoKpipiMLResponse", fConfigPath.Data());
          fMLResponse->MLResponseInit();
          break;
        case kDstartoKpipi:
            fMLResponse = new AliHFMLResponseDstartoD0pi("DstartoD0piMLResponse", "DstartoD0piMLResponse", fConfigPath.Data());
          fMLResponse->MLResponseInit();
          break;
      }
    }
    else {
      std::string configLocalPath = AliMLModelHandler::ImportFile(fConfigPath.Data());
      YAML::Node nodeList;
      try {
        nodeList = YAML::LoadFile(configLocalPath);
      } catch (std::exception &e) {
        AliFatal(Form("Yaml-ccp error: %s! Exit", e.what()));
      }
      fPtLimsML = nodeList["BINS"].as<vector<float> >();

      for (const auto &model : nodeList["MODELS"]) {
        fMLScoreCuts.push_back(model["cut"].as<std::vector<double> >());
        fMLOptScoreCuts.push_back(model["cut_opt"].as<std::vector<std::string> >());
      }
    }
  }

  PostData(1, fQA);
  PostData(2, fEvtHistList);
  PostData(3, fTrackCutHistList);
  PostData(4, fAntiTrackCutHistList);
  PostData(5, fDChargedHistList);
  PostData(6, fResultList);
  PostData(7, fResultQAList);
  int nOutput = 9;
  if (fIsMC) {
    PostData(nOutput++, fTrackCutHistMCList);
    PostData(nOutput++, fAntiTrackCutHistMCList);
  }
  if (fUseTree) {
    for (auto pair : *fPairTreeSE) {
      PostData(nOutput++, pair.second);
    }
    for (auto pair : *fPairTreeME) {
      PostData(nOutput++, pair.second);
    }
  }
}

//________________________________________________________________________
int AliAnalysisTaskCharmingFemto::IsCandidateSelected(AliAODRecoDecayHF *&dMeson, AliAODRecoDecayHF *&dMesonWithVtx, int absPdgMom, bool &unsetVtx, bool &recVtx, AliAODVertex *&origOwnVtx, std::vector<double> &scores) {
  
  if(!dMeson || !dMesonWithVtx) {
    return 0;
  }

  bool isSelBit = true;
  switch(fDecChannel) {
    case kDplustoKpipi:
      isSelBit = dMeson->HasSelectionBit(AliRDHFCuts::kDplusCuts);
      if(!isSelBit) {
        return 0;
      }
      break;
    case kDstartoKpipi:
      isSelBit = dMeson->HasSelectionBit(AliRDHFCuts::kDstarCuts);
      if(!isSelBit) {
        return 0;
      }
      break;
  }

  unsetVtx = false;
  if (!dMesonWithVtx->GetOwnPrimaryVtx())
  {
    dMesonWithVtx->SetOwnPrimaryVtx(dynamic_cast<AliAODVertex *>(fInputEvent->GetPrimaryVertex()));
    unsetVtx = true;
    // NOTE: the own primary vertex should be unset, otherwise there is a memory leak
    // Pay attention if you use continue inside this loop!!!
  }

  double ptD = dMeson->Pt();
  double yD = dMeson->Y(absPdgMom);
  int ptbin = fRDHFCuts->PtBin(ptD);
  if(ptbin < 0) {
    if (unsetVtx) {
      dMesonWithVtx->UnsetOwnPrimaryVtx();
    }
    return 0;
  }

  bool isFidAcc = fRDHFCuts->IsInFiducialAcceptance(ptD, yD);
  if(!isFidAcc) {
    if (unsetVtx) {
      dMesonWithVtx->UnsetOwnPrimaryVtx();
    }
    return 0;
  }

  int isSelected = fRDHFCuts->IsSelected(dMeson, AliRDHFCuts::kAll, fInputEvent);
  if(!isSelected) {
    if (unsetVtx) {
      dMesonWithVtx->UnsetOwnPrimaryVtx();
    }
    return 0;
  }

  // ML application
  std::vector<double> modelPred{};
  if(fApplyML) {
    if(!fDependOnMLSelector) { //direct application
      AliAODPidHF* pidHF = fRDHFCuts->GetPidHF();
      bool isMLsel = true;
      switch(fDecChannel) {
        case kDplustoKpipi:
          isMLsel = fMLResponse->IsSelectedMultiClass(modelPred, dMeson, fInputEvent->GetMagneticField(), pidHF);
          if(!isMLsel) {
            isSelected = 0;
          }
          break;
        case kDstartoKpipi:
          isMLsel = fMLResponse->IsSelectedMultiClass(modelPred, dMeson, fInputEvent->GetMagneticField(), pidHF);
          if(!isMLsel) {
            isSelected = 0;
          }
          break;
      }
      scores = modelPred;
    }
    else { // read result from common task
      std::vector<float>::iterator low = std::lower_bound(fPtLimsML.begin(), fPtLimsML.end(), ptD);
      int bin = low - fPtLimsML.begin() - 1;
      if(bin < 0)
        bin = 0;
      else if(bin > fPtLimsML.size()-2)
        bin = fPtLimsML.size()-2;
      for(size_t iScore = 0; iScore < scores.size(); iScore++) {
        if((fMLOptScoreCuts[bin][iScore] == "upper" && scores[iScore] > fMLScoreCuts[bin][iScore]) || (fMLOptScoreCuts[bin][iScore] == "lower" && scores[iScore] < fMLScoreCuts[bin][iScore])){
          isSelected = 0;
          break;
        }
      }
    }
  } else {
    scores = modelPred;
  }

  recVtx = false;
  origOwnVtx = nullptr;

  if (fRDHFCuts->GetIsPrimaryWithoutDaughters())
  {
    if (dMesonWithVtx->GetOwnPrimaryVtx()) {
      origOwnVtx = new AliAODVertex(*dMesonWithVtx->GetOwnPrimaryVtx());
    }
    if (fRDHFCuts->RecalcOwnPrimaryVtx(dMesonWithVtx, fInputEvent)) {
      recVtx = true;
    }
    else {
      fRDHFCuts->CleanOwnPrimaryVtx(dMesonWithVtx, fInputEvent, origOwnVtx);
    }
  }

  return isSelected;
}

//____________________________________________________________________________________________________
bool AliAnalysisTaskCharmingFemto::MassSelection(const double mass,
                                                 const double pt,
                                                 const int pdg) {
  if (fMassSelectionType == kAny) {
    return true;
  }

  // simple parametrisation from D+ in 5.02 TeV
  double massMean = TDatabasePDG::Instance()->GetParticle(pdg)->Mass() + 0.0025;  // mass shift observed in all Run2 data samples for all
                                                                                  // D-meson species
  double massWidth = 0.;
  switch (fDecChannel) {
    case kDplustoKpipi:
      if (fSystem == kpp5TeV) {
        massWidth = 0.0057 + pt * 0.00066;
      } else if (fSystem == kpp13TeV) {
        massWidth = 0.006758 + pt * 0.0005124;
      }
      break;
    case kDstartoKpipi:
      Double_t mDstarPDG = TDatabasePDG::Instance()->GetParticle(413)->Mass();
      Double_t mD0PDG = TDatabasePDG::Instance()->GetParticle(421)->Mass();
      massMean = mDstarPDG-mD0PDG; // no extra mass shift because it is deltamass
      if (fSystem == kpp5TeV) {
        massWidth = 0.00105236 - pt * 0.000255556 + pt * pt * 3.2264e-05;
        if(pt > 4 && pt < 5) massWidth = 0.000606852 - 0.000015123 * pt;
        else if(pt >= 5) massWidth = 0.000476887 + pt * 1.087e-05;
      } else if (fSystem == kpp13TeV) {
        massWidth = 0.00124673 - pt * 0.000340426 + pt * pt * 4.40729e-05;
        if(pt > 4 && pt < 5) massWidth = 0.00104329 - 0.000113275 * pt;
        else if(pt >= 5) massWidth = 0.000519861 - 8.58874e-06 * pt;
      }
      break;
  }

  // select D mesons mass window
  if (fMassSelectionType == kSignal) {
    fLowerMassSelection = massMean - fNSigmaMass * massWidth;
    fUpperMassSelection = massMean + fNSigmaMass * massWidth;
  } else if ( fMassSelectionType == kSidebandLeft) {
    fLowerMassSelection = massMean - fNSigmaOffsetSideband * massWidth - fSidebandWidth;
    fUpperMassSelection = massMean - fNSigmaOffsetSideband * massWidth;
  } else if ( fMassSelectionType == kSidebandRight) {
    fLowerMassSelection = massMean + fNSigmaOffsetSideband * massWidth;
    fUpperMassSelection = massMean + fNSigmaOffsetSideband * massWidth + fSidebandWidth;

    if(fDecChannel == kDplustoKpipi){
      // additional removal of D*
      if ( mass > fLowerDstarRemoval && mass < fUpperDstarRemoval) {
        return false;
      }
    }
  }


  if (mass > fLowerMassSelection && mass < fUpperMassSelection) {
    return true;
  }

  return false;
}
