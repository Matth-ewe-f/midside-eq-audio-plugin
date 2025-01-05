# Split EQ
<p float="left">
  <img src="/screenshots/shot3.png" width="49%" />
  <img src="/screenshots/shot2.png" width="49%" /> 
</p>

A plugin capable of EQing individual channels of incoming audio, in a switchable stereo or mid-side configuration. With **Split EQ**, you can clean up bass frequencies by filtering them on the side channel. You can move an instrument in the stereo field by adjusting its brightness in the left or right channel. You can focus elements of your mix by boosting them in the mid channel. You can download Split EQ from the "Releases" section of this repository, or see the **[Installation Guide](#installation-guide)** if you'd like to build it from source yourself.

## Description
Split EQ allows you to apply EQ to each individual channel of an incoming stereo audio signal, as either stereo or mid-side. For each channel it provides:
* Low-Cut/Shelf Filter (Switchable)
* High-Cut/Shelf Filter (Switchable)
* Three Peak Filters
* Channel Gain

Each filter can be adjusted individually for each channel, or together for the entire audio signal. Split EQ also provides a visualization of both channel's frequency responses overlaid on top of each other, for easy comparison.

## Built With
* [CMake](https://cmake.org/)
* [JUCE](https://juce.com/)

## Installation Guide
Pre-build VST3 and Audio Unit version of Split EQ are available for download from the "Releases" section. You can also follow the below steps to build Split EQ from source yourself.

1. [Download and Install CMake](https://cmake.org/download/)

2. Verify that you have a C++ 23 compatible compiler on your machine

3. Clone this repository onto your local machine with the below command in a dedicated directory

    `git clone https://github.com/Matth-ewe-f/midside-eq-audio-plugin`

4. Install dependencies by running the below command in the same directory (this may take some time)

    `cmake -S . -B build`

5. Build the plugin by running the below command in the same directory (this may also take some time)

    `cmake --build build`

6. Plugin files will be located in subfolders of `build/plugin/Split-EQ_artefacts/` based on plugin format (VST, AU, etc). Choose the one you plan to use and install it as you would any other plugin.

<p float="left">
  <img src="/screenshots/shot4.png" width="49%" />
  <img src="/screenshots/shot1.png" width="49%" /> 
</p>
