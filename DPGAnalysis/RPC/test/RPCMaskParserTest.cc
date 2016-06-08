#include "DPGAnalysis/RPC/test/RPCMaskParserTest.h"

#include <vector>

#include <fstream>
#include <iostream>

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/RPCGeometry/interface/RPCChamber.h"
#include "Geometry/RPCGeometry/interface/RPCGeomServ.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DPGAnalysis/RPC/interface/RPCMaskDetId.h"
#include "DPGAnalysis/RPC/interface/RPCMaskParser.h"

RPCMaskParserTest::RPCMaskParserTest(edm::ParameterSet const &)
{}

void RPCMaskParserTest::analyze(edm::Event const & _event, edm::EventSetup const & _setup)
{
    edm::ESHandle<RPCGeometry> _rpc_geom;
    _setup.get<MuonGeometryRecord>().get(_rpc_geom);
    RPCDetId _rpcdetid;

    // Some common cases
    {
        RPCMaskDetId _rpccompdetid;
        _rpccompdetid.setRegion(0);
        _rpccompdetid.setRing(0);
        RPCMaskDetId _rpccompdetid_parsed(RPCMaskParser::parse("W0"));
        std::cerr << "W0:"
                  << " " << _rpccompdetid << " " << _rpccompdetid_parsed
                  << " " << (_rpccompdetid == _rpccompdetid_parsed ? 1 : 0) << std::endl;

        _rpccompdetid.setRing(1);
        _rpccompdetid_parsed = RPCMaskParser::parse("W1");
        std::cerr << "W1:"
                  << " " << _rpccompdetid << " " << _rpccompdetid_parsed
                  << " " << (_rpccompdetid == _rpccompdetid_parsed ? 1 : 0) << std::endl;

        _rpccompdetid_parsed = RPCMaskParser::parse("W+1");
        std::cerr << "W+1:"
                  << " " << _rpccompdetid << " " << _rpccompdetid_parsed
                  << " " << (_rpccompdetid == _rpccompdetid_parsed ? 1 : 0) << std::endl;

        _rpccompdetid.reset();

        _rpccompdetid.setRegion(-1);
        _rpccompdetid.setStation(2);
        _rpccompdetid.setLayer(1);
        _rpccompdetid_parsed = RPCMaskParser::parse("RE-2");
        std::cerr << "RE-2:"
                  << " " << _rpccompdetid << " " << _rpccompdetid_parsed
                  << " " << (_rpccompdetid == _rpccompdetid_parsed ? 1 : 0) << std::endl;
    }

    {
        std::vector<RPCChamber const *> const & _chambers = (_rpc_geom->chambers());
        std::vector<RPCChamber const *>::const_iterator _chamber_it_end = _chambers.end();
        std::ofstream _chamber_file("chambers.txt");
        for (std::vector<RPCChamber const *>::const_iterator _chamber_it = _chambers.begin()
                 ; _chamber_it != _chamber_it_end; ++_chamber_it)
            {
                _rpcdetid = (*_chamber_it)->id();
                RPCMaskDetId _rpccompdetid(_rpcdetid);
                RPCMaskDetId _rpccompdetid_parsed(RPCMaskParser::parse(_rpccompdetid.getName()));
                if (_rpcdetid != RPCMaskDetId(_rpcdetid).getRPCDetId()
                    || _rpccompdetid != _rpccompdetid_parsed)
                    std::cerr << RPCGeomServ(_rpcdetid).name()
                              << " " << _rpccompdetid << " " << _rpccompdetid_parsed
                              << " " << _rpccompdetid.getId() << " " << _rpccompdetid_parsed.getId() << std::endl;
                _chamber_file << std::setw(12) << _rpccompdetid.getId() << " "
                              << std::setw(20) << RPCGeomServ(_rpcdetid).name() << " "
                              << std::setw(20) << _rpccompdetid << " "
                              << std::endl;
            }
        _chamber_file.close();
    }

    {
        std::vector<RPCRoll const *> const & _rolls = (_rpc_geom->rolls());
        std::vector<RPCRoll const *>::const_iterator _roll_it_end = _rolls.end();

        std::ofstream _roll_file("rolls.txt");
        for (std::vector<RPCRoll const *>::const_iterator _roll_it = _rolls.begin()
                 ; _roll_it != _roll_it_end ; ++_roll_it)
            {
                _rpcdetid = (*_roll_it)->id();
                RPCMaskDetId _rpccompdetid(_rpcdetid);
                RPCMaskDetId _wod(_rpccompdetid.isBarrel() ? _rpccompdetid.getWheelId() : _rpccompdetid.getDiskId());
                RPCMaskDetId _rpccompdetid_parsed(RPCMaskParser::parse(_rpccompdetid.getName()));
                if (_rpcdetid != RPCMaskDetId(_rpcdetid).getRPCDetId()
                    || _rpccompdetid != _rpccompdetid_parsed)
                    std::cerr << RPCGeomServ(_rpcdetid).name()
                              << " " << _rpccompdetid << " " << _rpccompdetid_parsed
                              << " " << _rpccompdetid.getId() << " " << _rpccompdetid_parsed.getId() << std::endl;
                _roll_file << std::setw(12) << _rpccompdetid.getId() << " "
                           << std::setw(30) << RPCGeomServ(_rpcdetid).name() << " "
                           << std::setw(20) << _rpccompdetid << " "
                           << std::setw(20) << _rpccompdetid.getChamberId() << " "
                           << std::setw(5) << _wod
                           << std::endl;
            }
        _roll_file.close();
    }

}

//define this as a plug-in
DEFINE_FWK_MODULE(RPCMaskParserTest);
