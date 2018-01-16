#pragma once
#include "const.h"

namespace DATACONTROLER
{

	class CDataControler
	{
	public:
		CDataControler(void);
		~CDataControler(void);
	private:
		char m_cStartChannel;
		char m_cEndChannel;
		//0x01:1K
		//0x02:2K
		//0x03:3K
		char m_cSampleFrequency;
		//0x01:���ٶ�
		//0x02:������
		//0x03:�������
		char m_cMode;//���ģʽ
		//0x01:word
		//0x02:excel
		//0x03:txt
		char m_cArchiveFormat;

		string m_strUserName;

		float m_fMaxAcceleratedVel;
		float m_fBrakingLength;
		float m_fAverageVel;
		float m_fGradient;//podu
		float m_fPedalDistance;
		float m_fMaxHandBrakeForce;
		float m_fMaxFootBrakeForce;
		char m_cResult;

		string m_strReportPath;

	public:
		void SetStartChannel(const char cStart);
		void SetEndChannel(const char cEnd);
		void SetSampleFrequency(const char cSampleFrequency);
		void SetMode(const char cMode);
		void SetArchiveFromat(char cFormat);
		void SetUserName(const string strUserName);

		void SetMaxAcceleratedVel(const float ff);
		void SetBrakingLength(const float ff);
		void SetAverageVel(const float ff);
		void SetGradient(const float ff);//�¶�.�������
		void SetPedalDistance(const float ff);
		void SetMaxHandBrakeForce(const float ff);
		void SetMaxFootBrakeForce(const float ff);
		void SetResult(const char cResult);

		void SetReportPath(const string strPath);
		 
			
	public:
		void HandleVelocityData(const double* pData, const int channelCount, const int sectionLength, const double deltat);
		void HandleStressData(const double* pData, const int channelCount, const int sectionLength);
		void GetStressInfo(STRESSINFO& stStressInfo);
		void GetVelocityInfo(VELOCITYINFO& stVelocityInfo);
	private:
		STRESSINFO m_stStressInfo;
		VELOCITYINFO m_stVelocityInfo;
	};

}

