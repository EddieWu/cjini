#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h> 
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
  bIniExist = false;
  pJiniFile = NULL;
  JSec      = NULL;
  iSectionCount = 0;
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
  if(NULL!=cIniStr)
  {
    free(cIniStr);
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
  //ini file is empty or invalid(MIN:[S]\nK=1)
  if(lFileLen<7)
  {
    return false;
  }
  //malloc ini context size
  cIniStr = (char*)malloc(/*sizeof(char)**/lFileLen+1);
  //init
  memset(cIniStr,0x0,lFileLen+1);
  //read all context
  fread(cIniStr,lFileLen,lFileLen,pJiniFile);
#ifdef _DEBUGMODE_
  printf("cIniStr[%s]\n",cIniStr);
#endif
  //01.First collect section number
  //and each section's keys number
  char ch = 0x0;
  // section area
  int  iSecFlag = 0;
  // key value area
  int  iKeyFlag = 0;
  int  iValFlag = 0;
  //int  iSecKeys[500] = {0};
  // annotate
  //bool bAnnoLine = false;
  ch=*cIniStr;//fgetc(pJiniFile);
  while (ch != '\0')
  {
    if('[' == ch || ']' == ch)
	{
	  iSecFlag++;
	  iKeyFlag = 0;
	}
	else if('\r' == ch || '\n' == ch)
	{
	  iSecFlag = 0;
	  iValFlag = 0;
	  iKeyFlag++;
	  //bAnnoLine = false;
	  //continue;
	}
	else if('=' == ch)
	{
	  iKeyFlag = 0;
	  iValFlag++;
	}
	else if(';' == ch)
	{
	  //bAnnoLine = true;
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
    //ch=fgetc(pJiniFile);
	cIniStr++;
	ch=*cIniStr;
  }
  //rest position
  //fseek(pJiniFile,0L,SEEK_SET);
  cIniStr-=lFileLen;
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
  printf("*JSec=[%p]\n",JSec);
  printf("JSec[0].KeyNum=[%d]\n",JSec[0].KeyNum);
#endif
  //full JSec
  FullJiniHashs(iSectionCount);
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
/************************************
* FullJiniHashs()
* full the JiniHashs&JiniSection
* argument(iSectionNum:section numbers)
************************************/
bool CJini::FullJiniHashs(int iSectionNum)
{
  if(NULL==JSec ||0==lFileLen)
  {
    return false;  
  }
  char cLineStr[LINE_MAX_LENGTH] = {0};
  char *cTempStr = cIniStr;
  int  iLinePos = 0;
  int  i = 0;
  // Section index
  int iSecIndex = -1;
  // annotate
  bool bAnnoLine = false;
  while(*cTempStr!='\0')
  {
    if(*cTempStr==';')
	{
	  bAnnoLine = true;
	}
    if('\r'==*cTempStr ||'\n'==*cTempStr)
	{
	  if(iLinePos>=LINE_MIN_LENGTH)
	  {
	    if(iLinePos>LINE_MAX_LENGTH)
	    {
	      return false;
	    }
		memset(cLineStr,0x0,LINE_MAX_LENGTH);
	    strncpy(cLineStr,cIniStr,iLinePos);
		JiniSpaceTrim(cLineStr);
		//...
		{
		  int iStrLen = strlen(cLineStr);
		  if(cLineStr[0]=='[' &&cLineStr[iStrLen-1]==']')
		  {
		    iSecIndex++;
			cLineStr[0]=' ';
			//cLineStr[iStrLen-1]=' ';
			JiniSpaceTrim(cLineStr);
			strncpy(JSec[iSecIndex].SectionName,cLineStr,iStrLen-2);
			printf("JSec[%d].SectionName[%s]\n",iSecIndex,JSec[iSecIndex].SectionName);
		  }
		  else
		  {
		    if(iSecIndex>=0 && iSecIndex<iSectionNum)
			{
			  int iEqualPos = JiniChrIndex(cLineStr,'=');
#ifdef _DEBUGMODE_
              printf("iSecIndex={%d}\n",iSecIndex);
              printf("iEqualPos={%d}\n",iEqualPos);
#endif
			  if(iEqualPos>0)
			  {
			    JSec[iSecIndex].KeyNum++;
			    //if(NULL==JSec[iSecIndex].hsKey)
			    int iHashNum = JSec[iSecIndex].KeyNum;
#ifdef _DEBUGMODE_
                //printf("JSec[%d].KeyNum={%d}\n",iSecIndex,JSec[iSecIndex].KeyNum);
                //printf("iHashNum={%d}\n",iHashNum);
#endif
                if(1==iHashNum)
				{
				  JSec[iSecIndex].hsKey = (JiniHash *)malloc(sizeof(JiniHash));
				  memset(JSec[iSecIndex].hsKey,0x0,sizeof(JiniHash));
				  //printf("malloc hsKey={%p}\n",JSec[iSecIndex].hsKey);
				}
				else
				{
			      JSec[iSecIndex].hsKey = (JiniHash *)realloc(JSec[iSecIndex].hsKey,JSec[iSecIndex].KeyNum*sizeof(JiniHash));
				}
#ifdef _DEBUGMODE_
                printf("hsKey={%p}\n",JSec[iSecIndex].hsKey);
#endif
			    strncpy(JSec[iSecIndex].hsKey[iHashNum-1].JiniKey,cLineStr,iEqualPos);
				memset(cLineStr,0x20,iEqualPos+1);
				JiniSpaceTrim(cLineStr);
				strcpy(JSec[iSecIndex].hsKey[iHashNum-1].sJiniValue,cLineStr);
			  }
			}
			//iSecIndex>=iSectionNum
			else
			{
#ifdef _DEBUGMODE_
              printf("iSecIndex>=iSectionNum Crash!\n");
#endif
			  return false;
			}
		  }
          //iSecIndex		  
		}		
		cIniStr+=i;
#ifdef _DEBUGMODE_
        printf("cLineStr={%s}\n",cLineStr);
#endif
      }
	  else
	  {
	    cIniStr+=i;
	  }
	  bAnnoLine = false;
	  iLinePos  = 0;
	  i = 0;
	}
  
    if(!bAnnoLine)
	{
	  iLinePos++;
	}
	i++;
    cTempStr++;
  }
  /*
  int iSecPos = -1;
  int iKeyPos = -1;
  int iValPos = -1;
  do
  {
    if(*cIniStr=='[')
	{
	  iSecPos = 0;
	}
	else if(*cIniStr==']')
	{
	  iSecPos = 1;
	}
	else if(*cIniStr=='\r' ||*cIniStr=='\n')
	{
	  iSecPos = -1;
      iKeyPos = -1;
      iValPos = -1;
	}
    
    iSectionNum--;
  }while(iSectionNum>0);
  free(cIniStr);
  */
  JiniValFormat();
  return true;
}
/************************************
* JiniSpaceTrim()
* trim the string(char*)
* argument(int iLRALL:0:ALL,1:LEFT,
* 2:RIGHT,3:MIDDLE)
************************************/
void CJini::JiniSpaceTrim(char* cMixStr,int iLRALL)
{
#ifdef _DEBUGMODE_
  //printf("Function JiniSpaceTrim Start\n");
  //printf("cMixStr=[%s]\n",cMixStr);
#endif
  char *cTmp    = cMixStr;
  char *cTmpStd = cMixStr;
  char cMStr = *cTmp;
  char *cTmpPos = cMixStr;
  int  iSLen    = strlen(cTmp);
  bool bTrimCancel = false;
  int  iLeftPos = 0;
  int  iRightPos = 0;
  int  j = 0;
  //get space at left numbers
  //left trim&middle trim need
  if(1==iLRALL || 3==iLRALL)
  {
    while(*cTmpPos!='\0')
    {
      if(!isspace(*cTmpPos))
	  {
	    break;
	  }
	  else
	  {
	    iLeftPos++;
	  }
      cTmpPos++;
    }
  }
  //reset pointer cTmpPos
  cTmpPos = cMixStr;
  //get space at right numbers
  //right trim&middle trim need
  if(2==iLRALL || 3==iLRALL)
  {
    cTmpPos+=(iSLen-1);
    while(*cTmpPos!='\0')
    {
      if(!isspace(*cTmpPos))
	  {
	    break;
	  }
	  else
	  {
	    iRightPos++;
	  }
      cTmpPos--;
    }
  }
  while(*cTmp!='\0')
  {
    switch(iLRALL)
	{
	  //LEFT
	  case 1:
	    if(!isspace(cMStr))
		{
		  bTrimCancel = true;
		}
	    break;
	  case 2:
	    bTrimCancel = true;
		if(j>=(iSLen-iRightPos))
		{
		  bTrimCancel = false;
		}
	    break;
	  case 3:
	    bTrimCancel = false;
		//left area
		if(j<iLeftPos)
		{
		  bTrimCancel = true;
		}
		//right area
		if(j>=(iSLen-iRightPos))
		{
		  bTrimCancel = true;
		}
	    break;
	  default:
	    break;
	}
	if(bTrimCancel)
	{
	  cMStr=0x31;
	}
    if(!isspace(cMStr/**cTmp*/))
	{
	  *cTmpStd = *cTmp;
	  cTmp++;
	  cTmpStd++;
#ifdef _DEBUGMODE_
      //printf("cTmp=[%s]\n",cTmp);
      //printf("cTmpStd=[%s]\n",cTmpStd);
#endif
	}
	else
	{
	  cTmp++;
	}
	cMStr = *cTmp;
	j++;
  }
  *cTmpStd = '\0';
#ifdef _DEBUGMODE_
  //printf("cMixStr=[%s]\n",cMixStr);
  //printf("Function JiniSpaceTrim End\n");
#endif
}
/************************************
* JiniIsEnter()
* char is \r\n or not
* argument(Unchar:undertest char)
************************************/
bool CJini::JiniIsEnter(char Unchar)
{
  if(Unchar!='\r' &&Unchar!='\n')
  {
    return false;
  }
  return true;
}
/************************************
* JiniChrIndex()
* index of char in string
* argument(cStr:undertest string,cFind:
* find char)
************************************/
int CJini::JiniChrIndex(char* cStr,char cFind)
{
  int iRet = -1;
  char *sStrTmp = cStr;
  while('\0'!=*sStrTmp)
  {
    iRet++;
    if(cFind==*sStrTmp)
	{
	  break;
	}
	sStrTmp++;
  }
  if((size_t)iRet==(strlen(cStr)-1))
  {
    iRet = -1;
  }
  return iRet;
}
/************************************
* JiniValFormat()
* value integer&string transfer
* argument()
************************************/
bool CJini::JiniValFormat(void)
{
  int j = 0;
  for(;j<iSectionCount;j++)
  {
    if(JSec[j].KeyNum>0)
	{
#ifdef _DEBUGMODE_
      printf("JSec[%d].SectionName=(%s),",j,JSec[j].SectionName);
      printf("KeyNum=(%d)\n",JSec[j].KeyNum);
#endif
	  int i = 0;
	  for(;i<JSec[j].KeyNum;i++)
	  {
#ifdef _DEBUGMODE_
        printf("JSec[%d].hsKey[%d].sJiniValue=(%s),",j,i,JSec[j].hsKey[i].sJiniValue);
#endif
	    if(strlen(JSec[j].hsKey[i].sJiniValue)>0)
		{
		  JSec[j].hsKey[i].iJiniValue = atoi(JSec[j].hsKey[i].sJiniValue);
		}
#ifdef _DEBUGMODE_
        printf(".iJiniValue=(%d)\n",JSec[j].hsKey[i].iJiniValue);
#endif
	  }	
	}
  }
  return true;
}
#ifdef JINI_TEST_MAIN
#include <stdio.h>
//int main(void) {
int main(int argc,char *argv[]) {
  CJini *pCJini = new CJini();
#ifdef _DEBUGMODE__
  char cTrimStr[] = " T S  ESD.   ";
  printf("ALL\n");
  pCJini->JiniSpaceTrim(cTrimStr);
  strcpy(cTrimStr," T S  ESD.   ");
  printf("LEFT\n");
  pCJini->JiniSpaceTrim(cTrimStr,1);
  strcpy(cTrimStr," T S  ESD.   ");
  printf("RIGHT\n");
  pCJini->JiniSpaceTrim(cTrimStr,2);
  strcpy(cTrimStr," T S  E SD.   ");
  printf("MIDDLE\n");
  pCJini->JiniSpaceTrim(cTrimStr,3);
#endif
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