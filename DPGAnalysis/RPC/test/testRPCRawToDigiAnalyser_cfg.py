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
                 , "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/DCSOnly/json_DCSONLY.txt"
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

process = cms.Process("testRPCRawToDigiAnalyser")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "80X_dataRun2_Express_v6"

process.load('Geometry.RPCGeometryBuilder.rpcGeometryDB_cfi')

process.load("EventFilter.RPCRawToDigi.rpcUnpackingModule_cfi")
process.load("EventFilter.RPCRawToDigi.RPCTwinMuxRawToDigi_cfi")
process.load("DPGAnalysis.RPC.RPCLinkMap_sqlite_cff")
process.load("DPGAnalysis.RPC.RPCDCCRawToDigi_cfi")
process.rpcUnpackingModuleSecond = process.rpcUnpackingModule.clone()
process.RPCDCCRawToDigiSecond = process.RPCDCCRawToDigi.clone()
process.RPCTwinMuxRawToDigi.calculateCRC = cms.bool(True)

process.load("EventFilter.L1TXRawToDigi.twinMuxStage2Digis_cfi")

process.load("DPGAnalysis.RPC.RPCRawToDigiAnalyser_cfi")
process.RPCRawToDigiAnalyser.LHSDigiCollection = cms.InputTag('rpcUnpackingModule')
# process.RPCRawToDigiAnalyser.RHSDigiCollection = cms.InputTag('RPCDCCRawToDigi')
process.RPCRawToDigiAnalyser.RHSDigiCollection = cms.InputTag('RPCTwinMuxRawToDigi')
process.RPCRawToDigiAnalyser.RollSelection = cms.vstring('RB') # 'RB/W-1/S9', 'RB/S9', ...

process.load("DPGAnalysis.Tools.EventFilter_cfi")

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

# Source
process.source = cms.Source("PoolSource"
                            , fileNames = cms.untracked.vstring(options.inputFiles)
                            # , lumisToProcess = lumilist.getVLuminosityBlockRange()
                            # , inputCommands =  cms.untracked.vstring("keep *"
                            # , "drop GlobalObjectMapRecord_*_*_*")
)

#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )
process.maxLuminosityBlocks = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.p = cms.Path((process.rpcUnpackingModule
                      # + process.RPCDCCRawToDigi
                      + process.RPCTwinMuxRawToDigi)
                     # + process.twinMuxStage2Digis)
                     * process.RPCRawToDigiAnalyser
                     * process.EventFilter
)

# Output
process.out = cms.OutputModule("PoolOutputModule"
                               , outputCommands = cms.untracked.vstring("drop *"
                                                                        , "keep *_*_*_testRPCRawToDigiAnalyser")
                               #, fileName = cms.untracked.string(options.outputFile)
                               , fileName = cms.untracked.string("testRPCRawToDigiAnalyser.root")
                               , SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring("p"))
)

process.e = cms.EndPath(process.out)
