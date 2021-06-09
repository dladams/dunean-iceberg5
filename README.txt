Iceberg 5 analysis

Run list is run5.dat.

To see many useful commands in duneproc, use
> dunprocHelp ib
> dunprocHelp ibex

Some local configuration and scripts:

To build and use the ROI tree At ADC level, e.g for calibration.
> duneproc roiTree iceberg008388 20
> view roiTree/iceberg008388_proc000020/adcrois.root
root> adcrois->Draw("hmax", "status==0&&fabs(1+hmin/hmax)<0.1&&hmax>200")

Build and post dqm pedestal distributions for an event:
> ./procDqm RUN EVENT

Build and post pedestal distributions for an event:
> ./procPedDists RUN EVENT

Post DQM plots and copies of pedestal noise for a
range of runs.
> ./doPedNoise 8300 8310

Noise vs. channel, both cal and cnr:
> ./procRun 9000
Both directories will also have ROI plots and the cnr has noise information
including that needed for the script ./doit.

Noise without the ROIs:
> duneproc ib5taiNoiseVsChan/setIceberg5a iceberg008282 10
> view ib5taiNoiseVsChan/setIceberg5a/iceberg008282_proc000010/chmet_nsgrms_all_runRunNotFound_tai.png

Create DFT power plots for a run:
> duneproc ib5pedNoiseDft/setIceberg5C22 iceberg009613
> duneproc ib5pnrNoiseDft/setIceberg5C22 iceberg009613
Merge these plots:
> ibMergeDftPlots 9666 . ped:pnr
Or do both and pubish results
./doDft 9613

Integrated noise vs width
> duneproc 
root> .L drawIntegratedNoise.C
root> drawIntegratedNoise("ib5%REC%NoiseTree/setIceberg5p/iceberg%RUN%/adcrois.root", "9000", "ped,pnr")
root> drawIntegratedNoise("ib4%REC%NoiseTree/setIceberg4b/iceberg%RUN%/adcrois.root", "7100", "tai,cnr")


Lifetime analysis:

Create a file list to specify an ROI chain:
> ./findRoiTrees SPER
where SPER is processing run period. Rename and edit for sub-run periods.

Perform a lifetime fit using a for run period ROI chain:
> ./drawLifetime SPER
Use SPER=all to do all the run periods in runPeriods.txt.

Plot lifetime fits vs. time:
> root.exe 'drawLifetimeVsTime.C(19, 38)'
Arguments are the range of days to plot.

Publish lifetime results:
> rm ~/wwwdune/protodune/iceberg/lifetimes/*
> cp plots-tau/*.png ~/wwwdune/protodune/iceberg/lifetimes/
> makeIndex ~/wwwdune/protodune/iceberg/lifetimes/

