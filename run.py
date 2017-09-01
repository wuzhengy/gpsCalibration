#!/usr/bin/python3
#-*-coding:utf-8-*-
import os
import sys
import signal
import time
import globalConfig
from subprocess import Popen

if __name__=="__main__":
        
    #long distance loam
    command_loam="roslaunch input_data input_data.launch input_bag_file:="+\
                 globalConfig.bag_input_filename + " output_track_file:="+\
                 globalConfig.slamTrackLongTmp +\
                 " total_distance:="+ str(globalConfig.totalLongDistance)+\
                 " overlap_distance:="+ "0.0;"
    print("command loam is: ", command_loam)
    pLoam= Popen(command_loam, shell= True)
    globalConfig.pidList[0]= pLoam.pid
    pLoam.wait()
    if(pLoam.returncode):
        globalConfig.function_end()
    
    #Get Long Distance Track Num
    trackNum= globalConfig.loam_file_search(globalConfig.slamTrackLongTmp)
    print ("long distance loam track file num is: ", trackNum)

    itrackNum= 1
    while(itrackNum<= trackNum):

        #Speed-Weight-Coeficient Calculation
        command_icp_weight_speed="roslaunch icp_weight_coe icp_weight_coe.launch"+\
             " coe_sort:="+ "1"+\
             " slam_track_file:="+ globalConfig.slamTrackLongTmp+ str(itrackNum)+\
             " gps_track_file:="+ "No_Need_File"+\
             " icp_track_file:="+ "No_Need_File"+\
             " weight_coe_file:="+ globalConfig.weightSpeedTmp+ str(itrackNum)+ "; "
        print("command icp weight speed is: ", command_icp_weight_speed)
        pICPWeightSpeed= Popen(command_icp_weight_speed, shell= True)
        globalConfig.pidList[1]= pICPWeightSpeed.pid
        pICPWeightSpeed.wait()
        if(pICPWeightSpeed.returncode):
            globalConfig.function_end()

        #Speed-LOAM-Weighted ICP
        command_segmentation_icp= "roslaunch long_distance_track_process long_distance_track_process.launch"+\
                                  " gps_track_file:="+ globalConfig.gps_input_filename+\
                                  " loam_track_file:="+ globalConfig.slamTrackLongTmp+ str(itrackNum)+\
                                  " gps_enu_file:="+ globalConfig.enuOriTmp+ str(itrackNum)+\
                                  " loam_enu_file:="+ globalConfig.enuProcessedTmp+ str(itrackNum)+\
                                  " icp_weight_file:="+ globalConfig.weightSpeedTmp+ str(itrackNum)+ \
                                  " icp_type:="+ "1"+ ";"
        print("command segmentation icp is: ", command_segmentation_icp)
        pSegICPProcess= Popen(command_segmentation_icp, shell= True)
        globalConfig.pidList[2]= pSegICPProcess.pid
        pSegICPProcess.wait()
        if(pSegICPProcess.returncode):
            globalConfig.function_end()

        for i in range(globalConfig.maxIterNum):
            #Speed And Weight-GPS-Reliable
            command_icp_weight_gps="roslaunch icp_weight_coe icp_weight_coe.launch"+\
                 " coe_sort:="+ "2"+\
                 " slam_track_file:="+ globalConfig.slamTrackLongTmp+ str(itrackNum)+\
                 " gps_track_file:="+ globalConfig.enuOriTmp+ str(itrackNum)+\
                 " icp_track_file:="+ globalConfig.enuProcessedTmp+ str(itrackNum)+\
                 " weight_coe_file:="+ globalConfig.weightGPSTmp+ str(itrackNum)+"; "
            print("command icp weight is: ", command_icp_weight_gps)
            pICPWeightGPS= Popen(command_icp_weight_gps, shell= True)
            globalConfig.pidList[3]= pICPWeightGPS.pid
            pICPWeightGPS.wait()
            if(pICPWeightGPS.returncode):
                globalConfig.function_end()

            #Speed And Weighted-GPS ICP
            command_segmentation_icp= "roslaunch long_distance_track_process long_distance_track_process.launch"+\
                                  " gps_track_file:="+ globalConfig.gps_input_filename+\
                " loam_track_file:="+ globalConfig.enuProcessedTmp+ str(itrackNum)+\
                " gps_enu_file:="+ globalConfig.enuOriTmp+ str(itrackNum)+\
                " loam_enu_file:="+ globalConfig.enuProcessedTmp+ str(itrackNum)+\
                " icp_weight_file:="+ globalConfig.weightGPSTmp+ str(itrackNum)+\
                " icp_type:="+ "1"+ ";"
            print("command segmentation icp is: ", command_segmentation_icp)
            pSegICPProcess= Popen(command_segmentation_icp, shell= True)
            globalConfig.pidList[4]= pSegICPProcess.pid
            pSegICPProcess.wait()
            if(pSegICPProcess.returncode):
                globalConfig.function_end()

        itrackNum+=1
   
    #Merge GPS Data And Weight Data
    command_merge_xyztw= "rosrun data_preprocess data_preprocess_node "+\
                         globalConfig.enuOriTmp+ " "+\
                         globalConfig.weightGPSTmp+ " "+\
                         globalConfig.EWMergeTmp+" "+ str(trackNum);
    print("command merge xyztw is: ", command_merge_xyztw)
    pMergeData= Popen(command_merge_xyztw, shell= True)
    globalConfig.pidList[5]= pMergeData.pid
    pMergeData.wait()
    if(pMergeData.returncode):
        globalConfig.function_end()
    
    #short distance loam
    command_loam="roslaunch input_data input_data.launch input_bag_file:=" +\
        globalConfig.bag_input_filename+ " output_track_file:="+\
        globalConfig.slamTrackShortTmp +\
        " total_distance:=" + str(globalConfig.totalShortDistance)+\
        " overlap_distance:="+ str(globalConfig.opDistance)+ ";"
    print("command loam is: ", command_loam)
    pLoam= Popen(command_loam, shell= True)
    globalConfig.pidList[6]= pLoam.pid
    pLoam.wait()
    if(pLoam.returncode):
        globalConfig.function_end()
    
    #Get Short Distance Track Num
    trackNum= globalConfig.loam_file_search(globalConfig.slamTrackShortTmp)
    print ("short distance loam track file num is: ", trackNum)

    #Short Distance Track ICP With GPS Track
    command_segmentation_icp= " rosrun short_distance_track_icp short_distance_track_icp_node "+\
                globalConfig.slamTrackShortTmp+" "+\
                globalConfig.EWMergeTmp+" "+ str(trackNum)
    print("command segmentation icp is: ", command_segmentation_icp)
    pSegICPProcess= Popen(command_segmentation_icp, shell= True)
    globalConfig.pidList[7]= pSegICPProcess.pid
    pSegICPProcess.wait()
    if(pSegICPProcess.returncode):
        globalConfig.function_end()

    #Merge Short Distance Results Data
    command_merge_results= "rosrun results_merge results_merge_node "+\
                         str(trackNum)+" "+ globalConfig.enuProcessedFinal
    print("command merge results is: ", command_merge_results)
    pMergeResults= Popen(command_merge_results, shell= True)
    globalConfig.pidList[8]= pMergeResults.pid
    pMergeResults.wait()
    if(pMergeResults.returncode):
        globalConfig.function_end()

    #Original GPS Track: ENU->KML
    command_enu_to_gps= "./devel/local_to_GPS "+ globalConfig.EWMergeTmp+ " "+\
                        globalConfig.ctm+ " "+\
                        globalConfig.gdt+ " "+\
                        "0"+ " "+ globalConfig.gps_original_filename+ "; "
    print("command enu to gps is: ", command_enu_to_gps)
    pENUToGPS= Popen(command_enu_to_gps, shell= True)
    globalConfig.pidList[9]= pENUToGPS.pid
    pENUToGPS.wait()
    if(pENUToGPS.returncode):
        globalConfig.function_end()

    #Processed GPS Track: ENU->KML
    command_enu_to_gps= "./devel/local_to_GPS "+ globalConfig.enuProcessedFinal+ " "+\
                        globalConfig.ctm+ " "+\
                        globalConfig.gdt+ " "+\
                        "1"+ " "+ globalConfig.gps_improved_filename+ "; "
    print("command enu to gps is: ", command_enu_to_gps)
    pENUToGPS= Popen(command_enu_to_gps, shell= True)
    globalConfig.pidList[10]= pENUToGPS.pid
    pENUToGPS.wait()
    if(pENUToGPS.returncode):
        globalConfig.function_end()

    #Clean Up The Enviroment
    globalConfig.clear_eviriment()
