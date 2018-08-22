This gives an instrution on how to generate histograms and plots about HB Timing based on PFG HcalTupleMaker (Will include HF Timing later)

> Credits: Changes made based on this project:
> Github website: https://github.com/HCALPFG/HcalTupleAnalyzer

---
 
Tasks generally have two categories:
* charge averaged timing
* rechit timing (include Method0 and MAHI)

corresponding to two .C files under the ```macro``` directory: analysisClass_HBHETiming_reco.C and analysisClass_HBHETiming_digi.C
Normally, people in PFG group cares about charge averaged timing while some people may want rechit timing.

# Make Histograms

To run the code, please provide the bx.json and lumi.json, otherwise need to comment corresponding code in the .C file. (Normally only isolated bunches are used for timing study) Note you may want to update these two files before you really want to use them. Of course, lumi part can be done in the Tuple making step. There's a link: https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/PromptReco/Cert_314472-317696_13TeV_PromptReco_Collisions18_JSON.txt, where you can check lumi sections.

In the .C file, there are some histograms that can check specific channels, ieta and iphi can be set through ieta_check_*, ieta_compare1_* and ieta_compare2_* variables. Also the array _iphi includes some iphis that will be plotted each ieta for some variables. But the first element must be zero meanning the all iphis in that ieta. It has to be mentioned here that _iphi information needs to be included in the plot script later to have the correct plotting, meaning that you include _phi = [0,*,*] in the beginning of plot.py(or whatever name your ploting script is).

* For the analysisClass_HBHETiming_reco.C:
can generate Method0 and MAHI histograms: method = 1 and 2
Please check the nmethod=3 in the .C file before run if you want both information
* For the analysisClass_HBHETiming_digi.C:
generate charge average timing by setting nmethod=1. It's not recommended to change this number to 2 or 3 since there's no corresponding codes to plot these histograms.


__To run the code, of course you have to compile it by ```make```. Before the ```make```, please make sure you have the file analysisClass.C under the directory ```src``` which is a soft link to the .C file you want to run. And run for example:__ ```./main  ./config/input_lists/main_input.txt ./tree_lists/tree_list.txt ./output.root ```

_And to speed up, you may need to want use batch mode.To combine multiple root files into one file, use command: hadd result.root output*.root_

To use batch mode, please refer to the folder: work/Tools/batchmode, where you can find the script: run.sh. You have to have your exectable file (like: main after you ```make```) and replace the name in the run.sh script (for example, main_check in the file) and put your NTuples path in the task_m0.txt file.

# Make Plots
Next step is to plot out the histograms. Corresponding, you have tow scripts: plot_reco.py and plot_digi.py. To run: ```python plot_reco.py output.root```

Also, people want to know the timing vs Run# which can be done through the file: analysisClass_HBHETiming_run.C, and then plotted by plot_run.py

Note, whatever you change in the .C file, please make sure to edit in the plot script accordingly.

__It has to be mentioned if you directly use HCALTupleMaker (https://github.com/HCALPFG/HcalTupleMaker) to generate the NTuples, you may not have reco infos then you cannot run this project on these NTuples about the reco part.__
