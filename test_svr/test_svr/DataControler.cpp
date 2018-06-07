#include "main.h"

#include "DataControler.h"
#include <math.h>
#include "time.h"

#include <Dbghelp.h>
#pragma comment(lib,"Dbghelp.lib")

extern CtheApp* theApp;


namespace DATACONTROLER
{

	CDataControler::CDataControler(void)
		:m_hEvtMoveDetectionInfo(NULL)
		,m_hEvtStillDetectionInfo(NULL)
		,m_hEvtInitGradientInfo(NULL)
		,m_nCurrentProjectState(NUM_ZERO)
		,m_dInitYAngle(0.0)
		,m_dInitXAngle(0.0)
		,m_dInitCarXAngle(0.0)
		,m_dInitCarYAngle(0.0)
		,m_strConfigFullName("")
		,m_nCurrentType(NONTYPE)
		,m_bUpdateCarAngleFlag(false)
	{
		g_logger.TraceInfo("CDataControler::CDataControler");
		m_hEvtMoveDetectionInfo = CreateEvent(NULL,TRUE,FALSE,L"");
		m_hEvtStillDetectionInfo = CreateEvent(NULL,TRUE,FALSE,L"");
		m_hEvtInitGradientInfo = CreateEvent(NULL,TRUE,FALSE,L"");
	}

	CDataControler::~CDataControler(void)
	{
	}

	int CDataControler::GetCurrentProjectState()const
	{
		return m_nCurrentProjectState;
	}


	void CDataControler::SetStartChannel(const char cStart){m_cStartChannel = cStart;}
	void CDataControler::SetEndChannel(const char cEnd){m_cEndChannel = cEnd;}
	void CDataControler::SetSampleFrequency(const char cSampleFrequency){m_cSampleFrequency = cSampleFrequency;}
	void CDataControler::SetMode(const char cMode){m_cMode = cMode;}
	void CDataControler::SetArchiveFromat(char cFormat){m_cArchiveFormat = cFormat;}
	void CDataControler::SetUserName(const string strUserName){m_strUserName = strUserName;}
	void CDataControler::SetMaxAcceleratedVel(const float ff){m_fMaxAcceleratedVel = ff;}
	void CDataControler::SetBrakingLength(const float ff){m_fBrakingLength = ff;}
	void CDataControler::SetAverageVel(const float ff){m_fAverageVel = ff;}
	void CDataControler::SetGradient(const float ff){m_fGradient = ff;}
	void CDataControler::SetPedalDistance(const float ff){m_fPedalDistance = ff;}
	void CDataControler::SetMaxHandBrakeForce(const float ff){m_fMaxHandBrakeForce = ff;}
	void CDataControler::SetMaxFootBrakeForce(const float ff){m_fMaxFootBrakeForce = ff;}
	void CDataControler::SetResult(const char cResult){m_cResult = cResult;}
	void CDataControler::SetReportPath(const string strPath)
	{
		m_strReportPath = strPath;
	}
	void CDataControler::SetCurrentType(enDETECTION_TYPE dt)
	{
		m_nCurrentType = dt;
	}

	void CDataControler::SetUpdateCarAngleFlag()
	{
		g_logger.TraceInfo("CDataControler::SetUpdateCarAngleFlag");
		m_bUpdateCarAngleFlag = true;
	}
	bool CDataControler::SaveMaxHandBrakeForce2INI()
	{
		//����ֻ�����������ɲ�������Խ�������г���֮��Ĳ��������˶��еõ���
		//�����������,�༴,��ֹ����еĵ�����Ǳ�û�����ã�ֻ�ڲ���ʱչʾ��һ��
		char buf[40] = {0};
		sprintf_s(buf,"%f",m_dMaxHandBrakeForce);
		if (NUM_ZERO == WritePrivateProfileStringA(gc_strResult.c_str(),gc_strMaxHandBrakeForce.c_str(),buf,m_strConfigFullName.c_str()))
		{
			g_logger.TraceError("CDataControler::SaveProjectInfo2INIFile: WritePrivateProfileStringA,GetLastError=%d",
				GetLastError());
			return false;
		}
		return true;
	}


	void CDataControler::SetNewProjectPara(const char* pData)
	{
		//����������Ϣ�����浽�ڴ�
		int nLoc = 2;
		char cTemp;
		memcpy(&cTemp,pData+nLoc,1);
		this->SetStartChannel(cTemp);
		++nLoc;
		memcpy(&cTemp,pData+nLoc,1);
		this->SetEndChannel(cTemp);
		++nLoc;
		memcpy(&cTemp,pData+nLoc,1);
		this->SetSampleFrequency(cTemp);
		++nLoc;
		memcpy(&cTemp,pData+nLoc,1);
		this->SetMode(cTemp);
		++nLoc;
		memcpy(&cTemp,pData+nLoc,1);
		this->SetArchiveFromat(cTemp);
		++nLoc;
		int nInfo = 0;
		memcpy(&nInfo,pData+nLoc,1);

		string strPath = "";
		if (NUM_ZERO != nInfo)
		{
			++nLoc;
			char* pbuf = new char[nInfo+1];
			memset(pbuf,0,nInfo+1);
			memcpy(pbuf,pData+nLoc,nInfo);
			strPath = pbuf;
			delete[] pbuf;
			pbuf = NULL;
			if( !CheckFolderExist(strPath) )
			{
				g_logger.TraceWarning("Passed in New Project Folder not right,created new one.");
				strPath = "";
			}
		}
		if (strPath == "")
		{
			this->CreateDefaultProjectPath(strPath);
		}
		this->SetProjectPath(strPath);
	}

	//return = true,strInfo�ǹ���Ŀ¼
	//return = false, strInfo�Ǵ�����Ϣ
	bool CDataControler::NewProject(string& strInfo)
	{
		//����Ƿ�ʼ�������ʼ�������½�������Ϣ�����û�п�ʼ�����ñ�־λ��
		if (NUM_ONE == m_nCurrentProjectState)
		{
			strInfo = string("������ڽ����С�");
			return false;
		}
		//ȡ�ü��ģʽ������DAQ�߳��еı�־λ
		char cMode = theApp->m_pDataController->GetMode();
		//�����½���־��DAQ
		if(false == theApp->m_pDAQController->NewProject(m_cMode))
		{
			strInfo = string("�ɼ�����ʼ��ʧ�ܡ�");
			return false;
		}

		if(false == SaveProjectInfo2INIFile())
		{
			strInfo = string("����������ʧ�ܡ�");
			return false;
		}

		//�½��ɹ��������½��ɹ������Ϣ
		strInfo = m_strProjectPath;

		m_nCurrentProjectState = NUM_ONE;
		return true;
	}
	bool CDataControler::TerminateCurrentProject()
	{
		if(NUM_ZERO == m_nCurrentProjectState)
		{
			g_logger.TraceError("CDataControler::TerminateCurrentProject - current project have not started.");
			return false;
		}
		if(NUM_TWO == m_nCurrentProjectState)
		{
			g_logger.TraceError("CDataControler::TerminateCurrentProject - current project already terminated.");
			return false;
		}
		//����DAQ��־����
		theApp->m_pDAQController->TerminateProject();

		m_nCurrentProjectState = NUM_TWO;
		return true;
	}


	char CDataControler::GetMode() const
	{
		return m_cMode;
	}

	//��������Ŀ¼���½�����Ŀ¼����������Ϣ���ļ�
	bool CDataControler::SaveProjectInfo2INIFile()
	{
		bool bRet=true;
		m_strConfigFullName = m_strProjectPath + gc_strProjectParaINI_FileName;

		char buf[40] = {0};
		sprintf_s(buf,"%d",m_cStartChannel);
		if (NUM_ZERO == WritePrivateProfileStringA(gc_strProjectInfo.c_str(),"StartChannel",buf,m_strConfigFullName.c_str()))
		{
			g_logger.TraceError("CDataControler::SaveProjectInfo2INIFile: WritePrivateProfileStringA,GetLastError=%d",
				GetLastError());
			bRet = false;
		}

		memset(buf,0,40);
		sprintf_s(buf,"%d",m_cEndChannel);
		if( NUM_ZERO == WritePrivateProfileStringA(gc_strProjectInfo.c_str(),"EndChannel",buf,m_strConfigFullName.c_str()) )
		{
			g_logger.TraceError("CDataControler::SaveProjectInfo2INIFile: WritePrivateProfileStringA,GetLastError=%d",
				GetLastError());
			bRet = false;
		}

		memset(buf,0,40);
		sprintf_s(buf,"%d KHz",m_cSampleFrequency);
		if( NUM_ZERO == WritePrivateProfileStringA(gc_strProjectInfo.c_str(),"SampleFrequency",buf,m_strConfigFullName.c_str()) )
		{
			g_logger.TraceError("CDataControler::SaveProjectInfo2INIFile: WritePrivateProfileStringA,GetLastError=%d",
				GetLastError());
			bRet = false;
		}

		memset(buf,0,40);
		switch(m_cMode)
		{
		case 0x01:
			sprintf_s(buf,"����ģʽ");
			break;
		case 0x02:
			sprintf_s(buf,"�������ģʽ");
			break;
		default:
			{
				g_logger.TraceError("CDataControler::SaveProjectInfo2INIFile error: Mode=%d",(int)m_cMode);
				bRet = false;
				break;
			}
		}
		if( NUM_ZERO == WritePrivateProfileStringA(gc_strProjectInfo.c_str(),"Mode",buf,m_strConfigFullName.c_str()))
		{
			g_logger.TraceError("CDataControler::SaveProjectInfo2INIFile: WritePrivateProfileStringA,GetLastError=%d",
				GetLastError());
			bRet = false;
		}

		memset(buf,0,40);
		switch(m_cArchiveFormat)
		{
		case 0x01:
			sprintf_s(buf,"word");
			break;
		case 0x02:
			sprintf_s(buf,"excel");
			break;
		case 0x03:
			sprintf_s(buf,"txt");
			break;
		}
		if( NUM_ZERO == WritePrivateProfileStringA(gc_strProjectInfo.c_str(),"ArchiveFormat",buf,m_strConfigFullName.c_str()))
		{
			g_logger.TraceError("CDataControler::SaveProjectInfo2INIFile: WritePrivateProfileStringA,GetLastError=%d",
				GetLastError());
			bRet = false;
		}

		return bRet;
	}

	void CDataControler::SetProjectPath(string& strPath)
	{
		if ( strPath.at(strPath.length()-1) != '\\' )
		{
			strPath.append(1,'\\');
		}
		m_strProjectPath = strPath;
	}

	bool CDataControler::GetProjectPath(string& strPath) const
	{
		if (m_strProjectPath == "")
		{
			g_logger.TraceError("CDataControler::GetProjectPath - m_strProjectPath is NULL");
		}
		strPath = m_strProjectPath ;
		return true;
	}

	void CDataControler::CreateDefaultProjectPath(string &strPath)
	{
		//make a file path
		struct tm *local;
		time_t t;
		t = time(NULL);
		local = localtime(&t);
		char stFilePath[MAX_PATH] = {0};
		sprintf_s(stFilePath,"C:\\CJZD_Data\\%04d%02d%02d_%02d%02d%02d\\",
			local->tm_year+1900,
			local->tm_mon+1,
			local->tm_mday,
			local->tm_hour,
			local->tm_min,
			local->tm_sec
			);
		MakeSureDirectoryPathExists(stFilePath);
		strPath = stFilePath;
	}
	void CDataControler::SaveCarAngle()
	{
		//��һ�ξ�ֹ���ʱ������Ǽ�ȥ���������Ϊ��������ǣ������浽ini��
		m_dInitCarXAngle = m_stStillDetectionInfo.GradientX - m_dInitXAngle;
		m_dInitCarYAngle = m_stStillDetectionInfo.GradientY - m_dInitYAngle;

		char bufx[40] = {0};
		sprintf_s(bufx,"%f",m_dInitCarXAngle);
		if(!WritePrivateProfileStringA(gc_strInitialCarAngle.c_str(),gc_strInitXAngle.c_str(),bufx,m_strConfigFullName.c_str()) )
		{
			g_logger.TraceError("CDataControler::SaveCarAngleX - %d",GetLastError());
		}

		char bufy[40] = {0};
		sprintf_s(bufy,"%f",m_dInitCarYAngle);
		if( !WritePrivateProfileStringA(gc_strInitialCarAngle.c_str(),gc_strInitYAngle.c_str(),bufy,m_strConfigFullName.c_str()) )
		{
			g_logger.TraceError("CDataControler::SaveCarAngleY - %d",GetLastError());
		}

	}

	//�����ɲ����XY���
	//V = sum(ai*t) 
	void CDataControler::HandleStillDetectionData(const double* pData, const int channelCount, const int sectionLength)
	{
		ResetEvent(m_hEvtStillDetectionInfo);
		m_stStillDetectionInfo.MaxHandBrakeForce = *(pData+4) - *(pData+5);
		m_stStillDetectionInfo.GradientX = *(pData+2);
		m_stStillDetectionInfo.GradientY = *(pData+3);
		TransformGradient(m_stStillDetectionInfo.GradientX);
		TransformGradient(m_stStillDetectionInfo.GradientY);

		STILLDETECTIONINFO stStillDetectionInfo;
		for (int i=0;i<sectionLength;++i)
		{
			stStillDetectionInfo.MaxHandBrakeForce = *(pData+(i*channelCount)+4) - *(pData+(i*channelCount)+5);
			stStillDetectionInfo.GradientX = *(pData+((i*channelCount))+2);
			stStillDetectionInfo.GradientY = *(pData+((i*channelCount))+3);
			TransformGradient(stStillDetectionInfo.GradientX);
			TransformGradient(stStillDetectionInfo.GradientY);
			if (m_stStillDetectionInfo.MaxHandBrakeForce < stStillDetectionInfo.MaxHandBrakeForce)
			{
				m_stStillDetectionInfo.MaxHandBrakeForce = stStillDetectionInfo.MaxHandBrakeForce;
			}
			if ( abs(m_stStillDetectionInfo.GradientX) < abs(stStillDetectionInfo.GradientX) )
			{
				m_stStillDetectionInfo.GradientX = stStillDetectionInfo.GradientX;
			}
			if ( abs(m_stStillDetectionInfo.GradientY) < abs(stStillDetectionInfo.GradientY) )
			{
				m_stStillDetectionInfo.GradientY = stStillDetectionInfo.GradientY;
			}

		}
		TransformHandBrakeForce(m_stStillDetectionInfo.MaxHandBrakeForce);

		if( m_dMaxHandBrakeForce<m_stStillDetectionInfo.MaxHandBrakeForce)//�����¾�ֹʱ�������ɲ��
		{
			m_dMaxHandBrakeForce = m_stStillDetectionInfo.MaxHandBrakeForce;
		}

		if(m_bUpdateCarAngleFlag)
		{
			SaveCarAngle();
			m_bUpdateCarAngleFlag = false;
		}
		SetEvent(m_hEvtStillDetectionInfo);
	}
	//����ɲ������ɲλ�á�XY��ǡ����ٶȽ���ȡ���һ�����ٶ�ȡ�����������㴦���ٶ�
	//�õ������ɲ���ͽ�ɲ�����¶Ⱥͽ�ɲλ�ý���ȡ��һ��ֵ
	void CDataControler::HandleMoveDetectionData(const double* pData, const int channelCount, const int sectionLength, const double deltat)
	{
		ResetEvent(m_hEvtMoveDetectionInfo);
		double dSumA = 0.0;
		double dCompoundA = 0.0;

		m_stMoveDetectionInfo.MaxFootBrakeForce = *(pData+0) - *(pData+1);
		m_stMoveDetectionInfo.GradientX = *(pData+2);
		m_stMoveDetectionInfo.GradientY = *(pData+3);
		TransformGradient(m_stMoveDetectionInfo.GradientX);
		TransformGradient(m_stMoveDetectionInfo.GradientY);
		m_stMoveDetectionInfo.PedalDistance = *(pData+6);
		MOVEDETECTIONINFO stMoveDetectionInfo;
		for (int i=0;i<sectionLength;++i)
		{
			dCompoundA = sqrt((*(pData+(i*channelCount)+7))*(*(pData+(i*channelCount)+7))+(*(pData+(i*channelCount)+8))*(*(pData+(i*channelCount)+8))+(*(pData+(i*channelCount)+9))*(*(pData+(i*channelCount)+9)));
			dSumA += dCompoundA;

			stMoveDetectionInfo.GradientX = *(pData+((i*channelCount))+2);
			stMoveDetectionInfo.GradientY = *(pData+((i*channelCount))+3);
			TransformGradient(stMoveDetectionInfo.GradientX);
			TransformGradient(stMoveDetectionInfo.GradientY);
			if ( abs(m_stMoveDetectionInfo.GradientX) < abs(stMoveDetectionInfo.GradientX) )
			{
				m_stMoveDetectionInfo.GradientX = stMoveDetectionInfo.GradientX;
			}
			if ( abs(m_stMoveDetectionInfo.GradientY) < abs(stMoveDetectionInfo.GradientY) )
			{
				m_stMoveDetectionInfo.GradientY = stMoveDetectionInfo.GradientY;
			}
			stMoveDetectionInfo.MaxFootBrakeForce = *(pData+(i*channelCount)) - *(pData+(i*channelCount)+1);
			if (m_stMoveDetectionInfo.MaxFootBrakeForce < stMoveDetectionInfo.MaxFootBrakeForce)
			{
				m_stMoveDetectionInfo.MaxFootBrakeForce = stMoveDetectionInfo.MaxFootBrakeForce;
			}
			stMoveDetectionInfo.PedalDistance = *(pData+(i*channelCount)+6);
			if (m_stMoveDetectionInfo.PedalDistance < stMoveDetectionInfo.PedalDistance)
			{
				m_stMoveDetectionInfo.PedalDistance = stMoveDetectionInfo.PedalDistance;
			}

		}
		m_stMoveDetectionInfo.LastAccelaration = dCompoundA;
		m_stMoveDetectionInfo.LastVelocity = dSumA * deltat;
		TransformAcceleration(m_stMoveDetectionInfo.LastAccelaration);
		TransformVelocity(m_stMoveDetectionInfo.LastVelocity);

		TransformFootBrakeForce(m_stMoveDetectionInfo.MaxFootBrakeForce);
		TransformPedalDistance(m_stMoveDetectionInfo.PedalDistance);

		//GetInitXAngle ��Ҫ��ȥ��ʼ�������
		m_stMoveDetectionInfo.GradientX = m_stMoveDetectionInfo.GradientX-m_dInitCarXAngle;
		m_stMoveDetectionInfo.GradientY = m_stMoveDetectionInfo.GradientY-m_dInitCarYAngle;

		SetEvent(m_hEvtMoveDetectionInfo);
	}

	void CDataControler::HandleInitGradientData(const double* pData, const int channelCount, const int sectionLength)
	{
		ResetEvent(m_hEvtInitGradientInfo);
		double dMaxX = *(pData+2);
		double dMaxY = *(pData+3);
		TransformGradient(dMaxX);
		TransformGradient(dMaxY);
		double dX=0,dY=0;

		for (int i=0;i<sectionLength;++i)
		{
			dX = *(pData+(i*channelCount)) - *(pData+(i*channelCount)+2);
			dY = *(pData+(i*channelCount)+4) - *(pData+(i*channelCount)+3);
			TransformGradient(dX);
			TransformGradient(dY);//������abs��ת�����м��������ܸı������ţ�������Ҫ�任���ٱȽ�
			if (abs(dMaxX)<abs(dX))
			{
				dMaxX = dX;
			}
			if ( abs(dMaxY)<abs(dY) )
			{
				dMaxY = dY;
			}
		}

		m_dInitXAngle = dMaxX;
		m_dInitYAngle = dMaxY;

		SetEvent(m_hEvtInitGradientInfo);
	}

	//����ɲ������ɲλ�á�XY��ǡ����ٶȽ���ȡ���һ�����ٶ�ȡ�����������㴦���ٶ�
	//�õ������ɲ���ͽ�ɲ�����¶Ⱥͽ�ɲλ�ý���ȡ��һ��ֵ
	void CDataControler::HandleMoveDetectionDataW(const double* pData, const int channelCount, const int sectionLength, const double deltat)
	{
		g_logger.TraceInfo("CDataControler::HandleMoveDetectionDataW");

		ResetEvent(m_hEvtMoveDetectionInfo);
		double dSumA = 0.0;
		double dCompoundA = 0.0;

		m_stMoveDetectionInfo.MaxFootBrakeForce = *(pData+7*1024);
		m_stMoveDetectionInfo.GradientX = *(pData+1*1024);
		m_stMoveDetectionInfo.GradientY = *(pData+4*1024);
		TransformGradient(m_stMoveDetectionInfo.GradientX);
		TransformGradient(m_stMoveDetectionInfo.GradientY);
		m_stMoveDetectionInfo.PedalDistance = *(pData+5*1024);
		MOVEDETECTIONINFO stMoveDetectionInfo;
		for (int i=0;i<sectionLength;++i)
		{
			dCompoundA = sqrt((*(pData+i)) * (*(pData+i)) + (*(pData+2*1024+i) * (*(pData+2*1024+i))) + (*(pData+3*1024+i)) * (*(pData+3*1024+i)) );
			dSumA += dCompoundA;

			stMoveDetectionInfo.GradientX = *(pData+1*1024+i);
			stMoveDetectionInfo.GradientY = *(pData+4*1024+i);
			TransformGradient(stMoveDetectionInfo.GradientX);
			TransformGradient(stMoveDetectionInfo.GradientY);
			if ( abs(m_stMoveDetectionInfo.GradientX) < abs(stMoveDetectionInfo.GradientX) )
			{
				m_stMoveDetectionInfo.GradientX = stMoveDetectionInfo.GradientX;
			}
			if ( abs(m_stMoveDetectionInfo.GradientY) < abs(stMoveDetectionInfo.GradientY) )
			{
				m_stMoveDetectionInfo.GradientY = stMoveDetectionInfo.GradientY;
			}
			stMoveDetectionInfo.MaxFootBrakeForce = *(pData+7*1024+i);
			if (m_stMoveDetectionInfo.MaxFootBrakeForce < stMoveDetectionInfo.MaxFootBrakeForce)
			{
				m_stMoveDetectionInfo.MaxFootBrakeForce = stMoveDetectionInfo.MaxFootBrakeForce;
			}
			stMoveDetectionInfo.PedalDistance = *(pData+5*1024+i);
			if (m_stMoveDetectionInfo.PedalDistance < stMoveDetectionInfo.PedalDistance)
			{
				m_stMoveDetectionInfo.PedalDistance = stMoveDetectionInfo.PedalDistance;
			}

		}
		m_stMoveDetectionInfo.LastAccelaration = dCompoundA;
		m_stMoveDetectionInfo.LastVelocity = dSumA * deltat * 1024;
		TransformAcceleration(m_stMoveDetectionInfo.LastAccelaration);
		TransformVelocity(m_stMoveDetectionInfo.LastVelocity);

		TransformFootBrakeForce(m_stMoveDetectionInfo.MaxFootBrakeForce);
		TransformPedalDistance(m_stMoveDetectionInfo.PedalDistance);

		//GetInitXAngle ��Ҫ��ȥ��ʼ�������
		m_stMoveDetectionInfo.GradientX = m_stMoveDetectionInfo.GradientX-m_dInitCarXAngle;
		m_stMoveDetectionInfo.GradientY = m_stMoveDetectionInfo.GradientY-m_dInitCarYAngle;

		SetEvent(m_hEvtMoveDetectionInfo);
	}

	void CDataControler::HandleInitGradientDataW(const double* pData, const int channelCount, const int sectionLength)
	{
		g_logger.TraceInfo("CDataControler::HandleInitGradientDataW");
		
		ResetEvent(m_hEvtInitGradientInfo);
		double dMaxX = *(pData+1*1024);
		double dMaxY = *(pData+4*1024);
		TransformGradient(dMaxX);
		TransformGradient(dMaxY);
		double dX=0,dY=0;

		for (int i=0;i<sectionLength;++i)
		{
			dX = *(pData+1*1024+i);
			dY = *(pData+4*1024+i);
			TransformGradient(dX);
			TransformGradient(dY);//������abs��ת�����м��������ܸı������ţ�������Ҫ�任���ٱȽ�
			if (abs(dMaxX)<abs(dX))
			{
				dMaxX = dX;
			}
			if ( abs(dMaxY)<abs(dY) )
			{
				dMaxY = dY;
			}
		}

		m_dInitXAngle = dMaxX;
		m_dInitYAngle = dMaxY;

		SetEvent(m_hEvtInitGradientInfo);
	}

	//�����ɲ����XY���
	void CDataControler::HandleStillDetectionDataW(const double* pData, const int channelCount, const int sectionLength)
	{
		g_logger.TraceInfo("CDataControler::HandleStillDetectionDataW");
		//V = sum(ai*t) 

		ResetEvent(m_hEvtStillDetectionInfo);
		m_stStillDetectionInfo.MaxHandBrakeForce = *(pData+6*1024);
		m_stStillDetectionInfo.GradientX = *(pData+1*1024);
		m_stStillDetectionInfo.GradientY = *(pData+4*1024);
		TransformGradient(m_stStillDetectionInfo.GradientX);
		TransformGradient(m_stStillDetectionInfo.GradientY);

		STILLDETECTIONINFO stStillDetectionInfo;
		for (int i=0;i<sectionLength;++i)
		{
			stStillDetectionInfo.MaxHandBrakeForce = *(pData+6*1024+i);
			stStillDetectionInfo.GradientX = *(pData+1*1024+i);
			stStillDetectionInfo.GradientY = *(pData+4*1024+i);
			TransformGradient(stStillDetectionInfo.GradientX);
			TransformGradient(stStillDetectionInfo.GradientY);
			if (m_stStillDetectionInfo.MaxHandBrakeForce < stStillDetectionInfo.MaxHandBrakeForce)
			{
				m_stStillDetectionInfo.MaxHandBrakeForce = stStillDetectionInfo.MaxHandBrakeForce;
			}
			if ( abs(m_stStillDetectionInfo.GradientX) < abs(stStillDetectionInfo.GradientX) )
			{
				m_stStillDetectionInfo.GradientX = stStillDetectionInfo.GradientX;
			}
			if ( abs(m_stStillDetectionInfo.GradientY) < abs(stStillDetectionInfo.GradientY) )
			{
				m_stStillDetectionInfo.GradientY = stStillDetectionInfo.GradientY;
			}

		}
		TransformHandBrakeForce(m_stStillDetectionInfo.MaxHandBrakeForce);

		if( m_dMaxHandBrakeForce<m_stStillDetectionInfo.MaxHandBrakeForce)//�����¾�ֹʱ�������ɲ��
		{
			m_dMaxHandBrakeForce = m_stStillDetectionInfo.MaxHandBrakeForce;
		}

		if(m_bUpdateCarAngleFlag)
		{
			SaveCarAngle();
			m_bUpdateCarAngleFlag = false;
		}
		SetEvent(m_hEvtStillDetectionInfo);
	}


	void CDataControler::GetInitGradientInfo(double& dX, double& dY)
	{
		DWORD dwRet = WaitForSingleObject(m_hEvtInitGradientInfo,1);
		if (WAIT_OBJECT_0 == dwRet)
		{
			dX = m_dInitXAngle;
			dY = m_dInitYAngle;
		}
		else
		{
			g_logger.TraceError("CDataControler::GetInitGradientInfo - %d",dwRet);
		}
	}
	void CDataControler::GetMoveDetectionInfo(MOVEDETECTIONINFO& stStressInfo)
	{
		DWORD dwRet = WaitForSingleObject(m_hEvtMoveDetectionInfo,1);
		if (WAIT_OBJECT_0 == dwRet)
		{
			stStressInfo = m_stMoveDetectionInfo;
		}
		else
		{
			g_logger.TraceError("CDataControler::GetStressInfo - %d",dwRet);
		}
	}
	void CDataControler::GetStillDetectionInfo(STILLDETECTIONINFO& stStillDetectionInfo)
	{
		DWORD dwRet = WaitForSingleObject(m_hEvtStillDetectionInfo,1);
		if (WAIT_OBJECT_0 == dwRet)
		{
			stStillDetectionInfo = m_stStillDetectionInfo;
		}
		else
		{
			g_logger.TraceError("CDataControler::GetVelocityInfo - %d",dwRet);
		}
	}

	bool TransformBrakeDistance(double & dDist)
	{
		dDist = dDist/0.04;
		return true;
	}
	bool CDataControler::TransformVelocity(double & dVel)
	{//nothing - DO NOT calc repeatedly!
		dVel = dVel/4.1;
		//dVel = dVel/0.04;
		return true;
	}
	bool CDataControler::TransformAcceleration(double & dAcc)
	{
		dAcc  = dAcc/4.1;
		//dAcc  = dAcc/0.04;
		return true;
	}
	bool CDataControler::TransformFootBrakeForce(double &dForce)
	{
		dForce = (dForce-15.2) * 0.87048384;
		//dForce = (dForce-0.095)*245.0259728;
		return true;
	}
	bool CDataControler::TransformHandBrakeForce(double &dForce)
	{
		dForce = (dForce - 4) * 0.27674141;
		//dForce = dForce * 4166.6666;
		return true;
	}
	bool CDataControler::TransformGradient(double &dGradient)
	{
		dGradient = tan( (dGradient-2500)/83.333 ) * 100;//%�¶ȵ�λ��100%��ʾ������*100
		//dGradient = (dGradient-2.59)*0.08333;
		return true;
	}
	bool CDataControler::TransformPedalDistance(double &dDist)
	{
		dDist = 1 / (0.12592593*dDist-0.01703704);
		//dDist = 1 / (dDist-0.44) * 0.1026856240126 + 1/30;
		return true;
	}

}
