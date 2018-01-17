#include "main.h"

#include "DataControler.h"
#include <math.h>
#include "time.h"

extern CtheApp theApp;


namespace DATACONTROLER
{

	CDataControler::CDataControler(void)
	{
		m_bCurrentProjectGoing = false;
	}

	CDataControler::~CDataControler(void)
	{
	}

	bool CDataControler::GetCurrentProjectState()const
	{
		return m_bCurrentProjectGoing;
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
		string strPath;
		if (NUM_ZERO == nInfo)
		{
			this->CreateProjectPath(strPath);
		}
		else
		{
			++nLoc;
			char* pbuf = new char[nInfo+1];
			memset(pbuf,0,nInfo+1);
			memcpy(pbuf,pData+nLoc,nInfo);
			strPath = pbuf;
			delete[] pbuf;
			pbuf = NULL;
		}
		this->SetProjectPath(strPath);

	}

	//return = true,strInfo�ǹ���Ŀ¼
	//return = false, strInfo�Ǵ�����Ϣ
	bool CDataControler::NewProject(string& strInfo)
	{
		//����Ƿ�ʼ�������ʼ�������½�������Ϣ�����û�п�ʼ�����ñ�־λ��
		if (m_bCurrentProjectGoing)
		{
			strInfo = string("������ڽ����С�");
			return false;
		}
		m_bCurrentProjectGoing = true;
		//ȡ�ü��ģʽ������DAQ�߳��еı�־λ



		//��������Ŀ¼���½�����Ŀ¼����������Ϣ���ļ�
		SaveProjectInfo2File();
		//�����½��ɹ������Ϣ
		strInfo = m_strProjectPath;
		return true;
	}
	bool CDataControler::TerminateCurrentProject()
	{
		m_bCurrentProjectGoing = false;
		//����DAQ��־����

		return true;
	}


	char CDataControler::GetMode() const
	{
		return m_cMode;
	}

	void CDataControler::SaveProjectInfo2File()
	{
		string strConfigName("config.ini");
		string strAppName("ProjectInfo");
		string strConfigFullName = m_strProjectPath+"\\"+strConfigName;
		string strTemp;
		char buf[40] = {0};
		
		sprintf_s(buf,"%d",m_cStartChannel);
		WritePrivateProfileStringA(strAppName.c_str(),"StartChannel",buf,strConfigFullName.c_str());
		memset(buf,0,40);
		sprintf_s(buf,"%d",m_cEndChannel);
		WritePrivateProfileStringA(strAppName.c_str(),"EndChannel",buf,strConfigFullName.c_str());
		memset(buf,0,40);
		sprintf_s(buf,"%d KHz",m_cSampleFrequency);
		WritePrivateProfileStringA(strAppName.c_str(),"SampleFrequency",buf,strConfigFullName.c_str());
		memset(buf,0,40);
		switch(m_cMode)
		{
		case 0x01:
			sprintf_s(buf,"����ģʽ");
			break;
		case 0x02:
			sprintf_s(buf,"���ٶ�ģʽ");
			break;
		case 0x03:
			sprintf_s(buf,"������ģʽ");
			break;
		case 0x04:
			sprintf_s(buf,"�������ģʽ");
			break;
		}
		WritePrivateProfileStringA(strAppName.c_str(),"Mode",buf,strConfigFullName.c_str());
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
		sprintf_s(buf,"%d KHz",m_cArchiveFormat);
		WritePrivateProfileStringA(strAppName.c_str(),"ArchiveFormat",buf,strConfigFullName.c_str());		
	}

	void CDataControler::SetProjectPath(const string strPath)
	{
		m_strProjectPath = strPath;
	}

	bool CDataControler::GetProjectPath(string& strPath) const
	{

		return true;
	}

	void CDataControler::CreateProjectPath(string &strPath)
	{
		//make a file path
		struct tm *local;
		time_t t;
		t = time(NULL);
		local = localtime(&t);
		char stFilePath[1000] = {0};
		sprintf_s(stFilePath,"d:\\Data\\%04d%02d%02d_%02d%02d%02d",
			local->tm_year+1900,
			local->tm_mon+1,
			local->tm_mday,
			local->tm_hour,
			local->tm_min,
			local->tm_sec
			);
		if( !::CreateDirectoryA(stFilePath,NULL))
		{
			g_logger.TraceError("CDataControler::CreateProjectPath failed");
		}
		strPath = stFilePath;
	}

	//�õ��ٶȺͼ��ٶȣ����ٶȽ���ȡ���һ�����ٶ�ȡ�����������㴦���ٶ�
	//V = sum(ai*t) 
	void CDataControler::HandleVelocityData(const double* pData, const int channelCount, const int sectionLength, const double deltat)
	{
		double dSumA = 0.0;
		double dCompoundA = 0.0;
		for (int i=0;i<sectionLength;++i)
		{
			dCompoundA = sqrt((*(pData+(i*channelCount)+7))*(*(pData+(i*channelCount)+7))+(*(pData+(i*channelCount)+8))*(*(pData+(i*channelCount)+8))+(*(pData+(i*channelCount)+9))*(*(pData+(i*channelCount)+9)));
			dSumA += dCompoundA;
		}
		m_stVelocityInfo.LastAccelaration = dCompoundA;
		m_stVelocityInfo.LastVelocity = dSumA * deltat;
	}
	//�õ������ɲ���ͽ�ɲ�����¶Ⱥͽ�ɲλ�ý���ȡ��һ��ֵ
	void CDataControler::HandleStressData(const double* pData, const int channelCount, const int sectionLength)
	{
		m_stStressInfo.MaxFootBrakeForce = *(pData+0) - *(pData+1);
		m_stStressInfo.Gradient = *(pData+2);
		m_stStressInfo.MaxHandBrakeForce = *(pData+4) - *(pData+5);
		m_stStressInfo.PedalDistance = *(pData+6);
		STRESSINFO stStressInfo;
		for (int i=0;i<sectionLength;++i)
		{
			stStressInfo.MaxFootBrakeForce = *(pData+(i*channelCount)) - *(pData+(i*channelCount)+1);
			stStressInfo.MaxHandBrakeForce = *(pData+(i*channelCount)+4) - *(pData+(i*channelCount)+5);
			if (m_stStressInfo.MaxFootBrakeForce < stStressInfo.MaxFootBrakeForce)
			{
				m_stStressInfo.MaxFootBrakeForce = stStressInfo.MaxFootBrakeForce;
			}
			if (m_stStressInfo.MaxHandBrakeForce< stStressInfo.MaxHandBrakeForce)
			{
				m_stStressInfo.MaxHandBrakeForce = stStressInfo.MaxHandBrakeForce;
			}
		}

	}
	void CDataControler::GetStressInfo(STRESSINFO& stStressInfo)
	{
		stStressInfo = m_stStressInfo;
	}
	void CDataControler::GetVelocityInfo(VELOCITYINFO& stVelocityInfo)
	{
		stVelocityInfo = m_stVelocityInfo;
	}


}
