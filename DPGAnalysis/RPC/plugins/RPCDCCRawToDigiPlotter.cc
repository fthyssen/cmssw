#include "DPGAnalysis/RPC/plugins/RPCDCCRawToDigiPlotter.h"

#include <map>

#include "TCanvas.h"
#include "TText.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DPGAnalysis/Tools/interface/DivergingPalette.h"

RPCDCCRawToDigiPlotter::RPCDCCRawToDigiPlotter(edm::ParameterSet const & _config)
    : first_run_(true)
    , output_pdf_(_config.getParameter<std::string>("PDFOutput") + ".pdf")
    , create_pdf_(_config.getParameter<bool>("CreatePDF"))
{
    std::string _analyser(_config.getParameter<std::string>("RPCDCCRawToDigi"));

    record_fed_token_
        = consumes<MergeTH2D, edm::InRun>(edm::InputTag(_analyser, "RecordFed"));
    error_fed_token_
        = consumes<MergeTH2D, edm::InRun>(edm::InputTag(_analyser, "ErrorFed"));
    cd_fed_dcci_tbi_token_
        = consumes<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>(edm::InputTag(_analyser, "CdFedDcciTbi"));
    invalid_fed_dcci_tbi_token_
        = consumes<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>(edm::InputTag(_analyser, "InvalidFedDcciTbi"));
    eod_fed_dcci_tbi_token_
        = consumes<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>(edm::InputTag(_analyser, "EodFedDcciTbi"));
    rddm_fed_dcci_tbi_token_
        = consumes<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>(edm::InputTag(_analyser, "RddmFedDcciTbi"));
    rcdm_fed_dcci_tbi_token_
        = consumes<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>(edm::InputTag(_analyser, "RcdmFedDcciTbi"));
}

void RPCDCCRawToDigiPlotter::fillDescriptions(edm::ConfigurationDescriptions & _descs)
{
    edm::ParameterSetDescription _desc;

    _desc.add<std::string>("RPCDCCRawToDigi", "RPCDCCRawToDigi");

    _desc.add<std::string>("PDFOutput", "RPCDCCRawToDigi");
    _desc.add<bool>("CreatePDF", false);

    _descs.add("RPCDCCRawToDigiPlotter", _desc);
}

void RPCDCCRawToDigiPlotter::beginJob()
{}

void RPCDCCRawToDigiPlotter::beginRun(edm::Run const &, edm::EventSetup const &)
{}

void RPCDCCRawToDigiPlotter::analyze(edm::Event const &, edm::EventSetup const &)
{}

void RPCDCCRawToDigiPlotter::endRun(edm::Run const & _run, edm::EventSetup const &)
{
    edm::Handle<MergeTH2D> _record_fed, _error_fed;
    edm::Handle<MergeMap<::uint32_t, MergeTH2D> > _cd_fed_dcci_tbi, _invalid_fed_dcci_tbi, _eod_fed_dcci_tbi
        , _rddm_fed_dcci_tbi, _rcdm_fed_dcci_tbi;


    _run.getByToken(record_fed_token_, _record_fed);
    _run.getByToken(error_fed_token_, _error_fed);
    _run.getByToken(cd_fed_dcci_tbi_token_, _cd_fed_dcci_tbi);
    _run.getByToken(invalid_fed_dcci_tbi_token_, _invalid_fed_dcci_tbi);
    _run.getByToken(eod_fed_dcci_tbi_token_, _eod_fed_dcci_tbi);
    _run.getByToken(rddm_fed_dcci_tbi_token_, _rddm_fed_dcci_tbi);
    _run.getByToken(rcdm_fed_dcci_tbi_token_, _rcdm_fed_dcci_tbi);

    if (first_run_) {
        record_fed_ = *_record_fed;
        error_fed_ = *_error_fed;
        cd_fed_dcci_tbi_ = *_cd_fed_dcci_tbi;
        invalid_fed_dcci_tbi_ = *_invalid_fed_dcci_tbi;
        eod_fed_dcci_tbi_ = *_eod_fed_dcci_tbi;
        rddm_fed_dcci_tbi_ = *_rddm_fed_dcci_tbi;
        rcdm_fed_dcci_tbi_ = *_rcdm_fed_dcci_tbi;


        first_run_ = false;
    } else {
        record_fed_.mergeProduct(*_record_fed);
        error_fed_.mergeProduct(*_error_fed);
        cd_fed_dcci_tbi_.mergeProduct(*_cd_fed_dcci_tbi);
        invalid_fed_dcci_tbi_.mergeProduct(*_invalid_fed_dcci_tbi);
        eod_fed_dcci_tbi_.mergeProduct(*_eod_fed_dcci_tbi);
        rddm_fed_dcci_tbi_.mergeProduct(*_rddm_fed_dcci_tbi);
        rcdm_fed_dcci_tbi_.mergeProduct(*_rcdm_fed_dcci_tbi);
    }
}

void RPCDCCRawToDigiPlotter::endJob()
{
    TH2D * _record_fed, * _error_fed;
    std::map<::uint32_t, TH2D *> _cd_fed_dcci_tbi, _invalid_fed_dcci_tbi, _eod_fed_dcci_tbi
        , _rddm_fed_dcci_tbi, _rcdm_fed_dcci_tbi;

    int _nbins(0);

    _record_fed = fileservice_->make<TH2D>(*record_fed_);
    _record_fed->UseCurrentStyle();
    _nbins = _record_fed->GetYaxis()->GetNbins();
    _record_fed->GetYaxis()->Set(_nbins, 0., 1.);
    _record_fed->GetYaxis()->SetNdivisions(-_nbins);

    _error_fed = fileservice_->make<TH2D>(*error_fed_);
    _error_fed->UseCurrentStyle();
    _nbins = _error_fed->GetYaxis()->GetNbins();
    _error_fed->GetYaxis()->Set(_nbins, 0., 1.);
    _error_fed->GetYaxis()->SetNdivisions(-_nbins);

    for (MergeMap<::uint32_t, MergeTH2D>::map_type::const_iterator _fed = cd_fed_dcci_tbi_->begin()
             ; _fed != cd_fed_dcci_tbi_->end() ; ++_fed) {
        TH2D * _th2d = fileservice_->make<TH2D>(*_fed->second);
        _th2d->UseCurrentStyle();
        _cd_fed_dcci_tbi[_fed->first] = _th2d;
    }

    for (MergeMap<::uint32_t, MergeTH2D>::map_type::const_iterator _fed = invalid_fed_dcci_tbi_->begin()
             ; _fed != invalid_fed_dcci_tbi_->end() ; ++_fed) {
        TH2D * _th2d = fileservice_->make<TH2D>(*_fed->second);
        _th2d->UseCurrentStyle();
        _invalid_fed_dcci_tbi[_fed->first] = _th2d;
    }

    for (MergeMap<::uint32_t, MergeTH2D>::map_type::const_iterator _fed = eod_fed_dcci_tbi_->begin()
             ; _fed != eod_fed_dcci_tbi_->end() ; ++_fed) {
        TH2D * _th2d = fileservice_->make<TH2D>(*_fed->second);
        _th2d->UseCurrentStyle();
        _eod_fed_dcci_tbi[_fed->first] = _th2d;
    }

    for (MergeMap<::uint32_t, MergeTH2D>::map_type::const_iterator _fed = rddm_fed_dcci_tbi_->begin()
             ; _fed != rddm_fed_dcci_tbi_->end() ; ++_fed) {
        TH2D * _th2d = fileservice_->make<TH2D>(*_fed->second);
        _th2d->UseCurrentStyle();
        _rddm_fed_dcci_tbi[_fed->first] = _th2d;
    }

    for (MergeMap<::uint32_t, MergeTH2D>::map_type::const_iterator _fed = rcdm_fed_dcci_tbi_->begin()
             ; _fed != rcdm_fed_dcci_tbi_->end() ; ++_fed) {
        TH2D * _th2d = fileservice_->make<TH2D>(*_fed->second);
        _th2d->UseCurrentStyle();
        _rcdm_fed_dcci_tbi[_fed->first] = _th2d;
    }

    if (!create_pdf_)
        return;

    // PDF Output
    TCanvas _dummy_canvas("DummyCanvas");
    _dummy_canvas.ResetBit(kMustCleanup);
    _dummy_canvas.ResetBit(kCanDelete);
    {
        std::string _output_pdf = output_pdf_ + "[";
        _dummy_canvas.Print(_output_pdf.c_str());
    }

    TText _title(0.10, 0.91, "Title");
    _title.ResetBit(kMustCleanup);
    _title.ResetBit(kCanDelete);
    _title.SetNDC();
    _title.SetTextSize(0.04);
    _title.SetTextAlign(11);
    TText _title_note(0.85, 0.91, "CMS Preliminary");
    _title_note.ResetBit(kMustCleanup);
    _title_note.ResetBit(kCanDelete);
    _title_note.SetNDC();
    _title_note.SetTextSize(0.02);
    _title_note.SetTextAlign(31);

    DivergingPalette _diverging_palette;
    _diverging_palette.applyPalette();

    {
        TCanvas _canvas(_record_fed->GetName());
        _canvas.ResetBit(kMustCleanup);
        _canvas.ResetBit(kCanDelete);
        _canvas.SetTicks(1, 1);
        _canvas.SetRightMargin(.15);
        _canvas.SetBottomMargin(.15);
        TH1 * _temp_th2d = _record_fed->DrawCopy("colz");
        _temp_th2d->SetTitle("");
        _temp_th2d->SetStats(false);
        _temp_th2d->GetXaxis()->SetTitle("");
        _temp_th2d->GetZaxis()->SetTitle("Records");
        _title.SetText(0.10, 0.91, _record_fed->GetTitle());
        _title.Draw();
        _title_note.Draw();
        _canvas.Print(output_pdf_.c_str());
    }

    {
        TCanvas _canvas(_error_fed->GetName());
        _canvas.ResetBit(kMustCleanup);
        _canvas.ResetBit(kCanDelete);
        _canvas.SetTicks(1, 1);
        _canvas.SetRightMargin(.15);
        _canvas.SetBottomMargin(.15);
        TH1 * _temp_th2d = _error_fed->DrawCopy("colz");
        _temp_th2d->SetTitle("");
        _temp_th2d->SetStats(false);
        _temp_th2d->GetXaxis()->SetTitle("");
        _temp_th2d->GetZaxis()->SetTitle("Errors");
        _title.SetText(0.10, 0.91, _error_fed->GetTitle());
        _title.Draw();
        _title_note.Draw();
        _canvas.Print(output_pdf_.c_str());
    }

    for (std::map<::uint32_t, TH2D *>::const_iterator _fed = _cd_fed_dcci_tbi.begin()
             ; _fed != _cd_fed_dcci_tbi.end() ; ++_fed) {
        TCanvas _canvas(_fed->second->GetName());
        _canvas.ResetBit(kMustCleanup);
        _canvas.ResetBit(kCanDelete);
        _canvas.SetTicks(1, 1);
        _canvas.SetRightMargin(.15);
        TH1 * _temp_th2d = _fed->second->DrawCopy("colz");
        _temp_th2d->SetTitle("");
        _temp_th2d->SetStats(false);
        _temp_th2d->GetZaxis()->SetTitle("Records");
        _title.SetText(0.10, 0.91, _fed->second->GetTitle());
        _title.Draw();
        _title_note.Draw();
        _canvas.Print(output_pdf_.c_str());
    }

    for (std::map<::uint32_t, TH2D *>::const_iterator _fed = _invalid_fed_dcci_tbi.begin()
             ; _fed != _invalid_fed_dcci_tbi.end() ; ++_fed) {
        TCanvas _canvas(_fed->second->GetName());
        _canvas.ResetBit(kMustCleanup);
        _canvas.ResetBit(kCanDelete);
        _canvas.SetTicks(1, 1);
        _canvas.SetRightMargin(.15);
        TH1 * _temp_th2d = _fed->second->DrawCopy("colz");
        _temp_th2d->SetTitle("");
        _temp_th2d->SetStats(false);
        _temp_th2d->GetZaxis()->SetTitle("Records");
        _title.SetText(0.10, 0.91, _fed->second->GetTitle());
        _title.Draw();
        _title_note.Draw();
        _canvas.Print(output_pdf_.c_str());
    }

    for (std::map<::uint32_t, TH2D *>::const_iterator _fed = _eod_fed_dcci_tbi.begin()
             ; _fed != _eod_fed_dcci_tbi.end() ; ++_fed) {
        TCanvas _canvas(_fed->second->GetName());
        _canvas.ResetBit(kMustCleanup);
        _canvas.ResetBit(kCanDelete);
        _canvas.SetTicks(1, 1);
        _canvas.SetRightMargin(.15);
        TH1 * _temp_th2d = _fed->second->DrawCopy("colz");
        _temp_th2d->SetTitle("");
        _temp_th2d->SetStats(false);
        _temp_th2d->GetZaxis()->SetTitle("Records");
        _title.SetText(0.10, 0.91, _fed->second->GetTitle());
        _title.Draw();
        _title_note.Draw();
        _canvas.Print(output_pdf_.c_str());
    }

    for (std::map<::uint32_t, TH2D *>::const_iterator _fed = _rddm_fed_dcci_tbi.begin()
             ; _fed != _rddm_fed_dcci_tbi.end() ; ++_fed) {
        TCanvas _canvas(_fed->second->GetName());
        _canvas.ResetBit(kMustCleanup);
        _canvas.ResetBit(kCanDelete);
        _canvas.SetTicks(1, 1);
        _canvas.SetRightMargin(.15);
        TH1 * _temp_th2d = _fed->second->DrawCopy("colz");
        _temp_th2d->SetTitle("");
        _temp_th2d->SetStats(false);
        _temp_th2d->GetZaxis()->SetTitle("Records");
        _title.SetText(0.10, 0.91, _fed->second->GetTitle());
        _title.Draw();
        _title_note.Draw();
        _canvas.Print(output_pdf_.c_str());
    }

    for (std::map<::uint32_t, TH2D *>::const_iterator _fed = _rcdm_fed_dcci_tbi.begin()
             ; _fed != _rcdm_fed_dcci_tbi.end() ; ++_fed) {
        TCanvas _canvas(_fed->second->GetName());
        _canvas.ResetBit(kMustCleanup);
        _canvas.ResetBit(kCanDelete);
        _canvas.SetTicks(1, 1);
        _canvas.SetRightMargin(.15);
        TH1 * _temp_th2d = _fed->second->DrawCopy("colz");
        _temp_th2d->SetTitle("");
        _temp_th2d->SetStats(false);
        _temp_th2d->GetZaxis()->SetTitle("Records");
        _title.SetText(0.10, 0.91, _fed->second->GetTitle());
        _title.Draw();
        _title_note.Draw();
        _canvas.Print(output_pdf_.c_str());
    }

    {
        std::string _output_pdf = output_pdf_ + "]";
        _dummy_canvas.Print(_output_pdf.c_str());
    }
}

//define this as a module
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(RPCDCCRawToDigiPlotter);
