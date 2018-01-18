#pragma once


#include "Communicator.h"
using namespace COMMUNICATOR;
#include "DBController.h"
using namespace DBCONTROLLER;
#include "DataControler.h"
using namespace DATACONTROLER;
#include "DAQControler.h"
using DAQCONTROLER::CDAQControler;
#include "Analysis.h"
using namespace ANALYSISSPACE;

//#include "const.h"

class CtheApp
{
public:
	CtheApp(void);
	~CtheApp(void);

public:
	CCommunicator* m_pCommunicator;
	CDBController* m_pDBC;
	CDataControler* m_pDataC;
	CDAQControler* m_pDAQC;
	CAnalysis* m_pAnalysis;
	DWORD m_dwMainThreadID;


};

inline	bool CheckFolderExist(const string& strInPath)
{//ע�⣬�����"\\",return false
	string strPath = strInPath;
	if ( strInPath.at(strPath.length()-1) == '\\' )
	{
		strPath.erase(strPath.length()-1,1);
	}

	WIN32_FIND_DATAA wfd;
	bool bRet = false;
	HANDLE hFind = FindFirstFileA(strPath.c_str(),&wfd);
	if ( (hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		bRet = true;
	}
	FindClose(hFind);
	return bRet;
}

inline bool CheckFileExist(const string& strFileFullPath)
{
	WIN32_FIND_DATAA wfd;
	bool bRet = false;
	HANDLE hFind = FindFirstFileA(strFileFullPath.c_str(),&wfd);
	//if ( (hFind != INVALID_HANDLE_VALUE) &&(wfd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) )
	if ( (hFind != INVALID_HANDLE_VALUE) )
	{
		bRet = true;
	}
	FindClose(hFind);
	return bRet;
}

//��ȡ���е��ļ���  
void GetAllFiles( string path, vector<string>& files, string format )    
{    
	long   hFile   =   0;    
	//�ļ���Ϣ    
	struct _finddata_t fileinfo;//�����洢�ļ���Ϣ�Ľṹ��   
	int len = format.length();
	string p,temp;    
	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)  //��һ�β���  
	{
		do    
		{     
			if((fileinfo.attrib &  _A_SUBDIR))  //������ҵ������ļ���  
			{    
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)  //�����ļ��в���  
				{  
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
					GetAllFiles( p.assign(path).append("\\").append(fileinfo.name), files,format );   
				}  
			}    
			else //������ҵ��Ĳ������ļ���   
			{    
				//files.push_back(p.assign(fileinfo.name) );  //���ļ�·�����棬Ҳ����ֻ�����ļ���:    p.assign(path).append("\\").append(fileinfo.name)  
				temp = fileinfo.name;
				//�ж��ַ����Ƿ���format��ʽ��β
				if(temp.length()>len && temp.compare(temp.length()-len,len,format)==0)
					files.push_back(p.assign(path).append("\\").append(fileinfo.name) ); 
			}   

		}while(_findnext(hFile, &fileinfo)  == 0);    

		_findclose(hFile); //��������  
	}   

}   