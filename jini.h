#ifndef __JINI_H
#define __JINI_H
#define _DEBUGMODE_
typedef struct _JiniHash
{
  char JiniKey[260];
  char sJiniValue[260];
  int  iJiniValue;
} JiniHash;
typedef struct _JiniSection
{
  int  KeyNum;
  char SectionName[260];
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
};
#endif