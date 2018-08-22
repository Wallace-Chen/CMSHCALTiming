import os
import commands
import time
from subprocess import call
import subprocess
from sys import argv
  
start = time.time()

#color options:
HEADER   = '\033[95m'
BLUE     = '\033[94m'
GREEN    = '\033[92m'
WARNING  = '\033[93m'
FAIL     = '\033[91m'
BOLD     = '\033[1m'
UNDERLINE= '\033[4m'
END      = '\033[0m'

if not len(argv)==5:
  print FAIL + "Input not valid! \n Simple Usage:\n python run.py macros/analysisClass_HFPhaseScan.C config/input_lists/simple_list.txt config/tree_lists/treelist.txt output.root"
  print "2-input type Usage:\n python run.py macros/analysisClass_uHBHECommissioning.C config/input_lists/main_input.txt config/tree_lists/tree_list.txt output.root" + END
  exit(1)
else:
  status,infile = commands.getstatusoutput("echo " + argv[1] + " | sed 's/\.[^.]*$//'")

  if(os.path.isfile('./src/analysisClass.C')):
    if not argv[1] in  os.readlink("./src/analysisClass.C") and os.path.exists("./src/analysisClass.o"):
      os.remove("./src/analysisClass.o")
      print "Delete ./src/analysisClass.o"
    os.remove("./src/analysisClass.C")
    print "Delete ./src/analysisClass.C"

  try:
    os.symlink('../'+argv[1],"./src/analysisClass.C")
  except OSError:
    if(os.path.isfile('./'+argv[1])):
      os.remove("./src/analysisClass.C")
      os.symlink('../'+argv[1],"./src/analysisClass.C")
    else:
      print FAIL + argv[1] + " does NOT exist, program terminated" + END
      exit()
  print "Making symlink from ./" + argv[1] + " to ./src/analysisClass.C"
  print "Compiling ..."
  exitcode = call("make")
  if not exitcode==0:
    print FAIL + "Compile failed, program terminated " + END
    exit()
  else:	
    elapsed = time.time() - start
    print GREEN + "Compiling succeed, now making Histograms, time used now is: " + str(elapsed) + " secs..." + END
    exitcode = call(["./main",argv[2],argv[3],argv[4]])
    if not exitcode == 0:
      print FAIL + "Error occured when creating " + argv[4] + " ,program terminated" + END
      exit()
    else:
      elapsed = time.time() - start
      print GREEN + argv[4] + " created successfully, now making plot, time used now is: " + str(elapsed) + " secs..." + END
      exitcode = call("python plot.py "+argv[4],shell=True)
      if not exitcode == 0:
        print FAIL + "Error Occured when ploting!" + END
      else:
        print GREEN + "All Done!" + END
        subprocess.Popen(["evince",infile+"_HB_timing_general.pdf"])
