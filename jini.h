#ifndef __JINI_H__
#define __JINI_H__
//#define _DEBUGMODE_
//#define _DEBUGFUNCMODE_
#define _IMPORTINFO_S_
#define LINE_MAX_LENGTH (260)
#define LINE_MIN_LENGTH (3)
#define STR_MAX_LENGTH  (260)
typedef struct _JiniHash
{
  char JiniKey[STR_MAX_LENGTH];
  char sJiniValue[STR_MAX_LENGTH];
  int  iJiniValue;
} JiniHash;
typedef struct _JiniSection
{
  int  KeyNum;
  char SectionName[STR_MAX_LENGTH];
  JiniHash *hsKey;
} JiniSection;
class CJini
{
public:
  CJini(void);
  CJini(char *pIniFilePath);
  ~CJini();
private:
  //ini file pointer
  FILE *pJiniFile;
  //ini file context length
  long lFileLen;
  //ini file context string
  char *cIniStr;
  //ini file SECTION number
  int  iSectionCount;
  //ini file is exist
  bool bIniExist;
  JiniSection *JSec;
public:
  //set ini file path
  bool SetIniFilePath(char *pFilePath,bool bforce=true);
  //read all ini sections&keys
  bool ReadAllIni(void);
  //ini file is valid
  bool JiniFileValid(void);
  //full the JiniHashs
  bool FullJiniHashs(int iSectionNum=1);
  //trim function
  void JiniSpaceTrim(char* cMixStr,int iLRALL=0);
  //char is \r\n
  bool JiniIsEnter(char Unchar);
  //char is space(\t,\r,\n,\v,\f)
  bool JiniIsSpace(char Unchar);
  //index of char in string
  int  JiniChrIndex(char* cStr,char cFind);
  //value integer&string transfer
  bool JiniValFormat(void);
  //use often function
  //get key value(string)
  bool JiniGetKeyStr(char *cSec,char *cKey,char *def,char *src);
  //get key value(integer)
  int  JiniGetKeyInt(char *cSec,char *cKey,int def);
};
#endif