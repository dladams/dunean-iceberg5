# setupPlotsIb5.sh
#
# David Adams
# March 2021
#
# Configuration file for the drawNoise command in dunenoise.

#echo Hi, run is $RUN period is $SPER
SDET=iceberg$SPER
XDIR=/addRoiTreeNoise
PLOTS="cal-nsgrms:400,cal-nsgrms50:300,cal-utcran:noplot"
CRVS="zcGood-uvGood"
DIRPAT="ib5%REC%NoiseVsChan/setIceberg$SPER/$XDIR/iceberg%RUN%/chmet_%VAR%_all_run%RUN%_%REC%.tpad"
DIRPAT="ib5%REC%NoiseVsChan/setIceberg$SPER/$XDIR/iceberg%RUN%/adcrois.root"
EVTPAT="ib5%REC%NoiseVsChan/setIceberg$SPER/$XDIR/iceberg%RUN%/event%EVT%/chmet_%VAR%_all_run%RUN%_%REC%.tpad"
GRPPAT="ib5%REC%NoiseVsChan/setIceberg$SPER/$XDIR/ibgroup%RUN%/chmet_%VAR%_all_runRunNotFound_%REC%.tpad"
SUMNAM="ib5calNoiseVsChan/setIceberg$SPER/$XDIR/iceberg%RUN%/noisesum_run%RUN%"
SUMNAM=("ib5calNoiseVsChan/setIceberg$SPER/$XDIR/iceberg%RUN%/noisesum_run%RUN%"
        "ib5calNoiseVsChan/setIceberg$SPER/$XDIR/iceberg%RUN%_proc000500_skip000000/noisesum_run%RUN%")
