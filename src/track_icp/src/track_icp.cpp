#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <math.h>
#include <cstdio>
#include <iomanip>
#include <algorithm>
#include <math.h>
#include <typeinfo>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

#define IMDP 15

using namespace Eigen;
using namespace std;

bool isAddWeight = false;
bool isCutOffICP = false;

//Define the numPoint to get the Point's number
int numPoint = 0;

//Define the breakPoint 
int breakPointNum = 0;

//Define the array to get slam track and GPS track
double x0GPS = 0.0;
double y0GPS = 0.0;
double *zTemp;
double *timeTemp;
double *gpsTimeTemp;
double *arrayWCTemp;

//Define string to get filename;
string fileSLAMTrack;
string fileGPSTrack;
string fileisWeightCoe;
string fileWeightCoe;
string fileDoneICPTrack;

//Define the matrix
MatrixXd slamCoordAll;
MatrixXd gpsCoordAll;
MatrixXd weightCoe;

MatrixXd aCoord;
MatrixXd bCoord;

MatrixXd transformT;
MatrixXd distancesD;

MatrixXd slamRotatedCoord;
MatrixXd gpsReCoord;
MatrixXd gpsReCoordAll;
MatrixXd gpsReAverCoordAll;

/*Read the matrix slam track to slamCoordAll
    Input: the txt of slamTrack  
*/
void slamTrackRead(string fileTrack)
{
    cout<<"Start To Read The Slam Track: " << fileTrack <<endl;
    
    int sumTrack = 0;
    int numTrack = 0;
    int numLine = 0;
    int rows = 0;
    int cols = 3;
    
    double **mtTemp;

    ifstream ifsTemp(fileTrack.c_str());
    ifstream ifs(fileTrack.c_str());
    string readTrackTemp;
    string readTrack;

    while(getline(ifsTemp,readTrackTemp))
    {
        rows++;
    }
 
    mtTemp = new double *[rows];
    for(int i=0;i<=rows-1;i++)
    {
        mtTemp[i] = new double[cols];
    }
 
    zTemp = new double[rows];
    timeTemp = new double[rows];
    while(getline(ifs,readTrack))
    {
        istringstream getTrack(readTrack); 
        string getTrackData;

        while(getline(getTrack,getTrackData,' '))
        {   
            if(numTrack==0)
            {
                mtTemp[numLine][0] = atof(getTrackData.c_str());
            }
            else if(numTrack==1)
            {
                mtTemp[numLine][1] = atof(getTrackData.c_str());                
            }
            else if(numTrack==2)
            {
                mtTemp[numLine][2] = atof(getTrackData.c_str());
                zTemp[numLine] = atof(getTrackData.c_str()); 
            }
            else
            {
                timeTemp[numLine] = atof(getTrackData.c_str());
                numTrack = 0;
                continue;
            }
            numTrack++; 
        }
        numLine++;
        numPoint++;
    }

    slamCoordAll.setOnes(numLine, 4);
   
    for(int i= 0; i< numLine; i++)
    {
        for(int j= 0; j< 3; j++)
        {
            slamCoordAll(i,j) = mtTemp[i][j];
        }
    }

    for(int i=0;i<=rows-1;i++)
    {
        delete[] mtTemp[i];
    }
    delete mtTemp;   
}

/*Read the matrix GPS track
    Input: the txt of GPS
*/
void gpsTrackRead(string fileTrack)
{
    cout<<"Start To Read The GPS Track: " << fileTrack <<endl;

    int sumTrack = 0;
    int numTrack = 0;
    int numLine = 0;
    int rows = 0;
    int cols = 3;

    double **mtTemp;
    
    ifstream ifsTemp(fileTrack.c_str());
    ifstream ifs(fileTrack.c_str());
    string readTrackTemp;
    string readTrack;

    while(getline(ifsTemp,readTrackTemp))
    {
        rows++;
    }
    
    mtTemp = new double *[rows];
    for(int i=0;i<=rows-1;i++)
    {
        mtTemp[i] = new double[cols];
    }
 
    gpsTimeTemp = new double[rows];
    while(getline(ifs,readTrack))
    {
        istringstream getTrack(readTrack);
        string getTrackData;

        while(getline(getTrack, getTrackData,' '))
        {
            if(numTrack==0)
            {
               mtTemp[numLine][numTrack] = atof(getTrackData.c_str()); 
            }
            else if(numTrack==1)
            {
               mtTemp[numLine][numTrack] = atof(getTrackData.c_str());
            }
            else if(numTrack==2)
            {
               mtTemp[numLine][numTrack] = atof(getTrackData.c_str());
            }
            else 
            {
               gpsTimeTemp[numLine] = atof(getTrackData.c_str());
               numTrack = 0;
               continue;
            }            
            sumTrack++;
            numTrack++;
        }
        numLine++;
    }

    gpsCoordAll.setOnes(numLine,4);

    for(int i= 0;i< numLine; i++)
    {
        for(int j= 0; j< 4; j++)
        {
            if(i==0)
            {
               if(j==0)
               {
                  x0GPS = mtTemp[i][j];
                  mtTemp[i][j] = 0.0;
                  gpsCoordAll(i,j) = 0.0;
               }
               else if(j==1)
               {
                  y0GPS = mtTemp[i][j];
                  mtTemp[i][j] = 0.0;
                  gpsCoordAll(i,j) = 0.0;
               }
               else
               {
                  gpsCoordAll(i,j) = mtTemp[i][j];
               }
            }
            else
            {
               if(j==0)
               {
                  gpsCoordAll(i,j) = mtTemp[i][j] - x0GPS;
               }
               else if(j==1)
               {
                  gpsCoordAll(i,j) = mtTemp[i][j] - y0GPS;
               }
               else
               {
                  gpsCoordAll(i,j) = mtTemp[i][j];
               }
            }     
        } 
    }
    
    for(int i=0;i<=rows-1;i++)
    {
        delete [] mtTemp[i];
    }
    delete mtTemp;
}

/*Read the weightCoe from WeightCoeFile
    Input: the txt of WeightCoeFile
*/
void weightCoeRead(string fileWC)
{
    cout<<"Start To Read The fileWeightCore : " << fileWC <<endl;

    int numLine = 0;
    int rows = 0;
    
    ifstream ifsTemp(fileWC.c_str());
    ifstream ifs(fileWC.c_str());
    string readWeightCoeTemp;
    string readWeightCoe;
    
    while(getline(ifsTemp,readWeightCoeTemp))
    {
	rows++;
    }
    
    arrayWCTemp = new double[rows];
    while(getline(ifs,readWeightCoe))
    {
        arrayWCTemp[numLine] = atof(readWeightCoe.c_str());
        numLine++;
    }
}

/*Find the nearest (Euclidean) neighbor in dst for each point in src
    Input:
        src: Nx3 array of points
        dst: Nx3 array of points
    Output:
        distances: Euclidean distances of the nearest neighbor
        indices: dst indices of the right neighbor 
*/
void nearestNeighbor(MatrixXd inputSrc,MatrixXd inputDst,MatrixXd *outputDistances,MatrixXd *outputIndices)
{
    //the matrix of inputSrc's rows;
    int srcLength = inputSrc.rows();

    //count 
    int jNum = 0;
    
    //N*1,index:0~N
    MatrixXd tempIndices;
    tempIndices.setOnes(srcLength,1);
    
    //N*N,euclidean distance 
    MatrixXd tempDistances;
    tempDistances.setOnes(srcLength,srcLength);
    
    //N*1,euclidean distances of the nearest neighbor
    MatrixXd tempDst;
    tempDst.setOnes(srcLength,1);

    for(int i= 0; i< srcLength; i++)
    {
        tempIndices(i,0)= i;
    }
    
    *outputIndices = tempIndices;

    for(int i=0;i<srcLength;i++)
    {   
        for(int j=0;j<srcLength;j++)
        {
            tempDistances(i,j) = sqrt((inputSrc(i,0)-inputDst(j,0))*(inputSrc(i,0)-inputDst(j,0)) + (inputSrc(i,1)-inputDst(j,1))*(inputSrc(i,1)-inputDst(j,1)));
        }       
    }
    
    for(int i=0;i<srcLength;i++)
    {
        tempDst(i,0) = tempDistances(i,jNum);
        jNum++;     
    }
    *outputDistances = tempDst;  
}

/*Calculates the least-squares best-fit transform between corresponding 3D points A->B
    Input:
        A: N*3 the matrix of corresponding 3D points
        B: Nx3 the matrix of corresponding 3D points
    Returns:
        T: 4x4 homogeneous transformation matrix
        R: 3x3 rotation matrix
        t: 3x1 column vector
*/
void BFTWithWeight(MatrixXd A,MatrixXd B,MatrixXd *outputT)
{
    int ALength= A.rows();
    int BLength= B.rows();

    double sumA_X = 0.0;
    double sumA_Y = 0.0;
    double sumA_Z = 0.0;

    double sumB_X = 0.0;
    double sumB_Y = 0.0;
    double sumB_Z = 0.0;
    
    double sumW = 0.0;

    //N*3,translate points to their centroids
    MatrixXd centroid_A;
    MatrixXd centroid_B;
    
    //N*3
    MatrixXd A1;
    MatrixXd B1;   

    //N*3
    MatrixXd AA;
    MatrixXd BB;

    //N*3
    MatrixXd tempA;
    MatrixXd tempB;

    //1*3
    MatrixXd tempAA;
    MatrixXd tempBB;

    A1.setOnes(ALength,3);
    B1.setOnes(BLength,3);

    centroid_A.setOnes(ALength, 3);
    centroid_B.setOnes(BLength, 3);

    AA.setOnes(ALength, 3);
    BB.setOnes(BLength, 3);

    tempA.setOnes(ALength, 3);
    tempB.setOnes(BLength, 3);

    tempAA.setOnes(1, 3);
    tempBB.setOnes(1, 3);
    
    for(int i=0;i<ALength;i++)
    {
        for(int j=0;j<3;j++)
        {
            if(j==0)
            {
                A1(i,j) = A(i,j) * arrayWCTemp[i];
                B1(i,j) = B(i,j) * arrayWCTemp[i];
            }
            else if(j==1)
            {
                A1(i,j) = A(i,j) * arrayWCTemp[i];
                B1(i,j) = B(i,j) * arrayWCTemp[i];
            }
            else
            {
                A1(i,j) = A(i,j) * arrayWCTemp[i];
                B1(i,j) = B(i,j) * arrayWCTemp[i];
            }
        }
    }
   
    for(int i=0;i<ALength;i++)
    {
        for(int j=0;j<3;j++)
        {
            if(j==0)
            {
                sumA_X += A1(i,j);
                sumB_X += B1(i,j);
            }
            else if(j==1)
            {
                sumA_Y += A1(i,j);
                sumB_Y += B1(i,j);
            }
            else
            {
                sumA_Z += A1(i,j);
                sumB_Z += B1(i,j);
            }  
        }
        sumW += arrayWCTemp[i];     
    }
   
    for(int i=0;i<ALength;i++)
    {
        for(int j=0;j<3;j++)
        {
            tempA(i,j) = A(i,j);
            tempB(i,j) = B(i,j);
        }
    }
    
    sumA_X = sumA_X/sumW;
    sumA_Y = sumA_Y/sumW;
    sumA_Z = sumA_Z/sumW;
    
    sumB_X = sumB_X/sumW;
    sumB_Y = sumB_Y/sumW;
    sumB_Z = sumB_Z/sumW;
    
    for(int i=0;i<3;i++)
    {
        if(i==0)
        {
            tempAA(0,i) = sumA_X;
            tempBB(0,i) = sumB_X;
        }
        else if(i==1)
        {
            tempAA(0,i) = sumA_Y;
            tempBB(0,i) = sumB_Y;
         }
        else
        {
            tempAA(0,i) = sumA_Z;
            tempBB(0,i) = sumB_Z;
        }
    }
    
    for(int i=0;i<ALength;i++)
    {
        centroid_A(i,0) = sumA_X;
        centroid_A(i,1) = sumA_Y;
        centroid_A(i,2) = sumA_Z;

        centroid_B(i,0) = sumB_X;
        centroid_B(i,1) = sumB_Y;
        centroid_B(i,2) = sumB_Z;
    }
    
    //translate points to their centroids
    AA= tempA- centroid_A;
    BB= tempB- centroid_B;
    
    for(int i=0;i<ALength;i++)
    {
        for(int j=0;j<3;j++)
        {
            if(j==0)
            {
                 AA(i,j) = AA(i,j) * arrayWCTemp[i];
                 BB(i,j) = BB(i,j) * arrayWCTemp[i];
            }
            else if(j==1)
            {
                 AA(i,j) = AA(i,j) * arrayWCTemp[i];
                 BB(i,j) = BB(i,j) * arrayWCTemp[i];
            }
            else
            {    
                 AA(i,j) = AA(i,j) * arrayWCTemp[i];
		         BB(i,j) = BB(i,j) * arrayWCTemp[i];
            }
        }
    }
    
    //rotation matrix
    MatrixXd H= (AA.transpose())*(BB);

    JacobiSVD<Eigen::MatrixXd> svd(H,ComputeThinU | ComputeThinV );
    MatrixXd U= svd.matrixU();
    MatrixXd Vt= svd.matrixV();
    MatrixXd S= svd.singularValues();
   
    MatrixXd R = Vt*U.transpose();

    //special reflection case
    if(R.determinant()< 0)
    {
        for(int i= 0; i< Vt.cols(); i++)
        {
            Vt(i, 2) = (-1) * Vt(i, 2);
        }
        R = Vt* U.transpose();
    }

    //translation
    MatrixXd t = tempBB.transpose() - ((R)*(tempAA.transpose()));
    
    //homogeneous transformation
    MatrixXd T;
    T.setIdentity(4, 4);

    for(int i= 0; i< 3;i++)
    {
        for(int j= 0; j< 3; j++)
        {
           
            T(i, j)= R(i, j);
        }
        double temp = t(i,0);
        T(i,3) = temp ;
    }
    *outputT = T;
}

/*Calculates the least-squares best-fit transform between corresponding 3D points A->B
    Input:
        A: Nx3 numpy array of corresponding 3D points
        B: Nx3 numpy array of corresponding 3D points
    Returns:
        T: 4x4 homogeneous transformation matrix
        R: 3x3 rotation m)atrix
        t: 3x1 column vector
*/
void bestFitTransform(MatrixXd A,MatrixXd B,MatrixXd *outputT)
{
    int ALength= A.rows();
    int BLength= B.rows();

    double sumA_X = 0.0;
    double sumA_Y = 0.0;
    double sumA_Z = 0.0;

    double sumB_X = 0.0;
    double sumB_Y = 0.0;
    double sumB_Z = 0.0;

    //N*3,translate points to their centroids
    MatrixXd centroid_A;
    MatrixXd centroid_B;
    
    //N*3
    MatrixXd AA;
    MatrixXd BB;

    //N*3
    MatrixXd tempA;
    MatrixXd tempB;

    //1*3
    MatrixXd tempAA;
    MatrixXd tempBB;

    centroid_A.setOnes(ALength, 3);
    centroid_B.setOnes(BLength, 3);

    AA.setOnes(ALength, 3);
    BB.setOnes(BLength, 3);

    tempA.setOnes(ALength, 3);
    tempB.setOnes(BLength, 3);

    tempAA.setOnes(1, 3);
    tempBB.setOnes(1, 3);
    
    for(int i=0;i<ALength;i++)
    {
        for(int j=0;j<3;j++)
        {
            if(j==0)
            {
                sumA_X += A(i,j);
                sumB_X += B(i,j);
            }
            else if(j==1)
            {
                sumA_Y += A(i,j);
                sumB_Y += B(i,j);
            }
            else
            {
                sumA_Z += A(i,j);
                sumB_Z += B(i,j);
            }   
        }
    }
    
    for(int i=0;i<ALength;i++)
    {
       for(int j=0;j<3;j++)
        {
            tempA(i,j) = A(i,j);
            tempB(i,j) = B(i,j);
        }
    }
    
    sumA_X = sumA_X/ALength;
    sumA_Y = sumA_Y/ALength;
    sumA_Z = sumA_Z/ALength;
    
    sumB_X = sumB_X/BLength;
    sumB_Y = sumB_Y/BLength;
    sumB_Z = sumB_Z/BLength;
    
    for(int i=0;i<3;i++)
    {
        if(i==0)
        {
            tempAA(0,i) = sumA_X;
            tempBB(0,i) = sumB_X;
        }
        else if(i==1)
        {
            tempAA(0,i) = sumA_Y;
            tempBB(0,i) = sumB_Y;
        }
        else
        {
            tempAA(0,i) = sumA_Z;
            tempBB(0,i) = sumB_Z;
        }
    }
    
    for(int i=0;i<ALength;i++)
    {
        centroid_A(i,0) = sumA_X;
        centroid_A(i,1) = sumA_Y;
        centroid_A(i,2) = sumA_Z;

        centroid_B(i,0) = sumB_X;
        centroid_B(i,1) = sumB_Y;
        centroid_B(i,2) = sumB_Z;
    }
    
    //translate points to their centroids
    AA= tempA- centroid_A;
    BB= tempB- centroid_B;

    //rotation matrix
    MatrixXd H= (AA.transpose())*(BB);

    JacobiSVD<Eigen::MatrixXd> svd(H,ComputeThinU | ComputeThinV );
    MatrixXd U= svd.matrixU();
    MatrixXd Vt= svd.matrixV();
    MatrixXd S= svd.singularValues();

    MatrixXd R = Vt*U.transpose();
    
    //special reflection case
    if(R.determinant()< 0)
    {
        for(int i= 0; i< Vt.cols(); i++)
        {
             Vt(i, 2) = (-1) * Vt(i, 2);
        }
        R = Vt* U.transpose();
    }

    //translation
    MatrixXd t = tempBB.transpose() - ((R)*(tempAA.transpose()));

    //homogeneous transformation
    MatrixXd T;
    T.setIdentity(4, 4);

    for(int i= 0; i< 3;i++)
    {
        for(int j= 0; j< 3; j++)
        {  
            T(i, j)= R(i, j);
        }
        double temp = t(i,0);
        T(i,3) = temp ;
    }
    *outputT = T;
}

/*The Iterative Closet Point method
    Input:
        slamTrackAll: N*4 matrix of source 3D points
        gpsTrackAll: N*4 matrix of destination 3D points
        maxIterations: exit algorithm after maxIterations
        tolerance: convergence criteria
    Output:
        outT: final homogeneous transformation
        outD: Euclidean distances (errors) of the nearest neighbor
*/
void trackICP(MatrixXd inputA,MatrixXd inputB,MatrixXd *outT,MatrixXd *outD)
{
    int iNumStart = 0;
    int iNumEnd = inputA.rows();
    int numPointICP = iNumEnd - iNumStart;

    //get slamCoordAll's rows and cols
    int slamTrackRows = inputA.rows() ;
    int slamTrackCols = inputA.cols() ;

    //get gpsCoordAll's rows and cols
    int gpsTrackRows = inputB.rows() ;
    int gpsTrackCols = inputB.cols() ;

    //N*3 matrix of source 3D points
    MatrixXd src;
    //N*3 matrix of destination 3D point
    MatrixXd dst;

    //N*1 Euclidean distances (errors) of the nearest neighbor
    MatrixXd distances;
    MatrixXd indices;

    MatrixXd _T;

    //src:N*4,dst:N*4，use "1" to fill the matrix
    src.setOnes(slamTrackRows,slamTrackCols);
    dst.setOnes(gpsTrackRows,gpsTrackCols);
    _T.setOnes(4,4);
    
    //get the slam track 
    for(int i=0;i<slamTrackRows;i++)
    {
       for(int j=0;j<slamTrackCols-1;j++)
       {
           src(i,j) = inputA(i,j);	
       }
    }

    //get the GPS track
    for(int i=0;i<gpsTrackRows;i++)
    {
       for(int j=0;j<gpsTrackCols-1;j++)
       {
           dst(i,j) = inputB(i,j);
       }
    }
    
    int maxIterations = 500;

    double prevError = 0;
    
    for(int i = 0;i<maxIterations;i++)
    {
        //find the nearest neighbours between the current source and destination points
        nearestNeighbor(src,dst,&distances,&indices);

        //compute the transformation between the current source and nearest destination points
        if(isAddWeight)
        {
            BFTWithWeight(src,dst,&_T);
        }
        else
        {
            bestFitTransform(src,dst,&_T);
        }
        
        //update the current source
        src =  src * _T.transpose();
        
        //check error
        double meanError = 0.0;
        
        for(int j=0;j<distances.rows();j++)
        {
            meanError += distances(j,0);
        }
        meanError = meanError/(distances.rows());
        
        if(abs(prevError-meanError)<0.003)
        {     
            break;
        }
        prevError = meanError;
    }
    
    //final homogeneous transformation
    MatrixXd tempTransformT;

    //calculate final transformation
    if(isAddWeight)
    {
        BFTWithWeight(inputA,src,&tempTransformT);
    }
    else
    {
        bestFitTransform(inputA,src,&tempTransformT);
    }

    *outT = tempTransformT;
    *outD = distances;
}

/*Regress the GPS track by the rotated slam track
    Input:
        gpsCoordInput: original GPS track
        slamRotatedCoordInput: ICP's rotated slam track
        numPoint: slamRotatedCoordInput's number
    Output:
        gpsReCoord: the regressed GPS points with the minimum variance of rotated slam track
        gpsReCoordAll: the whole regressed GPS track
        gpsReAverCoordAll: the average of whole regressed GPS track and rotated slam track
*/
void regressGpsWithSlamTrack(MatrixXd gpsCoordInput,MatrixXd slamRotatedCoordInput,int numPointInput,MatrixXd *gpsReCoord,MatrixXd *gpsReCoordAll,MatrixXd *gpsReAverCoordAll)
{ 
    int iNum = 0;
    int cNum = 0;
    int iCoord = 0;

    double variance;
    double averageX;
    double averageY;
    double minVar;
    double disX;
    double disY;

    MatrixXd tempGpsReCoord ;
    MatrixXd tempGpsReCoordAll;
    MatrixXd tempGpsReAverCoordAll;
    MatrixXd addToTempGpsReCoord;

    //regression  
    tempGpsReCoord.setOnes(numPointInput,2);
    tempGpsReCoordAll.setOnes(numPointInput,2);
    tempGpsReAverCoordAll.setOnes(numPointInput,2);
    addToTempGpsReCoord.setOnes(numPointInput+2,2);

    while(iNum < numPointInput)
    {
        variance = 0.0;
        averageX = 0.0;
        averageY = 0.0;
        iCoord = 0;
        
        while(iCoord<numPointInput)
        {
            disX = slamRotatedCoordInput(iCoord,0) - slamRotatedCoordInput(iNum,0);
            disY = slamRotatedCoordInput(iCoord,1) - slamRotatedCoordInput(iNum,1);
        
            //for Mininum Variance calculation
            tempGpsReCoord(iCoord,0) = gpsCoordInput(iCoord,0) - disX - gpsCoordInput(iNum,0);
            tempGpsReCoord(iCoord,0) = gpsCoordInput(iCoord,1) - disY - gpsCoordInput(iNum,1);
            variance += pow((tempGpsReCoord(iCoord,0) * tempGpsReCoord(iCoord,0)+ tempGpsReCoord(iCoord,1) * tempGpsReCoord(iCoord,1)),0.5);
            
            //regressed GPS dots dopend on iNum Slam Track dot
            averageX += gpsCoordInput(iCoord,0) - disX;
            averageY += gpsCoordInput(iCoord,1) - disY;
            iCoord++;
        }
        
        tempGpsReCoordAll(iNum,0) = averageX/numPointInput;
        tempGpsReCoordAll(iNum,1) = averageY/numPointInput;

        tempGpsReAverCoordAll(iNum,0) = (tempGpsReCoordAll(iNum,0) + slamRotatedCoordInput(iNum,0))/2.0;
        tempGpsReAverCoordAll(iNum,1) = (tempGpsReCoordAll(iNum,1) + slamRotatedCoordInput(iNum,1))/2.0; 
        
        if(iNum == 0)
        {
            minVar = variance;
        }
        
        if(variance < minVar)
        {
            minVar = variance;
            cNum = iNum;
        }
        iNum++;
    }

    //for Minimum Variance calculation-average coordinate calculation
    averageX = 0.0;
    averageY = 0.0;
    iCoord = 0;
    while(iCoord < numPointInput)
    {
        disX = slamRotatedCoordInput(iCoord,0) - slamRotatedCoordInput(cNum,0);
        disY = slamRotatedCoordInput(iCoord,1) - slamRotatedCoordInput(cNum,1);
        
        tempGpsReCoord(iCoord,0) = gpsCoordInput(iCoord,0) - disX;
        tempGpsReCoord(iCoord,1) = gpsCoordInput(iCoord,1) - disY;
        
        averageX += tempGpsReCoord(iCoord,0);
        averageY += tempGpsReCoord(iCoord,1);
        iCoord++;
    }
   
    averageX/=numPointInput;
    averageY/=numPointInput;
    
    //vstack 
    for(int i=0;i<tempGpsReCoord.rows();i++)
    {
        for(int j=0;j<2;j++)
        {
             addToTempGpsReCoord(i,j) = tempGpsReCoord(i,j);
        }
    }
    
    for(int i=0;i<2;i++)
    {
        addToTempGpsReCoord(tempGpsReCoord.rows(),i) = slamRotatedCoordInput(cNum,i);  
    }
    
    addToTempGpsReCoord(((tempGpsReCoord.rows())+1),0) = averageX;
    addToTempGpsReCoord(((tempGpsReCoord.rows())+1),1) = averageY;
    
    *gpsReCoord = addToTempGpsReCoord;
    *gpsReCoordAll = tempGpsReCoordAll;
    *gpsReAverCoordAll = tempGpsReAverCoordAll;
}

/*Output the ICPTrack to txt*/
void outPutXYZT(string inputFile)
{
    ofstream doneICPTrackWrite;
    doneICPTrackWrite.open(inputFile.c_str(),ios::out);

    for(int i=0;i<gpsReAverCoordAll.rows();i++)
    {
        double xTemp = gpsReAverCoordAll(i,0) + x0GPS;
        double yTemp = gpsReAverCoordAll(i,1) + y0GPS;
        doneICPTrackWrite.precision(IMDP);
        if(isAddWeight==true)
        {
	    doneICPTrackWrite<<xTemp<<" "<<yTemp<<" "<<zTemp[i]<<" "<<timeTemp[i]<<" "<<arrayWCTemp[i]<<endl;
        }
        else
        {
            doneICPTrackWrite<<xTemp<<" "<<yTemp<<" "<<zTemp[i]<<" "<<timeTemp[i]<<endl;
        }
        
    }
    doneICPTrackWrite.close();
}

/*Output the doneCutOffICPTrack to txt*/
void outPutWithCutOff(string inputFile)
{
    ofstream coICPTrackWrite;
    coICPTrackWrite.open(inputFile.c_str(),ios::out);

    for(int i=0;i<slamRotatedCoord.rows();i++)
    {
	    coICPTrackWrite.precision(IMDP);
        coICPTrackWrite<<slamRotatedCoord(i,0)<<" "<<slamRotatedCoord(i,1)<<" "<<slamRotatedCoord(i,2)<<" "<<gpsTimeTemp[i]<<endl;
    }
}

/*Cut off the slamTrack*/
void doCutOff()
{
    for(int i=0;i<numPoint;i++)
    {
	if(timeTemp[i]==gpsTimeTemp[0])
        {
            breakPointNum = i;
            break;
        }
    }
    
    int breakNum = slamCoordAll.rows() - breakPointNum;    

    if(breakNum>=gpsCoordAll.rows())
    {
        aCoord.setOnes(gpsCoordAll.rows(),4);
        bCoord.setOnes(gpsCoordAll.rows(),4);    
        
        for(int i=0;i<aCoord.rows();i++)
        {
            for(int j=0;j<aCoord.cols()-1;j++)
            {
                aCoord(i,j) = slamCoordAll(breakPointNum+i,j);
                bCoord(i,j) = gpsCoordAll(i,j);
            }
        }
    }
    else
    {
        aCoord.setOnes(breakNum,4);
        bCoord.setOnes(breakNum,4);
        
        for(int i=0;i<aCoord.rows();i++)
        {
            for(int j=0;j<aCoord.cols()-1;j++)
            {
                 aCoord(i,j) = slamCoordAll(breakPointNum+i,j);
                 bCoord(i,j) = gpsCoordAll(i,j);
            } 
        }
    }
}

/*Get the rotation of slamTrack*/
void getSlamRotatedCoord(MatrixXd inputMatrix)
{
    //N*3
    MatrixXd tempSlamCoordAll;
    MatrixXd rTransformT;
    MatrixXd tTransformT;
    
    tempSlamCoordAll.setOnes(inputMatrix.rows(),3);
    rTransformT.setOnes(3,3);
    tTransformT.setOnes(inputMatrix.rows(),3);
    
    //Get t from transformT
    for(int i=0;i<inputMatrix.rows();i++)
    {
        for(int j=0;j<3;j++)
        {
            tempSlamCoordAll(i,j) = inputMatrix(i,j);
            tTransformT(i,j) = transformT(j,3);
        }
    }

    //Get r from transformT
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
            rTransformT(i,j) = transformT(i,j);
        }
    }
    
    //Get the rotation of slam track
    slamRotatedCoord = tempSlamCoordAll*(rTransformT.transpose()) + tTransformT;
}

/*Run cutOffICP*/
void cutOffICPRun()
{
    cout<<"start Cutoff ICP"<<endl;
    trackICP(bCoord,aCoord,&transformT,&distancesD);
    cout<<"end Cutoff ICP"<<endl;
    
    getSlamRotatedCoord(gpsCoordAll);
    cout<<"rotated over"<<endl;
    

    outPutWithCutOff(fileDoneICPTrack);
    cout<<"data output over"<<endl;
}

/*Run ICP*/
void ICPRun()
{
    //Do the iterative closet points algorithm
    cout << "start ICP " << endl;
    trackICP(slamCoordAll,gpsCoordAll,&transformT, &distancesD);
    cout << "end ICP " << endl;
    
    getSlamRotatedCoord(slamCoordAll);  
    
    //Regression Calculation
    regressGpsWithSlamTrack(gpsCoordAll,slamRotatedCoord,numPoint,&gpsReCoord,&gpsReCoordAll,&gpsReAverCoordAll);

    outPutXYZT(fileDoneICPTrack);
}

int main(int argc, char** argv)
{
    if(argc!=8)
    {
        printf("parameter error\n");
        return 0;
    }
    
    //Define variable name to save loam_track path
    fileSLAMTrack = argv[1];
    //Define variable name to save GPS_track path
    fileGPSTrack = argv[2];
    //Define variable name ,if '==1' to run ICP_with_weightCoe
    fileisWeightCoe = argv[5];
    //Define variable name to save ICP_track_result path
    fileDoneICPTrack = argv[4];
    
    //According to inputing to adjust to do ICP_with_weight
    if(fileisWeightCoe=="1")
    {
       //Define variable name to save Weight_Coe path
       fileWeightCoe = argv[3];
       isAddWeight = true;
    }
    
    cout << "slam track is: " << fileSLAMTrack << endl;
    cout << "gps track is: " << fileGPSTrack << endl;

    //Read slam track and GPS track
    slamTrackRead(fileSLAMTrack);
    gpsTrackRead(fileGPSTrack);

    //Read value of weight from fileWeightCoe
    if(isAddWeight == true)
    {
        weightCoeRead(fileWeightCoe);
    }
    
    /*According to whether the same time to determine whether next step.
      we use the strategy, because running times of LOAM are too long, 
      the slam track is not very accurate, we have to run loam with short 
      distance, and use the track of cutting off to do ICP, to make the 
      result smoother
    */
    if(timeTemp[0]!=gpsTimeTemp[0])
    {	
        isCutOffICP = true;
        //if here, cutting off 
        doCutOff();
        //use the track of loam with short distance to ICP
        cutOffICPRun();
    }
    else
    {
        //if here,run ICP, to do ICP_with_weightCoe or not
        ICPRun();
    }

    delete[] zTemp;
    delete[] timeTemp;
    delete[] gpsTimeTemp;
    zTemp= NULL;
    timeTemp= NULL;
    gpsTimeTemp= NULL;
    if(isAddWeight == true)
    {
        delete[] arrayWCTemp;
        arrayWCTemp= NULL;
    }
        
    return 0;
}
