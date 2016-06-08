import FWCore.ParameterSet.Config as cms

process = cms.Process("RPCMaskParserTest")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = "80X_dataRun2_Express_v6"

process.load('Geometry.RPCGeometryBuilder.rpcGeometryDB_cfi')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.source = cms.Source("EmptyIOVSource"
                            , timetype = cms.string('runnumber')
                            , lastValue = cms.uint64(500001)
                            , firstValue = cms.uint64(500000)
                            , interval = cms.uint64(1))

process.RPCMaskParserTest = cms.EDAnalyzer("RPCMaskParserTest")

process.p = cms.Path(process.RPCMaskParserTest)
