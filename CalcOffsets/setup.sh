#!/bin/bash                                                                                                                                             
source /cvmfs/sbnd.opensciencegrid.org/products/sbnd/setup_sbnd.sh
kx509                                                                                                                                                  
voms-proxy-init -noregen -rfc -voms fermilab:/fermilab/sbnd/Role=Analysis -valid 168:00                                                                
setup sbndcode v09_83_01 -q e26:prof
make
