#include "SurfCascadeModel.h"

bool CascadeStage::LoadStage(FileNode *node)
{
	Clear();

	if(node == NULL || (*node).empty() || (*node)["Threshold"].empty() 
		|| (*node)["NumWeaks"].empty() || (*node)["WeakSequence"].empty())
	{
		CV_Error(CV_StsParseError, "Stage classifier format wrong in the model file");
		return false;
	}
	(*node)["Threshold"] >> threshold;
	
	int numWeaks = 0;
	(*node)["NumWeaks"] >> numWeaks;
	weak.reserve(numWeaks);

	FileNode weaksNode = (*node)["WeakSequence"];
	int idx = 0;
	for(FileNodeIterator iter = weaksNode.begin(); iter != weaksNode.end(); iter++, idx++)
	{
		LogRegression ww;
		if(!ww.LoadWeak(&(*iter)))
			return false;

		weak.push_back(ww);
	}

	return true;
}

bool CascadeStage::LoadStage(FileStorage *file)
{
	if( file == NULL || !(*file).isOpened() )
	{
		CV_Error(CV_StsNullPtr, "File is NULL or is not opened.");
		return false;
	}

	FileNode firstNode  = (*file).root();

	if( firstNode.empty() )
	{
		CV_Error(CV_StsParseError, "Wrong format of model file.");
		return false;
	}

	return LoadStage(&firstNode);
}

double CascadeStage::Predict(const Mat &_sumImg, float _scale, double &_score)
{
	//double sum = 0.0;
	_score = 0.0;

	for(int idx=0; idx<weak.size(); idx++)
	{
		_score += weak[idx].Predict(_sumImg, _scale);
	}

	//if(!_sumRes)
		//sum = (sum<threshold? 0:1);

	return (_score < threshold)? 0:1;
}

bool CascadeStage::GetFeatureList(vector<Rect> &rectList)
{
	for(int i=0; i<weak.size(); i++)
	{
		Rect r = weak[i].GetFeature();
		rectList.push_back(r);
	}
	return !!weak.size();
}

bool CascadeStage::getWeak(int idx, LogRegression &w)
{
	if(idx < weak.size())
	{
		w = weak[idx];
		return true;
	}
	return false; 
}

int CascadeStage::getWeak(vector<LogRegression> &w)
{
	w = weak;
	return weak.size();
}

bool SurfCascadeModel::LoadSurfCascadeModel(char *_fileName)
{
	FileStorage file(_fileName,FileStorage::READ);

	return LoadSurfCascadeModel(&file);

}

bool SurfCascadeModel::LoadSurfCascadeModel(FileStorage *_file)
{
	Clear();
	if( _file == NULL || !(*_file).isOpened() )
		return false;

	int numStages = 0;
	if( (*_file)["NumStages"].empty() || (*_file)["WeakSequence"].empty() )
	{
		CV_Error(CV_StsParseError, "Wrong format of model file.");
		return false;
	}

	(*_file)["NumStages"] >> numStages;
	stages.reserve(numStages);

	FileNode weakSeq = (*_file)["WeakSequence"];
	int idx = 0;
	for(FileNodeIterator weak = weakSeq.begin(); weak != weakSeq.end(); weak++, idx++)
	{
		CascadeStage stage;

		if(!stage.LoadStage(&(*weak)))
		{
			return false;
		}
		stages.push_back(stage);
	}

	return true;
}

bool SurfCascadeModel::LoadSurfCascadeModelByStages(vector<char *> _stageFiles)
{
	Clear();

	for(int idx=0; idx<_stageFiles.size(); idx++)
	{
		FileStorage file(_stageFiles[idx], FileStorage::READ);
		CascadeStage stage;
		if(!stage.LoadStage(&file))
			return false;

		stages.push_back(stage);

	}
	return true;
}

int SurfCascadeModel::JudgeWindow(Mat &_sumImg, float _scale, double &_score)
{
	_score = 0;
	for(int idx = 0; idx<stages.size(); idx++)
	{
		if( stages[idx].Predict(_sumImg, _scale, _score) == 0.0 )
			return 0;
	}
	return 1;
}

bool SurfCascadeModel::GetFeature(int idx, vector<Rect> &rectList )
{
	if(!rectList.empty())
		return true;
	if( (idx < stages.size()) && 
		stages[idx].GetFeatureList(rectList) )
		return true;
	return false;
}

bool SurfCascadeModel::GetStage(int idx, CascadeStage &stage )
{
	if(stages.size() && (idx < stages.size()))
	{
		stage = stages[idx];
		return true;
	}
	return false;
}

int SurfCascadeModel::GetStages(vector<CascadeStage>  &_stages)
{
	if(stages.size())
	{
		_stages = stages;
	}
	return stages.size();
}
/*
int SurfCascadeModel::GetFeatureList(vector<vector<Rect> *> &list) 
{
	if(!list.empty())
		return list.size();
	for(int idx = 0; idx<stages.size(); idx++)
	{
		vector<Rect> r;
		GetFeature(idx,r);
		list.push_back(r);
	}

	return stages.size();
}
*/
