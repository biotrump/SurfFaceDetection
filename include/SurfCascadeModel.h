//declaration of SurfCascadeModel
//autho:Guotian Xie
//version:1.0

#ifndef _SURF_CASCADE_MODEL_H_
#define _SURF_CASCADE_MODEL_H_

#include <vector>
//#include <opencv\cv.h>
#include "opencv2\core\core_c.h"
#include "DebugConfig.h"
#include "opencv2\core\core.hpp"

#include "LogRegression.h"
#include "Global.h"

using namespace cv;
using namespace std;

class CascadeStage
{
public:
	double Predict(const Mat &_sumImg, float _scale, double &_score);

	bool LoadStage(FileNode *node);
	bool LoadStage(FileStorage *file);

	bool GetFeatureList(vector<Rect > &rectList);
	inline float getThreshold(void) {return threshold;}
	bool getWeak(int idx, LogRegression &w);
	int getWeak(vector<LogRegression> &w);

protected:
	void Clear(){weak.clear();}
	vector<LogRegression> weak;
	float threshold;
};

class SurfCascadeModel
{
public:	//API extern
	bool LoadSurfCascadeModel(char *_fileName);
	bool LoadSurfCascadeModel(FileStorage *_file);
	bool LoadSurfCascadeModelByStages(vector<char *> _stageFiles);

	int JudgeWindow(Mat &_sumImg, float _sacle, double &_score);

	inline int stages_size(void) {return stages.size();}
	bool GetStage(int idx, CascadeStage &stage );
	int GetStages(vector<CascadeStage>  &_stages);
	bool GetFeature(int idx, vector<Rect> &rectList );
	//int GetFeatureList(vector<vector<Rect> *> &list);

protected:
	//API intern
	void Clear(){ stages.clear(); }
	//data
	vector<CascadeStage>  stages;
	//vector<vector<Rect> *> featureList;
};

#endif
