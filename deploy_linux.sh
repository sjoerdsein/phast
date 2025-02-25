#!/bin/sh

set -e

buildDir=./build/
deployDir=./deployed/
appProjectName=phast_gui
appName=phast

# Add any additional plugis that chn be placed in the general "plugins" folder here
plugins=("decaycurve_view_plugin" "correlation_view_plugin" "fake_timetag_device_plugin" "timetrace_view_plugin")

# Get executable
mkdir -p ${deployDir}
cp ${buildDir}/${appProjectName}/${appName} ${deployDir}
strip ${deployDir}${appName}

# Let's ignore the regular dependencies for now...


# Copy plugins
mkdir -p ${deployDir}/plugins

for pl in "${plugins[@]}"
do
    cp ${buildDir}/${pl}/lib${pl}.so ${deployDir}/plugins/
    strip ${deployDir}/plugins/lib${pl}.so

    # Ignore plugin dependencies for now...
done


# Copy quTAG MC plugin
mkdir -p ${deployDir}/qutag_mc_plugin
cp ./qutag_mc_timetag_plugin/qutag_mc_lib/libtdcbase.so ${deployDir}/qutag_mc_plugin/
cp ${buildDir}/qutag_mc_timetag_plugin/libqutag_mc_timetag_plugin.so ${deployDir}/qutag_mc_plugin/
strip ${deployDir}/qutag_mc_plugin/libtdcbase.so
strip ${deployDir}/qutag_mc_plugin/libqutag_mc_timetag_plugin.so

# # Copy quTAG plugin: not supported on Linux
# mkdir -p ${deployDir}/qutag_plugin
# cp ./qutag_timetag_plugin/qutag_lib/libtdcbase.so ${deployDir}/qutag_plugin/
# cp ${buildDir}/qutag_timetag_plugin/libqutag_timetag_plugin.so ${deployDir}/qutag_plugin/
# strip ${deployDir}/qutag_plugin/libtdcbase.so
# strip ${deployDir}/qutag_plugin/libqutag_timetag_plugin.so

# # Copy quTAU plugin: not supported on Linux
# mkdir -p ${deployDir}/qutau_plugin
# cp ./qutau_timetag_plugin/qutau_lib/libtdcbase.so ${deployDir}/qutau_plugin/
# cp ${buildDir}/qutau_timetag_plugin/libqutau_timetag_plugin.so ${deployDir}/qutau_plugin/
# strip ${deployDir}/qutau_plugin/libtdcbase.so
# strip ${deployDir}/qutau_plugin/libqutau_timetag_plugin.so

# Create shortcut files to start using qutag or qutau plugins
echo "./phast -P qutag_mc_plugin" > ${deployDir}phast_qutag_mc
# echo "phast -P qutag_plugin" > ${deployDir}phast_qutag
# echo "phast -P qutau_plugin" > ${deployDir}phast_qutau

chmod +x ${deployDir}phast_qutag_mc
# chmod +x ${deployDir}phast_qutag
# chmod +x ${deployDir}phast_qutau

# Copy in readme and license file
cp LICENSE ${deployDir}license.txt
cp app_readme.txt ${deployDir}readme.txt