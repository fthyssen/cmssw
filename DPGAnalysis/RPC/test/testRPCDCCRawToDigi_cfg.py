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

process = cms.Process("testRPCDCCRawToDigi")

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
process.GlobalTag.globaltag = "80X_dataRun2_Express_v6"

process.load("EventFilter.RPCRawToDigi.rpcUnpackingModule_cfi")
process.load("DPGAnalysis.RPC.RPCLinkMap_sqlite_cff")
process.load("DPGAnalysis.RPC.RPCDCCRawToDigi_cfi")

process.load('Geometry.RPCGeometryBuilder.rpcGeometryDB_cfi')

process.load("DPGAnalysis.RPC.RPCRawToDigiAnalyser_cfi")
process.RPCRawToDigiAnalyser.LHSDigiCollection = cms.InputTag('rpcUnpackingModule')
process.RPCRawToDigiAnalyser.RHSDigiCollection = cms.InputTag('RPCDCCRawToDigi')

process.load("DPGAnalysis.Tools.EventFilter_cfi")

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

# Source
process.source = cms.Source("PoolSource"
                            , fileNames = cms.untracked.vstring(options.inputFiles)
                            # , fileNames = cms.untracked.vstring("/store/express/Commissioning2016/ExpressCosmics/FEVT/Express-v1/000/266/668/00000/FEFEB1B4-D5E8-E511-BF25-02163E013642.root")
                            # , lumisToProcess = lumilist.getVLuminosityBlockRange()
)

#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000) )
#process.maxLuminosityBlocks = cms.untracked.PSet( input = cms.untracked.int32(20) )

process.RPCDCCRawToDigiSecond = process.RPCDCCRawToDigi.clone();
process.rpcUnpackingModuleSecond = process.rpcUnpackingModule.clone();

process.p = cms.Path((process.RPCDCCRawToDigi + process.rpcUnpackingModule)
                     * (process.RPCDCCRawToDigiSecond + process.rpcUnpackingModuleSecond)
                     * process.RPCRawToDigiAnalyser
                     * process.EventFilter
)

# Output
process.out = cms.OutputModule("PoolOutputModule"
                               , outputCommands = cms.untracked.vstring("drop *"
                                                                        , "keep *_*_*_testRPCDCCRawToDigi")
                               #, fileName = cms.untracked.string(options.outputFile)
                               , fileName = cms.untracked.string("testRPCDCCRawToDigi.root")
                               , SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring("p"))
)

process.e = cms.EndPath(process.out)
