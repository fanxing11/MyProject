#include "main.h"
#include "Analysis.h"
#include "Filter.h"
#include <sstream>

#include <algorithm>

extern CtheApp* theApp;

namespace ANALYSISSPACE
{

	unsigned int WINAPI AnalysisThreadFunc(LPVOID lp)
	{
		CAnalysis* pAnalysis = (CAnalysis*)lp;

		//�½��ĵ�һ��Ҳ��Ҫ����
		PostThreadMessage(GetCurrentThreadId(),msg_ANA_ANALYSIS_BEGIN ,NULL,NULL);

		MSG msg;
		bool bRet = false;
		while(GetMessage(&msg,NULL,0,0))
		{
			if (msg_ANA_ANALYSIS_BEGIN == msg.message)
			{
				string strInfo;
				if( !(pAnalysis->_BeginaAnalysis(strInfo)) )
				{

					char* pBuf = NULL;
					int nLen = strInfo.length();
					pBuf = new char[nLen+1];
					memset(pBuf,0,nLen+1);
					memcpy(pBuf,strInfo.c_str(),nLen);
					if(!PostThreadMessage(theApp->m_dwMainThreadID,msg_ANA_ANALYSIS_STATE,NUM_THREE,(LPARAM)pBuf))
					{
						g_logger.TraceError("AnalysisThreadFunc - PostThreadMessage failed");
						delete[] pBuf;
						pBuf = NULL;
					}

				}
				else
				{
					pAnalysis->PostAnalysisStateMsg(NUM_TWO);
					//PostThreadMessage(theApp->m_dwMainThreadID,msg_ANA_ANALYSIS_STATE,NUM_TWO,NULL);
				}
			}
		}


		return 0;
	}

	CAnalysis::CAnalysis(void)
		:m_hAnalysisThread(NULL)
		,m_hFileReader(NULL)
		,m_dCarInitXAngle(0.0)
		,m_dCarInitYAngle(0.0)
		,m_dMaxHandBrakeForce(0.0)
		,m_dInitAccA(0.0)
		,m_dInitAccB(0.0)
		,m_dInitAccC(0.0)
		,m_dInitFootBrakeForce(0.0)
		,m_dInitHandBrakeForce(0.0)
	{
		g_logger.TraceInfo("CAnalysis::CAnalysis");
	}
	void CAnalysis::InitData()
	{
		m_vAnalysisData.clear();
		m_vAccelaration.clear();
		m_vVelocity.clear();
		m_vFootBrakeForce.clear();
		m_vPedalDistance.clear();
		m_stResult.MaxAccelaration = 0.0;
		m_stResult.BrakeDistance = 0.0;
		m_stResult.AverageVelocity = 0.0;
		m_stResult.GradientX = 0.0;
		m_stResult.GradientY = 0.0;
		m_stResult.PedalDistance = 0.0;
		m_stResult.MaxHandBrakeForce = 0.0;
		m_stResult.MaxFootBrakeForce = 0.0;
		m_dInitAccA = 0.0;
		m_dInitAccA = 0.0;
		m_dInitAccB = 0.0;
		m_dInitAccC = 0.0;
		m_dInitFootBrakeForce = 0.0;
		m_dInitHandBrakeForce = 0.0;

	}


	CAnalysis::~CAnalysis(void)
	{
		if( WAIT_OBJECT_0 == WaitForSingleObject(m_hAnalysisThread,1000) )
		{
			////normal
		}

	}

	void CAnalysis::BeginAnalysis(const string &strProjectPath)
	{
		m_strProjectPath = strProjectPath;
		if ( m_strProjectPath.at(m_strProjectPath.length()-1) != '\\' )
		{
			m_strProjectPath.append(1,'\\');
		}
		bool bGood = true;
		string strInfo;
		m_strConfigFile = m_strProjectPath+gc_strProjectParaINI_FileName;
		string strBinFile = m_strProjectPath + "*.bin";

		if ( !CheckFolderExist(m_strProjectPath))
		{
			bGood = false;
			strInfo = ("ProjectPath(");
			strInfo = strInfo+m_strProjectPath+string(") is not exist");
		}
		else if ( !CheckFileExist(m_strConfigFile))
		{
			bGood = false;
			strInfo = ("ProjectParaFile(");
			strInfo = strInfo+m_strConfigFile+string(") is not exist");
		}
		else if ( !CheckFileExist(strBinFile))
		{
			bGood = false;
			strInfo = ("DataFile(");
			strInfo = strInfo+ strBinFile +string(") is not exist");
		}

		if (!bGood)
		{
			g_logger.TraceWarning("CAnalysis::BeginAnalysis - %s",strInfo.c_str());

			char* pBuf = NULL;
			int nLen = strInfo.length();
			pBuf = new char[nLen+1];
			memset(pBuf,0,nLen+1);
			memcpy(pBuf,strInfo.c_str(),nLen);
			if(!PostThreadMessage(theApp->m_dwMainThreadID,msg_ANA_ANALYSIS_STATE,NUM_THREE,(LPARAM)pBuf))
			{
				g_logger.TraceError("CAnalysis::BeginAnalysis - PostThreadMessage failed");
				delete[] pBuf;
				pBuf = NULL;
			}

			return;
		}
		
		if (NULL == m_hAnalysisThread)
		{
			m_hAnalysisThread = (HANDLE)_beginthreadex(NULL, 0, AnalysisThreadFunc, (LPVOID)this, CREATE_SUSPENDED, NULL);  
			ResumeThread(m_hAnalysisThread);
		}
		else
		{
			PostThreadMessage(GetThreadId(m_hAnalysisThread),msg_ANA_ANALYSIS_BEGIN ,NULL,NULL);
		}
	}

	bool CAnalysis::_BeginaAnalysis(string &strErrInfo)
	{
		//get para from INI
		if(!ReadParaFromINI(strErrInfo))
		{
			return false;
		}

		//read and transform data
		if (!ReadDataFromFile(strErrInfo))
		{
			return false;
		}

		////send result to main thread msg_ANA_ANALYSIS_RESULT
		if( !AnalyseResult() )
		{
			return false;
		}

		return true;

	}

	bool CAnalysis::ReadParaFromINI(string &strErrInfo)
	{
		g_logger.TraceInfo("CAnalysis::ReadParaFromINI -in");

		//---------read car angle from ini------
		char stInitX[50]={0};  
		char stInitY[50]={0};  

		DWORD dwRet1,dwRet2;
		dwRet1 = GetPrivateProfileStringA(gc_strInitialCarAngle.c_str(), gc_strInitXAngle.c_str(), "", stInitX, 50, m_strConfigFile.c_str());  
		dwRet2 =GetPrivateProfileStringA(gc_strInitialCarAngle.c_str(), gc_strInitYAngle.c_str(), "", stInitY, 50, m_strConfigFile.c_str());  
		if (dwRet2 <= 0 || dwRet1 <= 0)
		{
			strErrInfo = "Get InitialX or InitialY in the INI file failed";
			g_logger.TraceError("CAnalysis::ReadParaFromINI - %s",strErrInfo.c_str() );
			return false;
		}
		std::stringstream stream;
		stream<<stInitX;
		stream>>m_dCarInitXAngle;
		stream.clear();
		stream<<stInitY;
		stream>>m_dCarInitYAngle;

		//---------read max hand brake force from ini------
		char stMaxHandBrakeForce[50]={0};  
		DWORD dwRet3;
		dwRet3 = GetPrivateProfileStringA(gc_strResult.c_str(), gc_strMaxHandBrakeForce.c_str(), "", stMaxHandBrakeForce, 50, m_strConfigFile.c_str());  
		if ( dwRet3 <= 0 )
		{
			strErrInfo = "Get MaxHandBrakeForce in the INI file failed";
			g_logger.TraceError("CAnalysis::ReadParaFromINI - %s",strErrInfo.c_str() );
			return false;
		}
		stream.clear();
		stream<<stMaxHandBrakeForce;
		stream>>m_dMaxHandBrakeForce;
		//------read Init Value from ini---------
		char stInitHandBrakeForce[50]={0};  
		char stInitFootBrakeForce[50]={0};  
		char stInitAccA[50]={0};  
		char stInitAccB[50]={0};  
		char stInitAccC[50]={0};  
		DWORD dwRet4,dwRet5,dwRet6,dwRet7,dwRet8;
		dwRet4 = GetPrivateProfileStringA(gc_strInitValue.c_str(), gc_strInitAccA.c_str(), "", stInitAccA, 50, m_strConfigFile.c_str());  
		dwRet5 = GetPrivateProfileStringA(gc_strInitValue.c_str(), gc_strInitAccB.c_str(), "", stInitAccA, 50, m_strConfigFile.c_str());  
		dwRet6 = GetPrivateProfileStringA(gc_strInitValue.c_str(), gc_strInitAccC.c_str(), "", stInitAccA, 50, m_strConfigFile.c_str());  
		dwRet7 = GetPrivateProfileStringA(gc_strInitValue.c_str(), gc_strInitHandBrakeForce.c_str(), "", stInitAccA, 50, m_strConfigFile.c_str());  
		dwRet8 = GetPrivateProfileStringA(gc_strInitValue.c_str(), gc_strInitFootBrakeForce.c_str(), "", stInitAccA, 50, m_strConfigFile.c_str());  
		if ( dwRet4 <= 0 || dwRet5 <= 0 || dwRet6 <= 0 || dwRet7 <= 0 || dwRet8 <= 0)
		{
			strErrInfo = "Get InitValue in the INI file failed";
			g_logger.TraceError("CAnalysis::ReadParaFromINI - %s",strErrInfo.c_str() );
			return false;
		}
		stream.clear();
		stream<<stInitAccA;
		stream>>m_dInitAccA;
		stream.clear();
		stream<<stInitAccB;
		stream>>m_dInitAccB;
		stream.clear();
		stream<<stInitAccC;
		stream>>m_dInitAccC;
		stream.clear();
		stream<<stInitHandBrakeForce;
		stream>>m_dInitHandBrakeForce;
		stream.clear();
		stream<<stInitFootBrakeForce;
		stream>>m_dInitFootBrakeForce;
		return true;
	}

	bool CAnalysis::ReadDataFromFile(string &strErrInfo)
	{
		try{

		g_logger.TraceInfo("CAnalysis::ReadDataFromFile -n");
		bool bRet = true;

		string strDataFileName;
		this->GetDataFile( strDataFileName );

		m_hFileReader = CreateFileA(
			strDataFileName.c_str(),
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN,
			NULL);

		if (m_hFileReader == INVALID_HANDLE_VALUE)
		{
			g_logger.TraceError("CAnalysis::ReadDataFromFile - %s", strErrInfo.c_str());
			bRet = false;
		}
		DWORD dwFileSize = GetFileSize(m_hFileReader,NULL); 

		double* buffer = (double*)VirtualAlloc(NULL, dwFileSize, MEM_COMMIT, PAGE_READWRITE);
		if (!buffer)
		{
			g_logger.TraceError("CAnalysis::ReadDataFromFile - Allocate buffer fail(error %d)\n",GetLastError());
			bRet = false;
		}

		double *tmpBuf = buffer;
		DWORD dwBytesRead = 0;
		DWORD dwBytesToRead = dwFileSize;//,tmpLen;
		do{ //ѭ�����ļ���ȷ�������������ļ�    
			static int nCount=0;
			++nCount;
			g_logger.TraceWarning("CAnalysis::ReadDataFromFile - begin read %d times",nCount);

			if(!ReadFile(m_hFileReader,tmpBuf,dwBytesToRead,&dwBytesRead,NULL))
			{
				g_logger.TraceWarning("CAnalysis::ReadDataFromFile - ReadFile failed:error=%d",GetLastError());
			}

			if (dwBytesRead == 0)
				break;

			dwBytesToRead -= dwBytesRead;
			tmpBuf += dwBytesRead;

		} while (dwBytesToRead > 0);

		if (theApp->m_pDataController->DAQIsWirelessType())
		{
			HandleDataW(buffer, DAQCONTROLER::channelCountW, dwFileSize, DAQCONTROLER::deltatW);
		}
		else
		{
			HandleData(buffer, DAQCONTROLER::channelCount, dwFileSize, DAQCONTROLER::deltat);
		}

		if (NULL != buffer)
		{
			VirtualFree(buffer, DAQCONTROLER::SingleSavingFileSize, MEM_RELEASE);
			buffer = NULL;
		}
		if (INVALID_HANDLE_VALUE != m_hFileReader)
		{
			CloseHandle(m_hFileReader);
			m_hFileReader = NULL;
		}

		if ( !bRet )
		{
			strErrInfo = "Read Data From File failed.";
		}

		return bRet;
		}
		catch(exception* e)
		{
			g_logger.TraceError("CAnalysis::ReadDataFromFile:%s",e->what());
		}

	}

	bool CAnalysis::GetDataFile(string& strFileName)
	{
		vector<string> files;
		string format = ".bin";  
		GetAllFiles(m_strProjectPath, files,format);
		int nFileNum = files.size();
		if (NUM_ONE != nFileNum)
		{
			g_logger.TraceWarning("CAnalysis::GetDataFile - there are more than 1 data File");
		}
		strFileName = files.at(0);
		return true;
	}

	void CAnalysis::HandleData(const double* pData, const int channelCount, const DWORD dwDataSize/*Byte*/, const double deltat)
	{
		g_logger.TraceInfo("CAnalysis::HandleData -in");
		this->PostAnalysisStateMsg();

		this->InitData();

		DWORD doubleNum = dwDataSize / (sizeof(double)) / channelCount;//����10ͨ��doubleֵ
		double dSumA = 0.0;
		double dCompoundA = 0.0;

		//v1.9 ʹ��һ������ļ��ٶȡ����ٽ��кϳɡ�
		//m_stResult.MaxAccelaration = sqrt((*(pData+7))*(*(pData+7))+(*(pData+8))*(*(pData+8))+(*(pData+9))*(*(pData+9)));
		m_stResult.MaxAccelaration = *(pData+7);
		double dSumVel=0.0;
		double dCurrentVel=0.0;
		double dCurrentDist=0.0;

		m_stResult.MaxFootBrakeForce = *(pData+0) - *(pData+1);
		m_stResult.GradientX = *(pData+2);
		m_stResult.GradientY = *(pData+3);
		m_stResult.PedalDistance = *(pData+6);

		ANALYSISRESULT stAnalysisInfo;
		for (DWORD i=0;i<doubleNum;++i)
		{
			//dCompoundA = sqrt((*(pData+(i*channelCount)+7))*(*(pData+(i*channelCount)+7))+(*(pData+(i*channelCount)+8))*(*(pData+(i*channelCount)+8))+(*(pData+(i*channelCount)+9))*(*(pData+(i*channelCount)+9)));
			dCompoundA = *(pData+(i*channelCount)+7);
			dSumA += dCompoundA;
			dCurrentVel = dSumA*deltat;
			dSumVel += dCurrentVel;
			dCurrentDist = dSumVel*deltat;

			stAnalysisInfo.MaxFootBrakeForce = *(pData+(i*channelCount)) - *(pData+(i*channelCount)+1);
			//stAnalysisInfo.MaxHandBrakeForce = *(pData+(i*channelCount)+4) - *(pData+(i*channelCount)+5);
			stAnalysisInfo.GradientX = *(pData+(i*channelCount)+2);
			stAnalysisInfo.GradientY = *(pData+(i*channelCount)+3);
			stAnalysisInfo.PedalDistance = *(pData+(i*channelCount)+6);

			if (m_stResult.MaxFootBrakeForce < stAnalysisInfo.MaxFootBrakeForce)
			{
				m_stResult.MaxFootBrakeForce = stAnalysisInfo.MaxFootBrakeForce;
			}
			//if (m_stResult.MaxHandBrakeForce< stStressInfo.MaxHandBrakeForce)
			//{
			//	m_stResult.MaxHandBrakeForce = stStressInfo.MaxHandBrakeForce;
			//}
			if (m_stResult.GradientX < stAnalysisInfo.GradientX)
			{
				m_stResult.GradientX = stAnalysisInfo.GradientX;
			}
			if (m_stResult.GradientY < stAnalysisInfo.GradientY)
			{
				m_stResult.GradientY = stAnalysisInfo.GradientY;
			}
			if (m_stResult.PedalDistance < stAnalysisInfo.PedalDistance)
			{
				m_stResult.PedalDistance = stAnalysisInfo.PedalDistance;
			}

			//save file data to vector for send to UI curve
			static int nCountBetweenSend = 0;
			++nCountBetweenSend;
			if (nCountBetweenSend > COUNTBETWEENSEND)//ÿCOUNTBETWEENSEND��������һ����
			{
				nCountBetweenSend = 0;
				ANALYSISDATA stData = {0.0};
				stData.Accelaration = dCompoundA;
				stData.Velocity = dCurrentVel;
				stData.FootBrakeForce = stAnalysisInfo.MaxFootBrakeForce;
				stData.PedalDistance = m_stResult.PedalDistance;
				m_vAnalysisData.push_back(stData);
				m_vAccelaration.push_back(stData.Accelaration);
				m_vFootBrakeForce.push_back(stData.FootBrakeForce);
				m_vPedalDistance.push_back(stData.PedalDistance);
				m_vVelocity.push_back(stData.Velocity);	
			}


		}
		m_stResult.AverageVelocity = dSumVel / (doubleNum/channelCount);
		m_stResult.BrakeDistance = dCurrentDist;
		m_stResult.MaxAccelaration = dCompoundA;

		NormalizData();
		SendAnalysisData();


		theApp->m_pDataController->TransformAcceleration(m_stResult.MaxAccelaration);
		theApp->m_pDataController->TransformVelocity(m_stResult.AverageVelocity);

		theApp->m_pDataController->TransformFootBrakeForce(m_stResult.MaxFootBrakeForce);
		//theApp->m_pDataController->TransformHandBrakeForce(m_stResult.MaxHandBrakeForce);
		theApp->m_pDataController->TransformGradient(m_stResult.GradientX);
		theApp->m_pDataController->TransformGradient(m_stResult.GradientY);
		//��Ҫ��ȥ���ʱ�����ο���������ǣ��õ�����ʵ�����
		m_stResult.GradientX -= m_dCarInitXAngle;
		m_stResult.GradientY -= m_dCarInitYAngle;
		theApp->m_pDataController->TransformPedalDistance(m_stResult.PedalDistance);

		m_stResult.MaxHandBrakeForce = m_dMaxHandBrakeForce;//from ini��������Ҫת��

		g_logger.TraceWarning("CAnalysis::HandleData - Result=%2f_%2f_%2f_%2f_%2f_%2f_%2f_%2f",
			m_stResult.MaxAccelaration,
			m_stResult.BrakeDistance,
			m_stResult.AverageVelocity,
			m_stResult.GradientX,
			m_stResult.GradientY,
			m_stResult.PedalDistance,
			m_stResult.MaxHandBrakeForce,
			m_stResult.MaxFootBrakeForce);

	}
	void CAnalysis::HandleDataW(const double* pData, const int channelCount, const DWORD dwDataSize/*Byte*/, const double deltat)
	{
		g_logger.TraceInfo("CAnalysis::HandleDataW -in");

		this->PostAnalysisStateMsg();
		int sectionLengthW = DAQCONTROLER::sectionLengthW;
		this->InitData();

		DWORD doubleNum = dwDataSize / (sizeof(double)) / channelCount;//����8ͨ��doubleֵ
		g_logger.TraceInfo("CAnalysis::HandleDataW - doubleNum=%d",doubleNum);
		double dSumA = 0.0;
		double dCompoundA = 0.0;

		double dSumVel=0.0;
		double dCurrentVel=0.0;
		double dCurrentDist=0.0;
		double minPedalDistance=*(pData+5*sectionLengthW);
		double maxPedalDistance=*(pData+5*sectionLengthW);

		//m_stResult.MaxAccelaration = *(pData);
		//m_stResult.MaxFootBrakeForce = *(pData+7*sectionLengthW);
		//m_stResult.GradientX = *(pData+1*sectionLengthW);
		//m_stResult.GradientY = *(pData+4*sectionLengthW);
		//m_stResult.PedalDistance = *(pData+5*sectionLengthW);

		Filter filterFootBrakeForceSingle(COUNTBETWEENSEND);
		Filter filterFootBrakeForce(doubleNum);
		ANALYSISRESULT stAnalysisInfo;
		ANALYSISDATA stData = {0.0};
		for (DWORD i=0;i<doubleNum;++i)
		{
			dCompoundA = *(pData+i);
			dCompoundA -= m_dInitAccA;
			dSumA += dCompoundA;
			dCurrentVel = dSumA*deltat;
			dSumVel += dCurrentVel;
			dCurrentDist = dSumVel*deltat;

			stAnalysisInfo.MaxFootBrakeForce = *(pData+7*sectionLengthW+i);
			stAnalysisInfo.GradientX = *(pData+1*sectionLengthW+i);
			stAnalysisInfo.GradientY = *(pData+4*sectionLengthW+i);
			stAnalysisInfo.PedalDistance = *(pData+5*sectionLengthW+i);

			//if (m_stResult.MaxFootBrakeForce < stAnalysisInfo.MaxFootBrakeForce)
			//{
			//	m_stResult.MaxFootBrakeForce = stAnalysisInfo.MaxFootBrakeForce;
			//}
			filterFootBrakeForceSingle.AddData(stAnalysisInfo.MaxFootBrakeForce);
			filterFootBrakeForce.AddData1(stAnalysisInfo.MaxFootBrakeForce);
			//if (m_stResult.MaxHandBrakeForce< stStressInfo.MaxHandBrakeForce)
			//{
			//	m_stResult.MaxHandBrakeForce = stStressInfo.MaxHandBrakeForce;
			//}
			if ( abs(m_stResult.GradientX) < abs(stAnalysisInfo.GradientX) )
			{
				m_stResult.GradientX = stAnalysisInfo.GradientX;
			}
			if ( abs(m_stResult.GradientY) < abs(stAnalysisInfo.GradientY) )
			{
				m_stResult.GradientY = stAnalysisInfo.GradientY;
			}
			if (maxPedalDistance < stAnalysisInfo.PedalDistance)
			{
				maxPedalDistance = stAnalysisInfo.PedalDistance;
			}
			if (minPedalDistance > stAnalysisInfo.PedalDistance)
			{
				minPedalDistance = stAnalysisInfo.PedalDistance;
			}

			//save file data to vector for send to UI curve
			static int nCountBetweenSend = 1;
			++nCountBetweenSend;
			if (nCountBetweenSend > COUNTBETWEENSEND)//ÿCOUNTBETWEENSEND������client����һ����
			{
				g_logger.TraceInfo("CAnalysis::HandleDataW - COUNTBETWEENSEND-in");

				nCountBetweenSend = 0;
				stData.Accelaration = dCompoundA;
				stData.Velocity += dCurrentVel;
				stData.FootBrakeForce = filterFootBrakeForceSingle.GetMeanData();
				g_logger.TraceInfo("CAnalysis::HandleDataW - COUNTBETWEENSEND-1");
				g_logger.TraceInfo("CAnalysis::HandleDataW - COUNTBETWEENSEND-2");
				stData.PedalDistance = m_stResult.PedalDistance;
				m_vAnalysisData.push_back(stData);
				m_vAccelaration.push_back(stData.Accelaration);
				m_vFootBrakeForce.push_back(stData.FootBrakeForce);
				m_vPedalDistance.push_back(stData.PedalDistance);
				m_vVelocity.push_back(stData.Velocity);	
				g_logger.TraceInfo("CAnalysis::HandleDataW - COUNTBETWEENSEND-out");

			}
		}
		m_stResult.AverageVelocity = dSumVel / (doubleNum/channelCount);
		m_stResult.BrakeDistance = dCurrentDist;
		m_stResult.MaxAccelaration = dCompoundA;

		//��ʾ������ֻ����ʾһ�����ƣ�����Ҫʵ��ֵ
		NormalizData();
		SendAnalysisData();

		theApp->m_pDataController->TransformAcceleration(m_stResult.MaxAccelaration);
		theApp->m_pDataController->TransformVelocity(m_stResult.AverageVelocity);

		m_stResult.MaxFootBrakeForce = filterFootBrakeForce.GetMidValue();
		m_stResult.MaxFootBrakeForce -= m_dInitFootBrakeForce;
		theApp->m_pDataController->TransformFootBrakeForce(m_stResult.MaxFootBrakeForce);

		//theApp->m_pDataController->TransformHandBrakeForce(m_stResult.MaxHandBrakeForce);
		theApp->m_pDataController->TransformGradient(m_stResult.GradientX);
		theApp->m_pDataController->TransformGradient(m_stResult.GradientY);
		//��Ҫ��ȥ���ʱ�����ο���������ǣ��õ�����ʵ�����
		m_stResult.GradientX -= m_dCarInitXAngle;
		m_stResult.GradientY -= m_dCarInitYAngle;

		theApp->m_pDataController->TransformPedalDistance(maxPedalDistance);
		theApp->m_pDataController->TransformPedalDistance(minPedalDistance);
		m_stResult.PedalDistance = maxPedalDistance - minPedalDistance;

		m_stResult.MaxHandBrakeForce = m_dMaxHandBrakeForce;//from ini��������Ҫת��

		g_logger.TraceWarning("CAnalysis::HandleDataW - Result=%2f_%2f_%2f_%2f_%2f_%2f_%2f_%2f",
			m_stResult.MaxAccelaration,
			m_stResult.BrakeDistance,
			m_stResult.AverageVelocity,
			m_stResult.GradientX,
			m_stResult.GradientY,
			m_stResult.PedalDistance,
			m_stResult.MaxHandBrakeForce,
			m_stResult.MaxFootBrakeForce);
	}
	
	bool CAnalysis::AnalyseResult()
	{
		g_logger.TraceInfo("CAnalysis::AnalyseResult-in");
		int nResult = NUM_ONE;//need add judge

		if(!PostThreadMessage(theApp->m_dwMainThreadID,msg_ANA_ANALYSIS_RESULT ,nResult,(LPARAM)&m_stResult))
		{
			g_logger.TraceError("CAnalysis::AnalyseResult - PostThreadMessage failed");
		}
		return true;
	}
	void CAnalysis::NormalizData()
	{//��һ����1-100֮���char����
		//accelaration - -0.625~0.625
		g_logger.TraceInfo("CAnalysis::NormalizData-in");
		double dminAccelaration=0,dmaxAccelaration=0;
		FindMaxMinRange(m_vAccelaration,dminAccelaration,dmaxAccelaration);
		double dminVelocity=0,dmaxVelocity=0;
		FindMaxMinRange(m_vVelocity,dminVelocity,dmaxVelocity);
		double dminFootBrakeForce=0,dmaxFootBrakeForce=0;
		FindMaxMinRange(m_vFootBrakeForce,dminFootBrakeForce,dmaxFootBrakeForce);
		double dminPedalDistance=0,dmaxPedalDistance=0;
		FindMaxMinRange(m_vPedalDistance,dminPedalDistance,dmaxPedalDistance);

		for( std::vector<ANALYSISDATA>::iterator itData = m_vAnalysisData.begin();
			itData!=m_vAnalysisData.end();
			++itData )
		{
			NormalizDataOne(itData->Accelaration, dminAccelaration, dmaxAccelaration);
			NormalizDataOne(itData->Velocity, dminVelocity, dmaxVelocity);
			NormalizDataOne(itData->FootBrakeForce, dminFootBrakeForce, dmaxFootBrakeForce);
			NormalizDataOne(itData->PedalDistance, dminPedalDistance, dmaxPedalDistance);
		}


	}
	void CAnalysis::NormalizDataOne(double& dData, const double dOriginMin, const double dOriginMax, const int nNewRange)
	{
		double dOriginRange = dOriginMax - dOriginMin;
		if (dOriginRange == 0)//�ɼ��������Ǻ�ֵ
		{
			if (dData>=nNewRange)
			{
				dData = nNewRange;
			}
			else if(dData <= 0)
			{
				dData = 0;
			}
			else
			{
				//do nothing
			}
		}
		else
		{
			dData = (dData-dOriginMin) / dOriginRange * nNewRange;
		}
	}

	void CAnalysis::FindMaxMinRange(vector<double> &vData/*in*/, double& minData, double& maxData)
	{
		g_logger.TraceInfo("CAnalysis::FindMaxMinRange-in");
		std::vector<double>::iterator max_ = std::max_element(std::begin(vData), std::end(vData));
		maxData = *max_;
		auto min_ = std::min_element(std::begin(vData), std::end(vData));
		minData = *min_;
		g_logger.TraceInfo("CAnalysis::FindMaxMinRange-max=%.2f,min=%.2f",maxData,minData);
		//�˲�:ԭʼ����С��10mv�Ķ�������
		if (10>(maxData - minData))
		{
			maxData = minData;
		}
	}

	void CAnalysis::SendAnalysisData()
	{
		g_logger.TraceInfo("CAnalysis::SendAnalysisData-in");
		theApp->m_pCommunicator->SendAnalysisData2UI(m_vAnalysisData);
		//if(!PostThreadMessage(theApp->m_dwMainThreadID, cmd_ANALYSIS_DATA, 1, (LPARAM)&stData))
		//{
		//	g_logger.TraceError("CAnalysis::AnalyseResult - PostThreadMessage failed");
		//}

	}
	//1 - ������
	//2 - �����ɹ�
	void CAnalysis::PostAnalysisStateMsg(const int nState)
	{
		PostThreadMessage(theApp->m_dwMainThreadID,msg_ANA_ANALYSIS_STATE,nState,NULL);//������
		g_logger.TraceInfo("CAnalysis::PostAnalysisStateMsg:%d",nState);
	}

}

