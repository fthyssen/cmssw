import FWCore.ParameterSet.Config as cms

from FWCore.PythonUtilities.LumiList import LumiList
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing("analysis")
options.register("runList"
                 , []
                 , VarParsing.multiplicity.list
                 , VarParsing.varType.int
                 , "Run selection")
options.register("lumiList"
                 , "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt"
                 , VarParsing.multiplicity.singleton
                 , VarParsing.varType.string
                 , "JSON file")
options.parseArguments()

lumilist = LumiList(filename = options.lumiList)
if len(options.runList) :
    runlist = LumiList(runs = options.runList)
    lumilist = lumilist & runlist
    if not len(lumilist) :
        raise RuntimeError, "The resulting LumiList is empty"

process = cms.Process("testRPCTwinMuxRawToDigi")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
# process.MessageLogger = cms.Service(
#     "MessageLogger"
#     , destinations = cms.untracked.vstring(
#         'detailedInfo',
#         'critical'
#     )
#     , detailedInfo = cms.untracked.PSet(
#         threshold = cms.untracked.string('DEBUG')
#     )
#     , debugModules = cms.untracked.vstring(
#         '*'
#     )
# )

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "80X_dataRun2_Express_v10"

process.load("EventFilter.RPCRawToDigi.rpcUnpackingModule_cfi")
process.load("CondTools.RPC.RPCLinkMap_sqlite_cff")
process.load("EventFilter.L1TXRawToDigi.twinMuxStage2Digis_cfi")
process.load("EventFilter.RPCRawToDigi.RPCTwinMuxRawToDigi_cfi")
process.RPCTwinMuxRawToDigi.bxMin = cms.int32(-5)
process.RPCTwinMuxRawToDigi.bxMax = cms.int32(5)

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

# Source
process.source = cms.Source("PoolSource"
                            , fileNames = cms.untracked.vstring(options.inputFiles)
                            , lumisToProcess = lumilist.getVLuminosityBlockRange()
)

# process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000) )
# process.maxLuminosityBlocks = cms.untracked.PSet( input = cms.untracked.int32(20) )

process.p = cms.Path(process.RPCTwinMuxRawToDigi
                     + process.twinMuxStage2Digis
                     + process.rpcUnpackingModule
)

# Output
process.out = cms.OutputModule("PoolOutputModule"
                               , outputCommands = cms.untracked.vstring("drop *"
                                                                        , "keep *_*_*_testRPCTwinMuxRawToDigi")
                               , fileName = cms.untracked.string(options.outputFile)
                               , SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring("p"))
)

process.e = cms.EndPath(process.out)
