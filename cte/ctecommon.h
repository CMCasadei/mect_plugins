#ifndef CTECOMMON_H
#define CTECOMMON_H
#include <QChar>
#include <QString>
#include "parser.h"

// Parametri generali della CT
#define MAXBLOCKSIZE 64
#define MIN_RETENTIVE 1
#define MAX_RETENTIVE 192
#define MIN_NONRETENTIVE 193
#define MAX_NONRETENTIVE 4999
#define MIN_DIAG 5000
#define MAX_DIAG 5171
#define MIN_NODE 5172
#define MAX_NODE 5299
#define MIN_SYSTEM  5390
#define COMMANDLINE 2048
#define MAX_TCPSRV_REGS 4095

// Int Constants
const int nCTCols = 13;
const int nMax_Int16 = 65535;
const int nMaxNodeID = 255;
const int nMaxRTUNodeID = 247;
const int nStartInputRegister = 300000;
const int nMaxInputRegister = nStartInputRegister + nMax_Int16;
const int nMaxRegister = 49999;
const int nCompareDecimals = 4;
const int nRowColWidth = 5;

// Priority
const int nNumPriority = 4;
const int nPriorityNone = 0;
const int nPriorityHigh = 1;
const int nPriorityMedium = 2;
const int nPriorityLow = 3;

// Gestione Server / Device / Nodi
const int nMAX_SERVERS  = 5;        // 3 RTU_SRV + 1 TCP_SRV + 1 TCPRTU_SRV (PLC in dataMain->dataNotifySet/Get)
const int nMAX_DEVICES = 16;        // 3 RTU + n TCP + m TCPRTU + 2 CANOPEN + 1 RTUSRV + 1 TCPSRV + 1 TCPRTUSRV
const int nMAX_NODES = 64;          // Numero Massimo di Nodi definiti
const int nMAX_BLOCKS = 4096;       // Numero massimo di Blocchi definiti

// Serial Ports
const int nMaxSerialPorts = 3;
// CanOpen Ports
const int nMaxCanPorts = 1;
// TCP Ports
const int nPortFTPControl = 21;
const int nPortSFTP = 22;
const int nPortTELNET = 23;
const int nPortHTTP = 80;
const int nPortVNC = 5900;


// QChar Constants
const QChar   chDOUBLEQUOTE = QChar::fromAscii(34);
const QChar   chSpace = QChar::fromAscii(20);
const QChar   chBACKSLASH = QChar::fromAscii(92);
const QChar   chX = QChar::fromAscii(88);
const QChar   chUNDERSCORE = QChar::fromAscii(95);
const QChar   chZERO = QChar::fromAscii(48);
const QChar   chOpenSQB = QChar::fromAscii(91);     // [
const QChar   chCloseSQB = QChar::fromAscii(93);    // ]


// String Costants
const QString szDEF_IP_PORT = QLatin1String("502");
const QString szEMPTY = QString::fromAscii("");
const QString szDOUBLEQUOTE = QString(1, chDOUBLEQUOTE);
const QString szUNDERSCORE = QString(1, chUNDERSCORE);
const QString szZERO = QString::fromAscii("0");
const QString szSEMICOL = QString::fromAscii(";");
const QString szSLASH = QString::fromAscii("/");
const QString szBACKSLASH = QString::fromAscii("\\");
const QString szNEWLINE = QString::fromAscii("\n");
const QString szSHARP = QString::fromAscii("#");
const QString szTRUE = QString::fromAscii("1");
const QString szFALSE = QString::fromAscii("0");
const QString szMectTitle = QString::fromAscii("Mect Editor");
const QString szPathIMG = QString::fromAscii(":/cteicons/img/");

// Constanti per gestione XML di transito tra variabili e lettura Modello MPNx
const QString szXMLCTENAME = QString::fromAscii("Mect_CTE");
const QString szXMLCTEVERSION = QString::fromAscii("Version");
const QString szXMLMODELTAG = QString::fromAscii("Model");
const QString szXMLCTNUMROWS = QString::fromAscii("CT_Rows");
const QString szXMLCTROW = QString::fromAscii("Crosstable_Row");
const QString szXMLCTDESTROW = QString::fromAscii("DestRow");
const QString szXMLCTSOURCEROW = QString::fromAscii("SourceRow");
const QString szXMLExt = QString::fromAscii(".xml");

// External Models
const QString szMODELSPATH =  QString::fromAscii(":/ctexml/models/");
const QString szMPNC006 = QString::fromAscii("MPNC006");
const QString szMPNE10 = QString::fromAscii("MPNE10");
const QString szTPLC050 = QString::fromAscii("TPLC050");

enum colonne_e
{
    colPriority = 0,
    colUpdate,
    colGroup,
    colModule,
    colName,
    colType,
    colDecimal,
    colProtocol,
    colIP,
    colPort,
    colNodeID,
    colInputReg,
    colRegister,
    colBlock,
    colBlockSize,
    colBehavior,
    colComment,
    colSourceVar,
    colCondition,
    colCompare,
    colTotals
};

enum colonne_MPNx  {
    colMPNxPriority = 0,
    colMPNxUpdate,
    colMPNxGroup,
    colMPNxModule,
    colMPNxName,
    colMPNxType,
    colMPNxDecimal,
    colMPNxNodeID,
    colMPNxRegister,
    colMPNxBlock,
    colMPNxBlockSize,
    colMPNxBehavior,
    colMPNxComment,
    colMPNxService,
    colMPNxTotals
};

enum modbusserver_e
{
    srvRTU0 = 0,
    srvRTU1,
    srvRTU2,
    srvRTU3,
    srvTCP,
    srvTCPRTU,
    srvTotals
};

enum regions_e
{
    regRetentive = 0,
    regNonRetentive,
    regDiagnostic,
    regLocalIO,
    regSystem,
    regTotals
};

enum showModes
{
    showHead = 0,
    showUsed,
    showAll,
    showTotals
};

enum colTree_e
{
    colTreeName = 0,
    colTreeInfo,
    colTreeTimings,
    colTreeTotals
};
enum treeRoles
{
    treeRoot = 1000,
    treeDevice,
    treeNode,
    treePriority,
    treeBlock,
    treeVariable
};

struct  extendedCT : CrossTableRecord {
    int     nGroup;
    int     nModule;
};


struct  MODBUS_Srv  {
    QString     szIpAddress;
    int         nProtocol;
    int         nPort;
    int         nodeId;
};

struct  TP_Config {
    // General Params
    QString     modelName;
    int         nModel;
    // Structural Parameters
    int         displayWidth;
    int         displayHeight;
    int         usbPorts;
    int         ethPorts;
    int         sdCards;
    int         nEncoders;
    int         digitalIN;
    int         digitalOUT;
    int         analogIN;
    int         analogINrowCT;
    int         analogOUT;
    int         analogOUTrowCT;
    bool        tAmbient;
    bool        audioIF;
    int         rpmPorts;
    // Config Parameters
    int         retries;
    int         blacklist;
    int         readPeriod1;
    int         readPeriod2;
    int         readPeriod3;
    int         fastLogPeriod;
    int         slowLogPeriod;
    // Bus Interfaces
    // Serial 0
    bool        ser0_Enabled;
    int         ser0_BaudRate;
    QString     ser0_Parity;
    int         ser0_DataBits;
    int         ser0_StopBits;
    int         ser0_TimeOut;
    int         ser0_Silence;
    int         ser0_BlockSize;
    // Serial 1
    bool        ser1_Enabled;
    int         ser1_BaudRate;
    QString     ser1_Parity;
    int         ser1_DataBits;
    int         ser1_StopBits;
    int         ser1_TimeOut;
    int         ser1_Silence;
    int         ser1_BlockSize;
    // Serial 2
    bool        ser2_Enabled;
    int         ser2_BaudRate;
    QString     ser2_Parity;
    int         ser2_DataBits;
    int         ser2_StopBits;
    int         ser2_TimeOut;
    int         ser2_Silence;
    int         ser2_BlockSize;
    // Serial 3
    bool        ser3_Enabled;
    int         ser3_BaudRate;
    QString     ser3_Parity;
    int         ser3_DataBits;
    int         ser3_StopBits;
    int         ser3_TimeOut;
    int         ser3_Silence;
    int         ser3_BlockSize;
    // TCP
    int         tcp_TimeOut;
    int         tcp_Silence;
    int         tcp_BlockSize;
    // Can0
    bool        can0_Enabled;
    int         can0_BaudRate;
    int         can0_BlockSize;
    // Can1
    bool        can1_Enabled;
    int         can1_BaudRate;
    int         can1_BlockSize;
};
// Struttura per Servers
struct  serverStruct {
    QString     szServerName;
    int         nProtocol;
    unsigned    IPAddress;
    QString     szIpAddress;
    int         nPort;
    int         NodeId;
    int         nVars;
};
// Struttura per Devices
struct  deviceStruct {
    QString     szDeviceName;
    int         nServer;
    int         nProtocol;
    unsigned    IPAddress;
    QString     szIpAddress;
    int         nPort;
    int         nMaxBlockSize;
    int         nBaudRate;
    int         nDataBits;
    int         nStopBits;
    int         nSilence;
    int         nTimeOut;
    double      dCharTime;
    double      dMinSilence;
    int         nVars;
    int         nDeviceReadTime[nNumPriority];
    int         diagnosticAddr;
    QString     diagnosticVarName;
};
// Struttura per Nodi
struct  nodeStruct {
    QString     szNodeName;
    int         nDevice;
    int         nNodeId;
    int         nVars;
    int         nNodeReadTime[nNumPriority];
    int         diagnosticAddr;
    QString     diagnosticVarName;
};
// Struttura per Blocchi
struct blockStruct  {
    int         nBlockId;
    int         nPriority;
    int         nDevice;
    int         nNode;
    int         nBlockSize;
    int         nProtocol;
    int         nRegisters;
    int         nByteSize;
    int         nReadTime_ms;
};

#endif // CTECOMMON_H
