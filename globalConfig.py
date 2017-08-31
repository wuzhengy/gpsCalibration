#!/usr/bin/python3
#-*-coding:utf-8-*-
import os
import sys
import signal
from subprocess import Popen

'''
    User Profiles
    1. bag_input_filename:
       input point cloud bag file list

    2. gps_input_filename:
       original GPS data with type GPRMC

    3. gps_original_filename:
       original GPS track type with KML format

    4. gps_improved_filename:
       imporved accurate GPS track type with KML format
'''
bag_input_filename= "./data/bag_list.txt"
gps_input_filename= "./data/original_gps_data.txt"
gps_original_filename=  "./data/original_gps_track.kml"
gps_improved_filename=  "./data/improved_gps_track.kml"

'''
    Project Profiles:
    1. ctm:
       WGS-84 coordinate to ENU local coordinates method:
       UTM, Gaussian

    2. gdt:
       GPS divide_type refers to longitude:
       3, 6

    3. slamTrackLongTmp:
       long distance slam-loam track temporary file

    4. slamTrackShortTmp:
       short distance slam-loam track temporary file

    5. weightSpeedTmp:
       weight coefficient temporary file calculated by speed

    6. weightGPSTmp:
       weight coefficient of GPS confidence level temporary file

    7. EWMergeTmp
       temporary file of merged long distance track results

    8. weightFinal
       1st merge of weight coeficient calculated by long distance track process module

    9. enuOriTmp
       original GPS track temporary file in ENU coordinate

    10. enuProcessedTmp:
       calibrated GPS track temporary file in ENU coordinate 

    11. enuProcessedFinal:
       calibrated GPS track merged file in ENU coordinate 

    12. gpsProcessedFinal:
       calibrated GPS track
'''
ctm= "UTM"
gdt= "3"
slamTrackLongTmp=  "./data/loam_track_long_distance_temp_"
slamTrackShortTmp= "./data/loam_track_short_distance_temp_"
weightSpeedTmp=    "./data/weight_coe_speed_"
weightGPSTmp=      "./data/weight_coe_gps_"
EWMergeTmp=        "./data/merge_gps_weight_file"
weightFinal=       "./data/weightFinal"
enuOriTmp=         "./data/enuOriTmp_"
enuProcessedTmp=   "./data/enuProcessedTmp_"
enuProcessedFinal= "./data/enuProcessedFinal"
gpsProcessedFinal= "./data/merge_results_file"

'''
    Project parameters:
    1. totalLongDistance:
        length of long distance lidar SLAM
        (600m-1000m, recommended)

    2. totalShortDistance:
        length of short distance lidar SLAM
        (200m-300m, recommended)

    3. opDistance:
        length of overlapped short distance lidar SLAM

    4. number of iteration in track registration        
'''
totalLongDistance= 1000.0
totalShortDistance= 300.0
opDistance= 100.0
maxIterNum= 5

pidList=[0]*11

def function_end():
    '''
        end the program processing
    '''
    print("Abnormal END, Please Check It!!!") 
    #kill the processes
    for pidTemp in pidList:
        killCommand= "kill -9 "+ str(pidTemp)
        Popen(killCommand, shell= True)

def loam_file_search(slamTrackTmp):
    '''
        get the bag segment num
    '''
    fileNum= 1
    while(1):
        filename= slamTrackTmp+ str(fileNum)
        if(os.path.exists(filename)== False):
            break
        fileNum+= 1

    return fileNum- 1

def clear_eviriment():
    '''
        clean up the temp files
    '''
    #rm the lidar slam track
    sysRM= "rm "+ slamTrackLongTmp+ "* "+ " "+ slamTrackShortTmp+ "* "
    os.system(sysRM)

    #rm the GPS temporary file
    sysRM= "rm "+ enuOriTmp+"* "+ enuProcessedTmp+ "* "
    os.system(sysRM)
    sysRM= "rm "+ enuProcessedFinal
    os.system(sysRM)

    #rm the w temporary files
    sysRM= "rm "+ weightSpeedTmp+"* "+ weightGPSTmp+ "* "
    os.system(sysRM)
    sysRM= "rm "+ EWMergeTmp
    os.system(sysRM)
