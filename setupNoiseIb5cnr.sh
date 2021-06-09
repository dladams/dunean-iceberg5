# setupNoiseIb5cnr.sh
#
# David Adams
# April 2021
#
# Configuration file for the drawNoise command in dunenoise.
#
# Makes plots of cal and cal+cnr

SPER=$(ibRunPeriod $1)
SDET=iceberg$SPER
XDIR=setIceberg$SPER/addRoiTreeNoise
PLOTS="cal-nsgrms:400,cal-nsgrms50:300,cnr-nsgrms:400,cnr-nsgrms50:300,cal-utcran:noplot"
CRVS="zcGood-uvGood"
DIRPAT="ib5%REC%NoiseVsChan/$XDIR/iceberg%RUN%/adcrois.root"
EVTPAT="ib5%REC%NoiseVsChan/$XDIR/iceberg%RUN%/event%EVT%/chmet_%VAR%_all_run%RUN%_%REC%.tpad"
GRPPAT="ib5%REC%NoiseVsChan/$XDIR/ibgroup%RUN%/chmet_%VAR%_all_runRunNotFound_%REC%.tpad"
SUMNAM=("ib5cnrNoiseVsChan/$XDIR/iceberg%RUN%/noisesum_run%RUN%"
        "ib5cnrNoiseVsChan/$XDIR/iceberg%RUN%_proc000500_skip000000/noisesum_run%RUN%")
