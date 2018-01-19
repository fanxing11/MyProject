#pragma once
#include "const.h"

#include "compatibility.h"
#include "bdaqctrl.h"
//#include <tchar.h>

using namespace Automation::BDaq;


namespace DAQCONTROLER
{

	#define       deviceDescription  L"USB-4716,BID#0"// DemoDevice
	const wchar_t* profilePath = L"profile.xml";
	int32         startChannel = 0;
	const int32   channelCount = 10;
	const int32   sectionLength = 1024;
	const int32   sectionCount = 0;

	DWORD    SingleSavingFileSize = 10240  * sizeof(double);//ÿ�βɼ�(����)1024*10����
	DWORD    RequirementFileSize  = 10240 * sizeof(double);//��������

	const double deltat = 0.0005;//sample rate = 2kHz,��������Ϊ1/2k

	HANDLE   hFile;
	double * buffer = NULL;
	DWORD    WrittenBytes;
	DWORD    RealFileSize;

#define       USER_BUFFER_SIZE   channelCount*sectionLength
	double        Data[USER_BUFFER_SIZE]; 
	//double		 DateOne[10];

	//event
	HANDLE m_gEvtSample;//-ing
	HANDLE m_gEvtStress;
	HANDLE m_gEvtVelocity;
	HANDLE m_gEvtSaveFile;
	HANDLE m_gEvtInitAngleFlag;//First need save InitAngle

	class CDAQControler
	{
	public:
		CDAQControler(void);
		~CDAQControler(void);

	public:
		void VelocityBegin();
		void VelocityEnd();
		void StressBegin();
		void StressEnd();

		void NewProject(char cMode);
		void TerminateProject();



	private:
		void Initialize();

		void SetInitAngleFlag();
		void SampleBegin();
		void SampleEnd();
		void CreateSyncEvent();
		void CloseEvtHandle();
		void DisInitialize();


		WaveformAiCtrl * m_wfAiCtrl;

		HANDLE m_hDAQThread;

		bool m_bDAQInitialSuccessfully;
		bool CheckDAQStarted()const;

	};


}
