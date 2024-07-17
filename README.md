# SBNDFieldDistortion
Standalone C++ and python scripts to analyze space charge effects and other E-Field distortions at SBND.

This code runs on the SBND GPVMs.  To set up the necessary container:
```
sh /exp/$(id -ng)/data/users/vito/podman/start_SL7dev_jsl.sh
```

## Calculating Offsets
Code to calculate offsets lives in `CalcOffsets/`.  
sh
A setup script gives us necessary products, creates a valid proxy, and compiles C++ code.

`sce_dx_analyzer.cpp` takes a text file of XRootD paths as input, and returns an outpur ROOT file with spatial offsets.
```
cd CalcOffsets
source setup.sh
./sce_dx_analyzer <path to text file>
```
## Quick Analysis
A python script exists that gives quick plotting results.  This is meant to be used for small datasets (e.g. a single run), not large-scale calibration.

Start by navigating to the `QuickPlot/` folder and creating a virtual python environment.
```
cd QuickPlot
source setup.sh
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```
