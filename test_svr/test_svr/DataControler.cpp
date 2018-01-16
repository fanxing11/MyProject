#include "DataControler.h"
#include <math.h>

namespace DATACONTROLER
{

	CDataControler::CDataControler(void)
	{
	}


	CDataControler::~CDataControler(void)
	{
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
