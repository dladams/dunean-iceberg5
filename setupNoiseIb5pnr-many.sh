# setupNoiseIb5pnr-many.sh
#
# David Adams
# April 2021
#
# Configuration file for the drawNoise command in dunenoise.
#
# Makes plots of ped and ped+pnr

SPER=$(ibRunPeriod $1)
SDET=iceberg$SPER
XDIR=setIceberg$SPER
PLOTS="ped-nsgrms:400,ped-nsgrms50:300,pnr-nsgrms:400,pnr-nsgrms50:300,ped-utcran:noplot"
CRVS="zcGood-uvGood"
DIRPAT="ib5%REC%NoiseTree/$XDIR/iceberg%RUN%/adcrois.root"
EVTPAT="ib5%REC%NoiseTree/$XDIR/iceberg%RUN%/event%EVT%/chmet_%VAR%_all_run%RUN%_%REC%.tpad"
GRPPAT="ib5%REC%NoiseTree/$XDIR/ibgroup%RUN%/chmet_%VAR%_all_runRunNotFound_%REC%.tpad"
SUMNAM=("ib5pnrNoiseTree/$XDIR/iceberg%RUN%/noisesum_run%RUN%"
        "ib5pnrNoiseTree/$XDIR/iceberg%RUN%_proc000500_skip000000/noisesum_run%RUN%")
