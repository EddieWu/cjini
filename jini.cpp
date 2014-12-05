#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "jini.h"

CJini::CJini(void)
{
  bIniExist = false;
  pJiniFile = NULL;
  JSec      = NULL;
  iSectionCount = 0;
  printf("CJini()\n");	
}
CJini::CJini(char *pIniFilePath)
{
  printf("CJini(char*)\n");
  //"rb:read+binary",read only
  pJiniFile=fopen(pIniFilePath,"rb");
  if(/*'\0'*/NULL==pJiniFile)
  {
    bIniExist = false;
    printf("fopen fail[%s]\n",pIniFilePath);
  }
  else
  {
    bIniExist = true;
	printf("fopen success[%s]\n",pIniFilePath);
  }
  if(bIniExist && !fclose(pJiniFile))
  {
    pJiniFile=fopen(pIniFilePath,"rb+");
  }
#ifdef _DEBUGMODE_
  char ch = 0x0;
  ch=fgetc(pJiniFile);
  printf("---------------------------\n%c",ch);
  while (ch != EOF)
  {
    ch=fgetc(pJiniFile);
	printf("%c",ch);
  }
  //rest position
  fseek(pJiniFile,0L,SEEK_SET);
  printf("\n---------------------------\n");
#endif
  ReadAllIni();
}
CJini::~CJini()
{
  if(NULL!=pJiniFile)
  {
    fclose(pJiniFile);
  }
  if(NULL!=JSec)
  {
    free(JSec);
  }
  printf("~CJini()\n");
}
/************************************
* SetIniFilePath(bool bforce)
* set ini file path,CJini() first
* argument(bforce:pJiniFile changed
* force)
************************************/
bool CJini::SetIniFilePath(char *pFilePath,bool bforce)
{
  if(bforce)
  {
    if(NULL!=pJiniFile)
	{
      fclose(pJiniFile);
	  //pJiniFile=NULL;
	}
	pJiniFile=fopen(pFilePath,"rb+");
    return(pJiniFile==NULL?false:true);
  }
  return true;
}
/************************************
* ReadAllIni()
* read ini file and fixed JiniSection
* argument...
************************************/
bool CJini::ReadAllIni(void)
{
  if(NULL==pJiniFile)
  {
    return false;
  }
  //00.Get the file context's size
  fseek(pJiniFile,0L,SEEK_END);
  lFileLen = ftell(pJiniFile);
#ifdef _DEBUGMODE_
  printf("lFileLen[%ld]\n",lFileLen);
#endif
  //rest position
  fseek(pJiniFile,0L,SEEK_SET);
  //01.First collect section number
  //and each section's keys number
  char ch = 0x0;
  // section area
  int  iSecFlag = 0;
  // key value area
  int  iKeyFlag = 0;
  int  iKeyValFlag = 0;
  // annotate
  bool bAnnoLine = false;
  ch=fgetc(pJiniFile);
  while (ch != EOF)
  {
    if('[' == ch || ']' == ch)
	{
	  iSecFlag++;
	  iKeyFlag = 0;
	}
	else if('\r' == ch || '\n' == ch)
	{
	  iSecFlag = 0;
	  iKeyValFlag = 0;
	  iKeyFlag++;
	  //continue;
	}
	else if('=' == ch)
	{
	  iKeyFlag = 0;
	  iKeyValFlag++;
	}
	else
	{
	  iSecFlag++;
	}
	//section count
	if(iSecFlag>2 && ']' == ch)
	{
	  iSectionCount++;  //+1
	  iSecFlag = 0;
	}	
    ch=fgetc(pJiniFile);
  }
#ifdef _DEBUGMODE_
  printf("SectionNumber[%d]\n",iSectionCount);
#endif
  //02.Struct malloc
  if(iSectionCount>0)
  {
    JSec = (JiniSection*)malloc(sizeof(JiniSection)*iSectionCount);
    memset(JSec,0x0,sizeof(JiniSection)*iSectionCount);
  }
  else
  {
    JSec = NULL;
    return false;
  }
#ifdef _DEBUGMODE_
  printf("JSec[0].KeyNum=[%d]\n",JSec[0].KeyNum);
#endif
  return true;
}
/************************************
* JiniFileValid()
* Jini file is valid or not
* argument...
************************************/
bool CJini::JiniFileValid(void)
{
  return bIniExist;
}

#ifdef JINI_TEST_MAIN
#include <stdio.h>
//int main(void) {
int main(int argc,char *argv[]) {
  CJini *pCJini = new CJini();
  delete pCJini;
  char sFilePath[] = "C:\\test.txt";
  CJini *pCJiniF = new CJini(sFilePath);
  delete pCJiniF;
  printf("argc[%d]\n", argc);
  for(int i=0;i<argc;i++)
  {
	printf("argv[%d]=[%s]\n", i,argv[i]);
  }
  return(0);
}
#endif 