#include "DebugConfig.h"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc_c.h"
#include <iostream>
#include "SurfFaceDetection.h"
#include <time.h>
#include <fstream>
//#include <mat.h>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
#undef MY_DEBUG
#ifdef MY_DEBUG

	eg = engOpen("\0");
	if( !eg )
	{
		cout<<"open engine failed."<<endl;
		return -1;
	}

#endif
	vector<char *> stageFiles;

	for(int i=0; i<5; i++)
	{
		char *file = new char[_MAX_PATH];
		sprintf(file, "d:/repos/SURF/SurfFaceDetection/model/stage%d.xml", i);
		stageFiles.push_back(file);
	}

	SurfFaceDetection detection(stageFiles);

//#define VEDIO_TEST
#ifdef VEDIO_TEST	
	VideoCapture cap(0);
	if(!cap.isOpened())
		return -1;

	namedWindow("video",1);
	Mat frame;
	Mat grayImg;
	vector<Rect> myFace;
	while(true)
	{
		myFace.clear();
		cap >> frame;
		clock_t t1 = clock();
		cvtColor(frame, grayImg, CV_RGB2GRAY);
		detection.DetectMultiScale(grayImg,1.1,0.1,Size(40,40),myFace);
		for(int idx=0; idx<myFace.size(); idx++)
			rectangle(frame,myFace[idx],Scalar(0,255,0));

		clock_t t2 = clock();
		cout<<(t2-t1)/1000.0 << "\r";
		imshow("video",frame);
		waitKey(10);
	}

#endif
#define IMG_TEST
#ifdef IMG_TEST
	//Mat img = imread("D:/microsoft/MyProject/FaceDectorPerfTest/DataSet/2003/02/02/big/img_367.jpg",0);
	//Mat img = imread("../data/test1.jpg",0);
	Mat img;
	if(argc>=2)
		img = imread(argv[1], IMREAD_GRAYSCALE);
	else exit(1);
	//cout<<img;
	namedWindow("tmpShow",1);
	vector<Rect> faces;
	detection.DetectMultiScale(img,1.1,0.4,Size(40,40),faces);

	//system("pause");
	Mat colorImg;
	cvtColor(img,colorImg,CV_GRAY2RGB);
	for(int idx=0; idx<faces.size(); idx++)
		rectangle(colorImg,faces[idx],Scalar(0,255,0));	
	namedWindow("myShow",1);
	imshow("myShow",colorImg);

	waitKey(0);
#endif

#define FDDB_TEST
#ifdef FDDB_TEST
	ifstream read("D:/microsoft/MyProject/FaceDectorPerfTest/DataSet/myfolds/all.txt");
	if(!read.is_open())
		return -1;
	string line;

#define SHOW
#ifdef SHOW
	namedWindow("FDDB",1);
#endif
#ifdef TEXT
	ofstream detRes("../data/detResOnFDDB-1.2.txt");
	if( !detRes.is_open() )
		return -1;
	int count = 0;
	double time = 0;
#endif
	while(getline(read,line))
	{
		string name = "D:/microsoft/MyProject/FaceDectorPerfTest/DataSet/" + line+".jpg";
		Mat img;
		Mat colorImg;
		colorImg = imread(name, 1);
		cvtColor(colorImg,img,CV_RGB2GRAY);
		vector<Rect> faces;


#ifdef SHOW
		vector<double> scores;
		detection.DetectMultiScale(img,1.2,0.1,Size(40,40),faces,true, &scores);
		for(int idx=0; idx<faces.size(); idx++)
			rectangle(colorImg,faces[idx],Scalar(0,255,0));
		imshow("FDDB",colorImg);
		waitKey(500);
#endif
#ifdef TEXT
		count ++;
		
		vector<double> scores;
		clock_t t1 = clock();
		detection.DetectMultiScale(img,1.2,0.1,Size(40,40),faces,true,&scores);
		clock_t t2 = clock();
		double tt = (t2 - t1)/1000.0;
		cout << "iter: " << count << "\t" << tt << "\r";
		time += tt;
		detRes << line << endl;
		detRes << faces.size() << endl;
		for(int idx=0; idx<faces.size(); idx++)
		{
			//faces[idx].x = (int)std::max<double>(0,faces[idx].x - 0.25 * faces[idx].width);
			faces[idx].y = (int)std::max<double>(0,faces[idx].y - 0.3 * faces[idx].height);
			faces[idx].height = std::min((int)(faces[idx].height * 1.5),img.rows - faces[idx].y);

			//faces[idx].x = (int)std::max<double>(0,faces[idx].x - 0.15 * faces[idx].width);
			//faces[idx].width = std::min((int)(faces[idx].width*1.3),img.cols - faces[idx].x);
			detRes << faces[idx].x << " " << faces[idx].y << " " <<
				faces[idx].width << " " << faces[idx].height 
				<< " " << scores[idx] << endl;

		}
#endif
	}
#ifdef TEXT
	detRes.close();
	cout << endl;
	cout << "averge time per pic: " << time / count << endl;
#endif 
	read.close();
#endif
	return 0;
}