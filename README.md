# Parallel port output plugin
A plugin that sends TTL events for a given channel over a parallel port. 

![Parallel port plugin image](parallel-port-image.PNG)

### Parameters

* In: The input TTL channel which triggers events. The parallel port will be set to high when an incoming "on" TTL event comes in on this channel.
* Port: The address of the LPT port. This can be found through device manager and clicking on "properties", "resources", and looking at the I/O range. Some experimentation may be required.
* Duration: This is how long the port will be set to high after a trigger event comes in.

## Installation
### Installing the 0MQ library
Currently, only Windows 64-bit systems are supported through using the inpoutx64.dll which is included in the libs folder. 

### Building the plugins
Building the plugins requires [CMake](https://cmake.org/). Detailed instructions on how to build open ephys plugins with CMake can be found in [our wiki](https://open-ephys.atlassian.net/wiki/spaces/OEW/pages/1259110401/Plugin+CMake+Builds).

## Parallel Port additional information
### Drivers and parallel port connected
* Obtain the appropriate drivers from the manufacturer's website.
* Install the drivers correctly.
* After installation, open Device Manager.
* If the "Ports (COM & LPT)" is not visible, go to View and select Show hidden devices.
* Verify that the parallel port appears under "Ports (COM & LPT)".
* The parallel port will be listed as LPT.

### Parallel port installed but is not visible on the OE GUI.
* Check the command prompt for any error messages under the parallel-port-output. If it shows "DLL Error 0x7e," proceed to the next step.
* Copy the DLL file from the lib folder.
* Paste the DLL file into the shared folder located in plugin-GUI--Build--Release--shared.
* Next, Make sure the DLLs in the plugin-GUI--Build--Release--plugin folder match the DLLs built for the parallel port.
