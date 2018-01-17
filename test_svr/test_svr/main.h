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

