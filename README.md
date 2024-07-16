# SBNDFieldDistortion
Standalone C++ and python scripts to analyze space charge effects and other E-Field distortions at SBND.


## Calculating Offsets
Code to calculate offsets lives here:
```
cd CalcOffsets
```

#### Setup
The following command sets up relevant products, creates a valid proxy, and compiles all relevant 
```
source setup.sh
```

#### Run Analyzer
The analyzer takes a text file of XRootD paths as input, and returns a ROOT file storing spatial offsets.
```
./sce_dx_analyzer <path to text file>
```
