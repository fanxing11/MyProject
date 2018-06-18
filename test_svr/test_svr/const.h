#pragma once 
#include <Windows.h>
#include <io.h>  //struct _finddata_t
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
using namespace std;
#include <stdio.h>

#include "Logger.h"
using namespace LOGGER;
extern CLogger g_logger;

#include <process.h>//_beginthreadex_endthreadex
#pragma comment(lib,"user32.lib")//GetMessage

const int NUM_ZERO = 0;
const int NUM_NEGONE = -1;
const int NUM_ONE = 1;
const int NUM_TWO = 2;
const int NUM_THREE = 3;
const int NUM_FOUR = 4;
const int NUM_FIVE = 5;
const int NUM_SIX = 6;
const int NUM_SEVEN = 7;
const int NUM_EIGHT = 8;
const int NUM_NINE = 9;
const int NUM_TEN = 10;
const int NUM_ELENEN = 11;
const double NUM_PI = 3.141592654;


//----- UDP_rev_thread to main thread msg  
// && main thread to UDP thread SendDatatoUI
const UINT msg_CONTROL_INITIL = WM_USER+100;
const UINT msg_CONTROL_QUIT = WM_USER+101;

const UINT msg_DB_USERREGISTER = WM_USER+200;
const UINT msg_DB_USERLOGIN = WM_USER+201;
const UINT msg_DB_MODIFYPASSWORD = WM_USER+202;
const UINT msg_DB_DELETEUSER = WM_USER+203;
const UINT msg_DAQ_NEWPROJECT = WM_USER+204;
const UINT msg_DAQ_TERMINATEPROJECT = WM_USER+205;
const UINT msg_DAQ_VELOCITY_BEGIN = WM_USER+206;
const UINT msg_DAQ_VELOCITY_END = WM_USER+207;
const UINT msg_DAQ_STRESS_BEGIN = WM_USER+208;
const UINT msg_DAQ_STRESS_END = WM_USER+209;
const UINT msg_DATA_SETREPORTPATH = WM_USER+210;
const UINT msg_ANA_ANALYSIS_BEGIN = WM_USER+211;
const UINT msg_ANA_ANALYSIS_STATE = WM_USER+212;
const UINT msg_ANA_ANALYSIS_RESULT = WM_USER+213;
const UINT msg_DB_ADMINUSER = WM_USER+214;

const UINT msg_MAIN_QIUT = WM_USER+444;

const UINT msg_DAQ_ASD = WM_USER+500;//system detect



//----- UDP cmd from/to UI
const BYTE cmd_HEADER=0x7E;
const BYTE cmd_TAIL=0x7F;

const BYTE cmd_USERLOGIN=0x01;//in out
const BYTE cmd_USERREGISTER=0x02;//in out
const BYTE cmd_MODIFYPWD=0x03;//in out
const BYTE cmd_USERDELETE=0x04;//in out
const BYTE cmd_ADMINUSER=0x05;//in out//v1.6

const BYTE cmd_NEWPROJECT=0x11;//in out
const BYTE cmd_TERMINATEPROJECT=0x12;//in out

//��ʼ�������
const BYTE cmd_INITGRADIENT_BEGIN=0x21;//in 
const BYTE cmd_INITGRADIENT_END=0x22;//in 
const BYTE cmd_INITGRADIENT=0x21;//out

//20180607 ʵʱ������̱仯��--begin
//�������Լ����
//û�о�̬/��̬��ֻ�г�ʼ����ʼ/���� ��������
//svr-clt:��ʼʱ��ȡ��ʼ��ǣ���ʼʱ��ʼ��̬+��̬��⡣

//Automatic System Check
const BYTE cmd_ASD = 0x13;//in out

//const BYTE cmd_STILL_DETECT_BEGIN=0x23;//in 
//const BYTE cmd_STILL_DETECT_END=0x24;//in 
const BYTE cmd_STILL_DETECT=0x23;//out
//
//const BYTE cmd_MOVE_DETECT_BEGIN=0x25;//in 
//const BYTE cmd_MOVE_DETECT_END=0x26;//in
const BYTE cmd_MOVE_DETECT=0x25;//out

const BYTE cmd_BEGIN_DETECT=0x27;//in
const BYTE cmd_END_DETECT=0x28;//in


//20180607 ʵʱ������̱仯��--end


const BYTE cmd_REPORTPATH=0x41;//in 
const BYTE cmd_ANALYSIS_BEGIN=0x42;//in 
const BYTE cmd_ANALYSIS_STATE=0x41;//out
const BYTE cmd_ANALYSIS_RESULT=0x42;//out
const BYTE cmd_ANALYSIS_DATA=0x43;//out
//����û����ֹ���

const BYTE cmd_HEARTBEAT=0x51;//in out

const BYTE cmd_QUIT=0x61;//in


//DAQ controler Msg to main thread msg  
const UINT msg_DAQ_VELOCITY = WM_USER+301;
const UINT msg_DAQ_STRESS = WM_USER+302;
const UINT msg_DAQ_DATAONE = WM_USER+303;



typedef struct MoveDetectionInfo
{
	double MaxFootBrakeForce;
	double PedalDistance;
	double GradientX;
	double GradientY;
	double LastAccelaration;
	double LastVelocity;//ֻ��һ��������������һ���ٶȵ�UI
}MOVEDETECTIONINFO;
typedef struct StillDetectionInfo
{
	double MaxHandBrakeForce;
	double GradientX;
	double GradientY;
}STILLDETECTIONINFO;

typedef struct AnalysisResult
{
	double MaxAccelaration;
	double BrakeDistance;
	double AverageVelocity;
	double GradientX;//max value
	double GradientY;//max value
	double PedalDistance;//max -min
	double MaxHandBrakeForce;
	double MaxFootBrakeForce;

}ANALYSISRESULT;
typedef struct AnalysisResult_Int
{
	int MaxAccelaration;
	int BrakeDistance;
	int AverageVelocity;
	int GradientX;//max value
	int GradientY;//max value
	int PedalDistance;//max value
	int MaxHandBrakeForce;
	int MaxFootBrakeForce;

}ANALYSISRESULT_INT;
typedef struct  
{
	double Accelaration;
	double Velocity;
	double PedalDistance;
	double FootBrakeForce;
}ANALYSISDATA;
typedef struct  
{
	int Accelaration;
	int Velocity;
	int PedalDistance;
	int FootBrakeForce;
}ANALYSISDATA_INT;
const string gc_strProjectParaINI_FileName("projectparameter.ini");
const string gc_strProjectInfo("ProjectInfo");

const string gc_strInitialCarAngle("InitialCarAngle");
const string gc_strInitXAngle("InitialX");
const string gc_strInitYAngle("InitialY");

const string gc_strResult("Result");
const string gc_strMaxHandBrakeForce("MaxHandBrakeForce");
const string gc_strInitValue("InitValue");
const string gc_strInitAccA("InitAccA");//��ʼ���ٶ�ֵ
const string gc_strInitAccB("InitAccB");
const string gc_strInitAccC("InitAccC");
const string gc_strInitFootBrakeForce("InitFootBrakeForce");
const string gc_strInitHandBrakeForce("InitHandBrakeForce");

//-----------------------------for sensor config paramaters
const string gc_strSensorConfig_FileName(".\\config.ini");
const string gc_strDAQType("DAQTYPE");
const string gc_strIsWireless("Wireless");

const string gc_strParaFootBrakeForce("FootBrakeForce_Wired");
const string gc_strParaHandBrakeForce("HandBrakeForce_Wired");
const string gc_strParaXYAngle("XYAngle_Wired");
const string gc_strParaPedalDistance("PedalDistance_Wired");
const string gc_strParaAccelaration("Accelaration_Wired");
const string gc_strParaFootBrakeForceW("FootBrakeForce_Wireless");
const string gc_strParaHandBrakeForceW("HandBrakeForce_Wireless");
const string gc_strParaXYAngleW("XYAngle_Wireless");
const string gc_strParaPedalDistanceW("PedalDistance_Wireless");
const string gc_strParaAccelarationW("Accelaration_Wireless");

const string gc_strPara1("Para1");
const string gc_strPara2("Para2");
const string gc_strPara3("Para3");
const string gc_strPara4("Para4");
const string gc_strPara5("Para5");

enum enDETECTION_TYPE
{
	NONTYPE,
	INITGRADIENT,
	STILLDETECTION,
	MOVEDETECTION
};
