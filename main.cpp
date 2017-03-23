#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/viz/vizcore.hpp"
#include "opencv2/video/tracking.hpp"

using namespace std;
using namespace cv;

string images_path="/home/opencv_data/calib_images/";
string storage_path="/home/opencv_data/matrices/";
bool distortion_test=false;
bool calibration_test=false;

int main(){

//-----------------------------------------------------------------------------------------
//----------------------------------- images loading --------------------------------------
//-----------------------------------------------------------------------------------------
vector<cv::Mat> input;
int iter=1;

while(1)
{
    std::ostringstream buff;
    buff<<iter;
    string string_iter=buff.str();

    Mat image = imread(images_path+string_iter+".jpg",CV_LOAD_IMAGE_GRAYSCALE);
    if(image.empty()) break;
    input.push_back(image);
    iter=iter+1;
}

int N=input.size();

//-----------------------------------------------------------------------------------------
//-------------------------- cheesboard corners detection ---------------------------------
//-----------------------------------------------------------------------------------------
Mat out;
vector<Point2f> corners;
vector<vector<Point2f> > image_points;
Scalar color[5]={Scalar(255,0,255),Scalar(255,180,0),Scalar(0,255,0),Scalar(0,255,255),Scalar(0,0,255)};

for(int i=0;i<N;i++)
{
    bool pattern_found=findChessboardCorners(input[i],Size(7,5),corners,CALIB_CB_ADAPTIVE_THRESH|CALIB_CB_NORMALIZE_IMAGE);

    if(pattern_found)
    {
        cvtColor(input[i],out,CV_GRAY2RGB);

        for(int j=0;j<5;j++)
        for(int i=0;i<7;i++)
        {
            circle(out,Point(corners[i+j*7].x,corners[i+j*7].y),6,color[j],3);
            if(i<6) line(out,Point(corners[i+j*7].x,corners[i+j*7].y),Point(corners[i+1+j*7].x,corners[i+1+j*7].y),color[j],3);
        }
    }
    image_points.push_back(corners);

    imshow("output",out);
    moveWindow("output",1,1);
    char key=waitKey();if(key==27)break;
}

N=image_points.size();

//-----------------------------------------------------------------------------------------
//------------------------------- camera calibration --------------------------------------
//-----------------------------------------------------------------------------------------
Mat intrinsic_matrix;
Mat distortion_coeffs;
vector<vector<Point3f> > object_points;

object_points.resize(N);
for(int i=0;i<N;i++)
{
    object_points[i].resize(35);
}

for(int k=0;k<N;k++)  //N images
for(int j=0;j<5;j++)  //5 rows
for(int i=0;i<7;i++)  //7 cols
{
    object_points[k][i+7*j].x=30*i;
    object_points[k][i+7*j].y=30*j;
    object_points[k][i+7*j].z=0;
}

vector<cv::Mat> rvecs;
vector<cv::Mat> tvecs;
double calibration_error = calibrateCamera(object_points,image_points,input[0].size(),intrinsic_matrix,distortion_coeffs,rvecs,tvecs);

cout<<"calibration_error: "<<calibration_error<<endl;
cout<<"intrinsic_matrix: "<<endl<<intrinsic_matrix<<endl;
cout<<"distortion_coeffs:"<<endl<< distortion_coeffs<<endl;

//-----------------------------------------------------------------------------------------
//----------------------------------- martices storage ------------------------------------
//-----------------------------------------------------------------------------------------
FileStorage file(storage_path+"cam_calib.xml",cv::FileStorage::WRITE);
file<<"intrinsic_matrix"<<intrinsic_matrix<<"distortion_coeffs"<<distortion_coeffs;
file.release();

//-----------------------------------------------------------------------------------------
//------------------------------------ distortion test ------------------------------------
//-----------------------------------------------------------------------------------------
if(distortion_test)
{
    Mat image_undistorted;
    undistort(input[0],image_undistorted,intrinsic_matrix,distortion_coeffs);
    imshow("distortion_test",image_undistorted);
    moveWindow("distortion_test",1,1);
    waitKey();
}

//-----------------------------------------------------------------------------------------
//----------------------------------- calibration test ------------------------------------
//-----------------------------------------------------------------------------------------
if(calibration_test)
{
    Mat test;
    int image_number=1;
    input[image_number].copyTo(test);
    cvtColor(test,test,CV_GRAY2BGR);

    vector<Point3f> cube_points;
    cube_points.resize(4);
    cube_points[0]=Point3f(0,0,0);
    cube_points[1]=Point3f(120,0,0);
    cube_points[2]=Point3f(0,120,0);
    cube_points[3]=Point3f(0,0,-120);

    vector<Point2f> cube_image_points;
    projectPoints(cube_points,rvecs[image_number],tvecs[image_number],intrinsic_matrix,distortion_coeffs,cube_image_points);

    line(test,Point(cube_image_points[0].x,cube_image_points[0].y),Point(cube_image_points[1].x,cube_image_points[1].y),Scalar(0,0,255),9);
    line(test,Point(cube_image_points[0].x,cube_image_points[0].y),Point(cube_image_points[2].x,cube_image_points[2].y),Scalar(0,255,0),9);
    line(test,Point(cube_image_points[0].x,cube_image_points[0].y),Point(cube_image_points[3].x,cube_image_points[3].y),Scalar(255,0,0),9);

    imshow("calibration_test",test);
    moveWindow("calibration_test",1,1);
    waitKey();
}

cvDestroyAllWindows();
return 0;

}
