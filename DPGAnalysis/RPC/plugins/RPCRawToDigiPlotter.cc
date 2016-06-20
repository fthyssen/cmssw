#include "DPGAnalysis/RPC/plugins/RPCRawToDigiPlotter.h"

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

#include "DPGAnalysis/RPC/interface/RPCMaskDetId.h"

#include "DPGAnalysis/Tools/interface/DivergingPalette.h"
#include "DPGAnalysis/Tools/interface/SequentialPalette.h"

RPCRawToDigiPlotter::RPCRawToDigiPlotter(edm::ParameterSet const & _config)
    : first_run_(true)
    , output_pdf_(_config.getParameter<std::string>("PDFOutput") + ".pdf")
    , create_pdf_(_config.getParameter<bool>("CreatePDF"))
{
    std::string _analyser(_config.getParameter<std::string>("RPCRawToDigiAnalyser"));
    region_station_ring_roll_roll_token_
        = consumes<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>(edm::InputTag(_analyser, "RSRRollRoll"));
    region_station_ring_strip_strip_token_
        = consumes<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>(edm::InputTag(_analyser, "RSRStripStrip"));
    region_station_ring_bx_bx_token_
        = consumes<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>(edm::InputTag(_analyser, "RSRBXBX"));
}

void RPCRawToDigiPlotter::fillDescriptions(edm::ConfigurationDescriptions & _descs)
{
    edm::ParameterSetDescription _desc;

    _desc.add<std::string>("RPCRawToDigiAnalyser", "RPCRawToDigiAnalyser");

    _desc.add<std::string>("PDFOutput", "RPCRawToDigiAnalyser");
    _desc.add<bool>("CreatePDF", false);

    _descs.add("RPCRawToDigiPlotter", _desc);
}

void RPCRawToDigiPlotter::beginJob()
{}

void RPCRawToDigiPlotter::beginRun(edm::Run const &, edm::EventSetup const &)
{}

void RPCRawToDigiPlotter::analyze(edm::Event const &, edm::EventSetup const &)
{}

void RPCRawToDigiPlotter::endRun(edm::Run const & _run, edm::EventSetup const &)
{
    edm::Handle<MergeMap<::uint32_t, MergeTH2D> > _region_station_ring_roll_roll
        , _region_station_ring_strip_strip
        , _region_station_ring_bx_bx;

    _run.getByToken(region_station_ring_roll_roll_token_, _region_station_ring_roll_roll);
    _run.getByToken(region_station_ring_strip_strip_token_, _region_station_ring_strip_strip);
    _run.getByToken(region_station_ring_bx_bx_token_, _region_station_ring_bx_bx);

    if (first_run_) {
        region_station_ring_roll_roll_ = *_region_station_ring_roll_roll;
        region_station_ring_strip_strip_ = *_region_station_ring_strip_strip;
        region_station_ring_bx_bx_ = *_region_station_ring_bx_bx;

        first_run_ = false;
    } else {
        region_station_ring_roll_roll_.mergeProduct(*_region_station_ring_roll_roll);
        region_station_ring_strip_strip_.mergeProduct(*_region_station_ring_strip_strip);
        region_station_ring_bx_bx_.mergeProduct(*_region_station_ring_bx_bx);
    }
}

void RPCRawToDigiPlotter::endJob()
{
    std::map<::uint32_t, TH2D *> _region_station_ring_roll_roll
        , _region_station_ring_strip_strip
        , _region_station_ring_bx_bx;

    TFileDirectory _roll_folder(fileservice_->mkdir("Roll"));
    for (MergeMap<::uint32_t, MergeTH2D>::map_type::const_iterator _region_station_ring = region_station_ring_roll_roll_->begin()
             ; _region_station_ring != region_station_ring_roll_roll_->end() ; ++_region_station_ring) {
        TH2D * _th2d = _roll_folder.make<TH2D>(*_region_station_ring->second);
        _th2d->UseCurrentStyle();
        int _nbins = _th2d->GetXaxis()->GetNbins();
        _th2d->GetXaxis()->Set(_nbins, 0., 1.);
        _th2d->GetXaxis()->SetNdivisions(-_nbins);
        _th2d->GetYaxis()->Set(_nbins, 0., 1.);
        _th2d->GetYaxis()->SetNdivisions(-_nbins);
        _region_station_ring_roll_roll[_region_station_ring->first] = _th2d;
    }

    TFileDirectory _strip_folder(fileservice_->mkdir("Strip"));
    for (MergeMap<::uint32_t, MergeTH2D>::map_type::const_iterator _region_station_ring = region_station_ring_strip_strip_->begin()
             ; _region_station_ring != region_station_ring_strip_strip_->end() ; ++_region_station_ring) {
        TH2D * _th2d = _strip_folder.make<TH2D>(*_region_station_ring->second);
        _region_station_ring_strip_strip[_region_station_ring->first] = _th2d;
        _th2d->UseCurrentStyle();
    }

    TFileDirectory _bx_folder(fileservice_->mkdir("Bx"));
    for (MergeMap<::uint32_t, MergeTH2D>::map_type::const_iterator _region_station_ring = region_station_ring_bx_bx_->begin()
             ; _region_station_ring != region_station_ring_bx_bx_->end() ; ++_region_station_ring) {
        TH2D * _th2d = _bx_folder.make<TH2D>(*_region_station_ring->second);
        _region_station_ring_bx_bx[_region_station_ring->first] = _th2d;
        _th2d->UseCurrentStyle();
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

    for (std::map<::uint32_t, TH2D *>::const_iterator _region_station_ring = _region_station_ring_roll_roll.begin()
             ; _region_station_ring != _region_station_ring_roll_roll.end() ; ++_region_station_ring) {
        TCanvas _canvas(_region_station_ring->second->GetName());
        _canvas.ResetBit(kMustCleanup);
        _canvas.ResetBit(kCanDelete);
        _canvas.SetTicks(1, 1);
        _canvas.SetLeftMargin(.20);
        _canvas.SetRightMargin(.15);
        _canvas.SetBottomMargin(.15);
        TH1 * _temp_th2d = _region_station_ring->second->DrawCopy("colz");
        _temp_th2d->SetTitle("");
        _temp_th2d->SetStats(false);
        _temp_th2d->GetXaxis()->SetTitle("");
        _temp_th2d->GetYaxis()->SetTitle("");
        _temp_th2d->GetZaxis()->SetTitle("Hits");
        _title.SetText(0.10, 0.91, _region_station_ring->second->GetTitle());
        _title.Draw();
        RPCMaskDetId _mask_id(_region_station_ring->first);
        std::ostringstream _mask_oss;
        _mask_oss << (_mask_id.getRPCDetId().rawId());
        _title_note.SetText(0.85, 0.91, _mask_oss.str().c_str());
        _title_note.Draw();
        _canvas.Print(output_pdf_.c_str());
    }

    for (std::map<::uint32_t, TH2D *>::const_iterator _region_station_ring = _region_station_ring_strip_strip.begin()
             ; _region_station_ring != _region_station_ring_strip_strip.end() ; ++_region_station_ring) {
        TCanvas _canvas(_region_station_ring->second->GetName());
        _canvas.ResetBit(kMustCleanup);
        _canvas.ResetBit(kCanDelete);
        _canvas.SetTicks(1, 1);
        _canvas.SetRightMargin(.15);
        TH1 * _temp_th2d = _region_station_ring->second->DrawCopy("colz");
        _temp_th2d->SetTitle("");
        _temp_th2d->SetStats(false);
        _temp_th2d->GetZaxis()->SetTitle("Hits");
        _title.SetText(0.10, 0.91, _region_station_ring->second->GetTitle());
        _title.Draw();
        _title_note.Draw();
        _canvas.Print(output_pdf_.c_str());
    }

    for (std::map<::uint32_t, TH2D *>::const_iterator _region_station_ring = _region_station_ring_bx_bx.begin()
             ; _region_station_ring != _region_station_ring_bx_bx.end() ; ++_region_station_ring) {
        TCanvas _canvas(_region_station_ring->second->GetName());
        _canvas.ResetBit(kMustCleanup);
        _canvas.ResetBit(kCanDelete);
        _canvas.SetTicks(1, 1);
        _canvas.SetRightMargin(.15);
        TH1 * _temp_th2d = _region_station_ring->second->DrawCopy("colz");
        _temp_th2d->SetTitle("");
        _temp_th2d->SetStats(false);
        _temp_th2d->GetZaxis()->SetTitle("Hits");
        _title.SetText(0.10, 0.91, _region_station_ring->second->GetTitle());
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
DEFINE_FWK_MODULE(RPCRawToDigiPlotter);
