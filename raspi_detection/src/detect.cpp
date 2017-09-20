//============================================================================
// Name        : detect.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
// STD
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <unistd.h>
#include <algorithm>

// CV
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
using namespace std;

#include <sys/resource.h>

#include <yaml-cpp/yaml.h>

bool allowed=true;
void sig_stop(int a)
{
	allowed=false;
}

struct myclass {
    bool operator() (RotatedRect pt1, RotatedRect pt2) { return (pt1.center.y > pt2.center.y);}
} comparaison_ligne;

bool is_line(vector<Point>  contour,Mat image,RotatedRect & res){
	Rect Bonding_rect=boundingRect(contour);

	Vec4f line;
	fitLine(contour,line,CV_DIST_L2,0,0.01,0.01);
	double angle=atan2(line.val[1],line.val[0])/M_PI*180;
	if(angle<0)angle+=180;

	if(Bonding_rect.height>25){
		res=RotatedRect(Point2f(Bonding_rect.x+Bonding_rect.width/2,Bonding_rect.y+Bonding_rect.height/2),
				        Size2f(Bonding_rect.width,Bonding_rect.height),
				        angle);
		return true;
	}
	else return false;
}


int main(int argc,char **argv) {
	char* read_name;
	char* record_name=NULL;
	bool pause=false;
	if(argc>=2){
		read_name=argv[1];
	}
	if(argc>=3){
		record_name=argv[2];
	}

	VideoCapture cap(read_name); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
    Mat edges,frame;
    cap.read(frame);
	VideoWriter rec;
	if(record_name!=NULL){
		rec.open(record_name,CV_FOURCC('M','J','P','G'),
			            30.,Size(frame.size[1],frame.size[0]));
		if(!rec.isOpened())  // check if we succeeded
		        return -1;
	}

    namedWindow("frame",CV_WINDOW_AUTOSIZE);
    namedWindow("edges",CV_WINDOW_AUTOSIZE);
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );

    int size=2;
    int sigma=150;
    int minCanny=120;
    int maxCanny=200;
    int ferm=0;
    createTrackbar( "blur:size  (2*x+1)", "edges", &size, 5);
    createTrackbar( "blur:sigma	(x/100)", "edges", &sigma, 1000);

    createTrackbar( "canny:min	       ", "edges", &minCanny, 500);
    createTrackbar( "canny:max	       ", "edges", &maxCanny, 1000);
    createTrackbar( "ferm:nb it	       ", "edges", &ferm, 10);

    while(cap.isOpened())
    {
        if(!pause){
        	if(!cap.read(frame))// get a new frame from camera
        		break;
        }
        //traitement brut (filtrage)
        cvtColor(frame, edges, CV_BGR2GRAY);
        GaussianBlur(edges, edges, Size(size*2+1,size*2+1), sigma, sigma);
        Canny(edges, edges, minCanny, maxCanny, 3);

        Mat element_ferm = getStructuringElement( MORPH_CROSS,
                                              Size( 2*ferm + 1, 2*ferm+1 ),
                                              Point( ferm, ferm ) );
        dilate(edges,edges,element_ferm);
		erode( edges,edges,element_ferm);

        //"detection" à proprement parler
        vector<vector<Point> > contours;
        vector<RotatedRect>line_filtred;
        vector<Vec4i> hierarchy;
        findContours( edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
        for( int i = 0; i< contours.size(); i++ )
		{
        approxPolyDP(Mat(contours[i]), contours[i], 3, true);//diminue le nb de points inutiles
		}

        Mat drawing = Mat::zeros( edges.size(), CV_8UC3 );
        RotatedRect  test;
        for( int i = 0; i< contours.size(); i++ )
		{
        	if(is_line(contours[i],frame,test)){
        	Scalar color ;
			color= Scalar(rand() % 255, rand() % 255, rand() % 255 );
        	drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
        	line_filtred.push_back(test);
        	}
		}
        //les plus en avant sont les plus utiles
        sort(line_filtred.begin(), line_filtred.end(), comparaison_ligne);

        for( int i = 0; i< line_filtred.size(); i++ ){
			cout<<line_filtred[i].center.x<<":"<<line_filtred[i].center.y<<"="<<line_filtred[i].angle<<endl;
		}

        line_filtred.resize((line_filtred.size()+1)/2);//on vire les lointains


        //post_traitement : maintenant qu'on a une information pas trop mal on l'exploite!
        Mat result;
        frame.copyTo(result);
        if( line_filtred.size()>0){
			Point2f centre_line(0,0);double angle_line=0;
			for( int i = 0; i< line_filtred.size(); i++ ){
				centre_line.x+=line_filtred[i].center.x;
				centre_line.y+=line_filtred[i].center.y;
				angle_line+=line_filtred[i].angle;
			}
			centre_line.x/= line_filtred.size();
			centre_line.y/= line_filtred.size();
			angle_line   /= line_filtred.size();
			cout<<centre_line.x<<":"<<centre_line.y<<"="<<angle_line<<endl;
			//TODO faire une vrai reprojection Cam => sol
			Point pt_dir1=Point((int)round(centre_line.x + 100 * cos(angle_line*M_PI/180) ),
		                        (int)round(centre_line.y + 100 * sin(angle_line*M_PI/180) ));
			Point pt_dir2=Point((int)round(centre_line.x - 100 * cos(angle_line*M_PI/180) ),
		                        (int)round(centre_line.y - 100 * sin(angle_line*M_PI/180) ));

			line(result,pt_dir1,pt_dir2,Scalar(0,256,0),5);

        }

        cout<<"------------------"<<contours.size()<<":"<<line_filtred.size()<<endl;
        imshow("frame", result);
        imshow("edges", edges);
        imshow( "Contours", drawing );

        if(record_name!=NULL)
			rec.write(result);

        char key=waitKey(30);
        if(key=='q' || key==27) {
        	break;
        }
        else if(key==' '){
        	pause^=true;
        }

    }
    cout<<"size:"<<size<<endl;
    cout<<"sigma:"<<sigma<<endl;
    cout<<"minCanny:"<<minCanny<<endl;
    cout<<"maxCanny:"<<maxCanny<<endl;

    cap.release();
    return 0;

}
