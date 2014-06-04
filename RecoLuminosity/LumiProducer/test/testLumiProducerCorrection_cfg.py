# usage: cmsRun test/testLumiProducerCorrection_cfg.py run=205908 firstLumi=190 lastLumi=200 lumisToProcess=
#        cmsRun test/testLumiProducerCorrection_cfg.py run=205908

import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
from FWCore.PythonUtilities.LumiList import LumiList

options = VarParsing()
options.register("run"
                 , 208509
                 , VarParsing.multiplicity.singleton
                 , VarParsing.varType.int
                 , "Run to process")
options.register("firstLumi"
                 , 1
                 , VarParsing.multiplicity.singleton
                 , VarParsing.varType.int
                 , "First LuminosityBlock in the Run")
options.register("lastLumi"
                 , 5000
                 , VarParsing.multiplicity.singleton
                 , VarParsing.varType.int
                 , "Last LuminosityBlock in the Run")
options.register("minBiasXsec"
                 , 69400. # https://twiki.cern.ch/twiki/bin/view/CMS/PileupJSONFileforData
                 , VarParsing.multiplicity.singleton
                 , VarParsing.varType.float
                 , "Minimum-Bias Cross Section")
options.register("lumisToProcess"
                 , "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt"
                 , VarParsing.multiplicity.singleton
                 , VarParsing.varType.string
                 , "LuminosityBlocks to process")
options.parseArguments()

process = cms.Process("testLumiProducerCorrection")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("RecoLuminosity.LumiProducer.lumiProducer_cff")
process.LumiCorrectionSource = cms.ESSource("LumiCorrectionSource"
                                            , connect=cms.string("frontier://LumiCalc/CMS_LUMI_PROD")
)

process.source = cms.Source("EmptySource"
                            , firstRun = cms.untracked.uint32(options.run)
                            , numberEventsInRun = cms.untracked.uint32(options.lastLumi - options.firstLumi + 1)
                            , firstLuminosityBlock = cms.untracked.uint32(options.firstLumi)
                            , numberEventsInLuminosityBlock = cms.untracked.uint32(1)
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.lastLumi - options.firstLumi + 1) )
process.MessageLogger.cerr.FwkReport.limit = cms.untracked.int32(0)

process.testLumiProducerCorrection = cms.EDAnalyzer("TestLumiProducerCorrection"
                                                    , minBiasXsec = cms.untracked.double(options.minBiasXsec)
                                                    , lumisToProcess = LumiList(filename = options.lumisToProcess).getVLuminosityBlockRange()
                                                    , run = cms.untracked.uint32(options.run)
                                                    , firstLumi = cms.untracked.uint32(options.firstLumi)
                                                    , lastLumi = cms.untracked.uint32(options.lastLumi)
)

process.p = cms.Path(process.lumiProducer * process.testLumiProducerCorrection)
