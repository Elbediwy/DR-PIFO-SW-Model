# DR-PIFO-SW-Model
To build and run:

1. Download the provided folders in "P4_simulation/" and save them in the main directory of your workspace.

2. Make sure these tools are installed in your workspace : Mininet, P4c, P4runtime and BMv2.
Optional : you can find these tools installed in the provided VM from https://github.com/p4lang/tutorials

3. Copy the files provided in "P4_simulation/BMv2 Files/" to the directory of the simple_switch in your system "behavioral-model/targets/simple_switch" (replace the already existed files, if needed)

4. In "behavioral-model/targets/simple_switch/simple_switch.cpp", for the lines from 42-45, include only the model of the packet scheduler that you would like to test.
https://github.com/Elbediwy/DR-PIFO-SW-Model/blob/0388caaab889b33689a67d131f58b0db828faf32/P4_simulation/BMv2%20files/simple_switch.cpp#L42:45

For example, if you uncommented only "#include "TM_buffer_dr_pifo.h"", so you will use the DR-PIFO packet scheduler model in the BMv2 model. 

5. In the directory of BMv2 "behavioral-model/", run these commands : 
```bash
./autogen.sh
./configure
sudo make
sudo make install
sudo ldconfig
```
optional, in "behavioral-model/targets/simple_switch" and "behavioral-model/targets/simple_switch_grpc", you can run these commands:
```bash
sudo make
sudo make install
sudo ldconfig
```
6. For the DR-PIFO, in the "P4_simulation/utils/user_externs_dr_pifo/", run these commands : 
```bash
sudo make clean
sudo make
```

7. For the DR-PIFO, in the "P4_simulation/utils/user_externs_dr_pifo/p4runtime_switch.py", uncomment the line refers to the folder "user_externs_dr_pifo", from line 122 to 125 (which is 122 for the DR-PIFO).

8. For the DR-PIFO, copy the content of the file "P4_simulation/utils/program/qos/p4 programs/DR_PIFO.p4" to this file "P4_simulation/utils/program/qos/qos.p4"

9. In "P4_simulation/utils/program/qos/", run these commands :
```bash
sudo make stop
sudo make clean
sudo make
```

10. Then, wait until the simulation is finished. (~ 30 mins)

You can find the log files of each switch in "P4_simulation/utils/program/qos/logs"

You can also find the received packets by each host in "P4_simulation/utils/program/qos/receiver_h'#host_id'"

11. To test other models, such as PIFO or PIEO or Ideal pFabric, repeat the steps from 4 to 10, with changing "DR-PIFO" to "pFabric" in each step.
