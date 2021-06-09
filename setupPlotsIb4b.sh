# setupPlotsIb4b.sh
#
# David Adams
# July 2020
#
# Example configuration file for the drawNoise command in dunenoise.

SDET=iceberg4b
PLOTS="tai-nsgrms:400,tai-nsgrms50:300,cnr-nsgrms:400,cnr-nsgrms50:300,tai-utcran:noplot"
PLOTS="tai-nsgrms:400,tai-nsgrms50:300,tai-utcran:noplot"
CRVS="zcGood-uvGood"
DIRPAT="ib4%REC%NoiseVsChan/setIceberg4b/addRoiTreeNoise/iceberg%RUN%/chmet_%VAR%_all_run%RUN%_%REC%.tpad"
DIRPAT="ib4%REC%NoiseVsChan/setIceberg4b/addRoiTreeNoise/iceberg%RUN%/adcrois.root"
EVTPAT="ib4%REC%NoiseVsChan/setIceberg4b/addRoiTreeNoise/iceberg%RUN%/event%EVT%/chmet_%VAR%_all_run%RUN%_%REC%.tpad"
GRPPAT="ib4%REC%NoiseVsChan/setIceberg4b/addRoiTreeNoise/ibgroup%RUN%/chmet_%VAR%_all_runRunNotFound_%REC%.tpad"
#SUMNAM="plots-noisesum/noise_%ALLSPEC%_%CURVES%_run%RUN%"
SUMNAM="plots-noisesum/noisesum_run%RUN%"
SUMNAM="ib4taiNoiseVsChan/setIceberg4b/addRoiTreeNoise/iceberg%RUN%/noisesum_run%RUN%"
export ROOTFCL="load-ib4.fcl"
