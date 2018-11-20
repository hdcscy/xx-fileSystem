/*
                   _ooOoo_
                  o8888888o
                  88" . "88
                  (| -_- |)
                  O\  =  /O
               ____/`---'\____
             .'  \\|     |//  `.
            /  \\|||  :  |||//  \
           /  _||||| -:- |||||-  \
           |   | \\\  -  /// |   |
           | \_|  ''\---/''  |   |
           \  .-\__  `-`  ___/-. /
         ___`. .'  /--.--\  `. . __
      ."" '<  `.___\_<|>_/___.'  >'"".
     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
     \  \ `-.   \_ __\ /__ _/   .-` /  /
======`-.____`-.___\_____/___.-`____.-'======
                   `=---='
^^^^^^��������^^^^^^^^^^^^^^^^^^^����BUG^^^^^
*/

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#pragma warning(disable:4996)

#define DATA_FILE_SIZE 1024*5120
#define INDEX_FILE_SIZE 4*1024*5120

//ö�ٲ�������
typedef enum { 
  false, 
  true 
} bool;

typedef struct {
  int nNameOffset;    //����ƫ��
  size_t stNameLen;   //���ֳ���
  int nPhoneOffset;   //����ƫ��
  size_t stPhoneLen;  //���볤��
} tagAccount;

FILE *fpData;
FILE *fpIndex;

char g_arycBuf[0x5000];            //����ȫ��һά����
char p_arycBuf[0x5000 * sizeof(tagAccount) + 4];      //����ȫ��һάָ������
char *p_arySaveAddress[0x5000];   //���������ַ
tagAccount *g_pAccount = (tagAccount *)(p_arycBuf + 4);

void funcPrintMenu(); //�����溯��

//��ʾ��ϵ����Ϣ����
void funcShowInfo() {
  int nLen = *(int *)p_arycBuf;
  int i = 0;
  for (i = 0; i < nLen; i++) {
    char szName[100] = "";
    char szPhone[100] = "";

    memcpy(szName, 4 + g_pAccount[i].nNameOffset + g_arycBuf, g_pAccount[i].stNameLen);
    memcpy(szPhone, 4 + g_pAccount[i].nPhoneOffset + g_arycBuf, g_pAccount[i].stPhoneLen);

    printf("%d. %s %s\r\n", i + 1, szName, szPhone);
  }
}

//ˢ�������ļ�����
bool funcFlushData() {  
  fseek(fpData, 0, SEEK_SET);
  if (fwrite(g_arycBuf, sizeof(g_arycBuf), 1, fpData) == 0) {
    return false;
  }
  fflush(fpData);
  return true;
}

//������ݡ�����ƫ�ƺ���
int funcAddDataRetnOffset(void *szStr, int nSize) {
  //����βָ��
  int nTailOffset = *(int *)g_arycBuf;
  int nTemp = nTailOffset;

  //�����ڴ�
  memcpy(g_arycBuf + nTailOffset + 4, szStr, nSize);
  nTailOffset = nTailOffset + nSize;

  //ˢ���ڴ�ǰ���ֽڣ�βָ��
  *(int *)g_arycBuf = nTailOffset;

  //ˢ�µ��ļ�
  if (funcFlushData() == false) {
    return -1;
  }

  return nTemp;
}

//ˢ�������ļ�����
bool funcFlushIndex() {
  fseek(fpIndex, 0, SEEK_SET);
  if (fwrite(p_arycBuf, sizeof(p_arycBuf), 1, fpIndex) == 0) {
    return false;
  }
  fflush(fpIndex);
  return true;
}

//�����������
bool funcAddIndex(tagAccount *tagTemp) {
  //����index������
  int nIndexCount = *(int *)p_arycBuf;
  g_pAccount[nIndexCount] = *tagTemp;
  nIndexCount++;

  //��д�ڴ��еļ���
  *(int *)p_arycBuf = nIndexCount;

  //ˢ�µ��ļ�
  if (funcFlushIndex() == false) {
    return false;
  }

  return true;
}

//ɾ����������
bool funcDelIndex(int nInputIndex) {
  int i = 0;
  int nIndexLen = *(int *)p_arycBuf;

  //����������С�ڻ��ߵ���0
  if (nInputIndex < 0) {
    return false;
  }

  for (i = nInputIndex; i < nIndexLen; i++) {
    g_pAccount[i] = g_pAccount[i + 1];
  }

  nIndexLen--;
  *(int *)p_arycBuf = nIndexLen;

  //ˢ�µ��ļ�
  if (funcFlushIndex() == false) {
    return false;
  }

  return true;
}


/*======================�߼���=====================*/
/*=================================================*/
/*======================ҵ���=====================*/


//�����ϵ�˺���
void funcAddData() {
  char ch = '0';                //�����ַ�
  char sTempStr[60] = "\0"; //�ֲ���ʱ�ַ�����
  int nCount = 0;               //���ڼ����ַ�����
  int i = 0;                    //����ѭ��

  system("cls");

  //��������
  printf("Please input a name:");
  scanf("%100s", sTempStr);
  
  tagAccount tagTemp = { 0 };

  int nNameLen = strlen(sTempStr) + 1;
  int nNameOffset = funcAddDataRetnOffset(sTempStr, nNameLen);

  if (nNameOffset == -1) {
    puts("Add error!");
    getch();
    return;
  }

  tagTemp.nNameOffset = nNameOffset;
  tagTemp.stNameLen = nNameLen;

  //����绰
  printf("Please input the phone:");
  scanf("%100s", sTempStr);

  int nPhoneLen = strlen(sTempStr) + 1;
  int nPhoneOffset = funcAddDataRetnOffset(sTempStr, nPhoneLen);

  if (nPhoneOffset == -1) {
    puts("Add error!");
    getch();
    return;
  }

  tagTemp.nPhoneOffset = nPhoneOffset;
  tagTemp.stPhoneLen = nPhoneLen;

  //��ӵ�����
  funcAddIndex(&tagTemp);

  //��ӡ��ϵ�˺���
  printf("\r\n");
  funcShowInfo();

  puts("\r\nCompleted! input Q return main menu.");

  while (ch != 'q' && ch != 'Q') {
    ch = getch();
  }

  funcPrintMenu();
}

//ɾ����ϵ�˺���
void funcDelData() {
  char ch = '0';
  int nInput = 0;     //ת���ַ�Ϊ����
  int nCount = 0;
  char sTempStr[12] = "\0";
  int i = 0;
  
  system("cls");
  funcShowInfo();
  printf("\r\nPlease select a number:");

  scanf("%d", &nInput);
  
  int nIndexLen = *(int *)p_arycBuf;
  
  if (nInput <= 0 || nInput > nIndexLen) {
    printf("Error! You input number more than max."); 
  } else {
    funcDelIndex(nInput - 1);
  }

  system("cls");

  funcShowInfo();
  
  puts("\r\nCompleted! input Q return main menu.");

  while (ch != 'q' && ch != 'Q') {
    ch = getch();
  }

  funcPrintMenu();
}

//�޸���ϵ�˺���
void funcMdfData() {
  char ch = '0';
  char szInputName[100] = "";
  char szInputPhone[100] = "";
  int nInput = 0;

  system("cls");
  funcShowInfo();
  printf("\r\nPlease select a number:");

  scanf("%d", &nInput);

  int nIndexLen = *(int *)p_arycBuf;

  if (nInput <= 0 || nInput > nIndexLen) {
    printf("Error! You input number more than max.");
  } else {
    //��������
    printf("Please input a name:");
    scanf("%100s", szInputName);

    //����绰
    printf("Please input the phone:");
    scanf("%100s", szInputPhone);

    //�ж�
    tagAccount tagTemp = { 0 };
    tagTemp = g_pAccount[nInput - 1];

    int nNameLen = strlen(szInputName) + 1;
    int nPhoneLen = strlen(szInputPhone) + 1;

    int nNewDataLen = nNameLen + nPhoneLen;
    int nOldDataLen = tagTemp.stNameLen + tagTemp.stPhoneLen;

    tagTemp.stNameLen = nNameLen;
    tagTemp.stPhoneLen = nPhoneLen;

    if (nNewDataLen > nOldDataLen) {
      //����µ���Ϣ���ھɵ���Ϣ������ӵ�ĩβ
      int nOffset = funcAddDataRetnOffset(szInputName, nNameLen);
      tagTemp.nNameOffset = nOffset;
      nOffset = funcAddDataRetnOffset(szInputPhone, nNameLen);
      tagTemp.nPhoneOffset = nOffset;  
    } else {
      //���ϵط����
      memcpy(g_arycBuf + 4 + tagTemp.nNameOffset , szInputName, nNameLen);
      memcpy(g_arycBuf + 4 + tagTemp.nPhoneOffset, szInputPhone, nPhoneLen);
    }

    g_pAccount[nInput - 1] = tagTemp;

    if (funcFlushData() == false) {
      printf("Change data error!");
      return;
    }

    if (funcFlushIndex() == false) {
      printf("Change data error!");
      return;
    }
 
  }

  system("cls");
  funcShowInfo();
  puts("\r\nCompleted! input Q return main menu.");
  
  while (ch != 'q' && ch != 'Q') {
    ch = getch();
  }

  funcPrintMenu();
}

//��ѯ��ϵ�˺���
void funcFindData() {  //��ѯ�ַ�������
  char ch = '0';
  int nInput = 0;
  int nFindIndex = 0;
  int i = 0;
  char szName[100] = "";
  char szPhone[100] = "";
  char szFuzzy[100] = "";

  system("cls");
  puts("1.Find by name.");
  puts("2.Find by phone.");
  puts("3.Find by index.");
  puts("4.Fuzzy search.");
  printf("\r\nPlease select a number:");

  int nIndexLen = *(int *)p_arycBuf;
  scanf("%d", &nInput);
  
  switch(nInput) {
    case 1:
      printf("Please input account name:");
      scanf("%100s", szName);
      for (i = 0; i < nIndexLen; i++) {
        if (strcmp(szName, 4 + g_pAccount[i].nNameOffset + g_arycBuf) == 0) {
          printf("\r\n%d. %s %s", i + 1, szName, 4 + g_pAccount[i].nPhoneOffset + g_arycBuf);
        }
      }
      break;
    
    case 2:
      printf("Please input account phone:");
      scanf("%100s", szPhone);
      for (i = 0; i < nIndexLen; i++) {
        if (strcmp(szPhone, 4 + g_pAccount[i].nPhoneOffset + g_arycBuf) == 0) {
          printf("\r\n%d. %s %s", i + 1, 4 + g_pAccount[i].nNameOffset + g_arycBuf, szPhone);
        }
      }
      break;
    
    case 3:
      printf("Please input account index:");
      scanf("%d", &nFindIndex);
      if (nFindIndex <= 0 || nFindIndex > nIndexLen) {
        printf("Input error!");
      } else {
        printf("\r\n%d. %s %s", nFindIndex, 4 + g_pAccount[nFindIndex - 1].nNameOffset + g_arycBuf, 4 + g_pAccount[nFindIndex - 1].nPhoneOffset + g_arycBuf);
      }
      break;
      
    case 4:
      printf("Please input fuzzy string:");
      scanf("%s", szFuzzy);
      for (i = 0; i < nIndexLen; i++) {
        if (strstr(4 + g_pAccount[i].nNameOffset + g_arycBuf, szFuzzy) != NULL) {
          printf("\r\n%d. %s %s", i + 1, 4 + g_pAccount[i].nNameOffset + g_arycBuf, 4 + g_pAccount[i].nPhoneOffset + g_arycBuf);
        } else if (strstr(4 + g_pAccount[i].nPhoneOffset + g_arycBuf, szFuzzy) != NULL) {
          printf("\r\n%d. %s %s", i + 1, 4 + g_pAccount[i].nNameOffset + g_arycBuf, 4 + g_pAccount[i].nPhoneOffset + g_arycBuf);
        }
      }
      break;
      
    default:
      printf("\r\n\r\nInput error!");
      break;
  }    

  puts("\r\n\r\nCompleted! input Q return main menu.");
  
  while (ch != 'q' && ch != 'Q') {
    ch = getch();
  }

  funcPrintMenu();
}

//ͳ����ϵ�˺���
void funcStatisData() {
  char ch = '0';
  int nIndexLen = *(int *)p_arycBuf;
  int i = 0;
  int j = 0;
  int k = 0;
  
  char szFqc[64] = "";
  char szWord[64] = "abcdefghijklmnopqrstuvwsyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  
  system("cls");
  
  for (i = 0; i < nIndexLen; i++) {
    for (j = 0; j < strlen(4 + g_pAccount[i].nNameOffset + g_arycBuf); j++) {
      for (k = 0; k < strlen(szWord); k++) {
        if ((4 + g_pAccount[i].nNameOffset + g_arycBuf)[j] == szWord[k]) {
          szFqc[k]++;
        }
      }
    }
  }
  
  for (i = 0; i < strlen(szWord); i++) {
    printf("%c: %d\r\n", szWord[i], szFqc[i]);
  }

  puts("\r\nCompleted! input Q return main menu.");
  
  while (ch != 'q' && ch != 'Q') {
    ch = getch();
  }

  funcPrintMenu();
}

//�洢��Ϣ����
void funcStorageInfo() {
  char ch = '0';
  char ch2 = '0';
  int i = 0;          //����ѭ��
  int j = 0;
  tagAccount tagTempSort = { 0 };
  
  int nIndexLen = *(int *)p_arycBuf;
  bool bFlag = false;
  
  system("cls");
  
  for (i = 0; i < 1000; i++) {
    for (j = 0; j < nIndexLen; j++) {
      tagAccount tagTemp = g_pAccount[j];  
      if (i >= tagTemp.nNameOffset && i <= (tagTemp.nNameOffset + tagTemp.stNameLen + tagTemp.stPhoneLen)) {
        bFlag = true;
        break;
      } else {
        bFlag = false;
      }
    }
    if (bFlag == false) {
      printf("F ");
    } else {
      printf("U ");
    }
  }
  
  puts("\r\n\r\nCompleted! input Q return main menu.\r\n");
  puts("Do you want clean up to file?(Y or Other)");
  
  ch2 = getch();
  if (ch2 == 'Y' || ch2 == 'y') {
    printf("Cleaning up...");
    //�������ļ���ƫ������������
    for (i = 0; i < nIndexLen; i++) {
      for (j = 1; j < nIndexLen - i; j++) {
        if (g_pAccount[j].nNameOffset < g_pAccount[j - 1].nNameOffset) {
          tagTempSort = g_pAccount[j];
          g_pAccount[j] = g_pAccount[j - 1];
          g_pAccount[j - 1] = tagTempSort;
        }
      }
    }
    
    //���е�ַ�Ա�
    for (i = 0; i < nIndexLen; i++) {
      tagAccount tagTemp2 = g_pAccount[i];
      if (i == 0) {
        if (tagTemp2.nNameOffset != 0) {
          memcpy(g_arycBuf + 0 , g_arycBuf + tagTemp2.nNameOffset, tagTemp2.stNameLen + tagTemp2.stPhoneLen);
          tagTemp2.nNameOffset = 0;
          tagTemp2.nPhoneOffset = tagTemp2.nNameOffset + tagTemp2.stNameLen;
          g_pAccount[0] = tagTemp2;
          funcFlushData();
          funcFlushIndex();
        }
      } else {
        tagAccount tagTemp3 = g_pAccount[i - 1];
        int nTemp2 = tagTemp3.nNameOffset + tagTemp3.stNameLen + tagTemp3.stPhoneLen;
        int nSpace = tagTemp2.nNameOffset - nTemp2;
        if (nSpace > 0) {
          memcpy(g_arycBuf + nTemp2, g_arycBuf + tagTemp2.nNameOffset, tagTemp2.stNameLen + tagTemp2.stPhoneLen);
          tagTemp2.nNameOffset = nTemp2;
          tagTemp2.nPhoneOffset = tagTemp2.nNameOffset + tagTemp2.stNameLen;
          g_pAccount[i] = tagTemp2;
          funcFlushData();
          funcFlushIndex();
        }
      }
    }
    //�޸�βָ��
    *(int *)g_arycBuf = g_pAccount[nIndexLen - 1].nNameOffset + g_pAccount[nIndexLen - 1].stNameLen + g_pAccount[nIndexLen - 1].stPhoneLen + 1;
    *(char *)(g_arycBuf + 4 + *(int *)g_arycBuf - 1) = 0x00;
    funcFlushData();
  } else {
    printf("You cancel clean up!");
  }
  
  puts("\r\nCompleted! input Q return main menu.");
  while (ch != 'q' && ch != 'Q') {
    ch = getch();
  }
  funcPrintMenu();
}

//��ӡ�˵�����
void funcPrintMenu() {  //�����溯��
  char ch = '0';
  system("cls");

  puts("1.Add a account.");
  puts("2.Delete a account.");
  puts("3.Modify a account.");
  puts("4.Search a account.");
  puts("5.Frequency tatistics table viewer.");
  puts("6.Storage resource distribution table viewer.");
  puts("7.Exit.\r\n");
  printf("Pleast select:");

  ch = getch();

  switch(ch) {
    case '1':
      funcAddData();
      break;
    case '2':
      funcDelData();
      break;
    case '3':
      funcMdfData();
      break;
    case '4':
      funcFindData();
      break;
    case '5':
      funcStatisData();
      break;
    case '6':
      funcStorageInfo();
      break;
    case '7':
      printf("\r\n");
      return;
    default:
      puts("\r\nInput error!");
  }
}

//����ƫ�������ļ��豸
bool boolCreateIndex() {
  int i = 0;
  FILE *fp = fopen("index.dat", "wb+");
  if (fp == NULL) {
    return false;
  }

  for (i = 0; i < INDEX_FILE_SIZE; i++) {
    fputc(0, fp);
  }

  if (NULL != fp) {
    fclose(fp);
  }

  return true;
}

//���������ļ�
bool boolCreateData() {
  int i = 0;
  int nT = 0;

  FILE *fp = fopen("data.dat", "wb+");

  if (fp == NULL) {
    return false;
  }

  for (i = 0; i < DATA_FILE_SIZE; i++) {
    fputc(-1, fp);
  }

  fseek(fp, 0, SEEK_SET);
  if (fwrite(&nT, sizeof(nT), 1, fp) == 0) {
    return false;
  }

  if (NULL != fp) {
    fclose(fp);
  }

  return true;
}

//��ʼ���ļ�
bool boolInitFile() {
  //��ƫ�������ļ�
  fpIndex = fopen("index.dat", "r+");
  if (NULL == fpIndex) {
  //ʧ�ܱ�ʾ�ļ������� �򴴽��ļ�
    puts("Not found data offset file, auto add.");
    char ch = 0;
    if (false == boolCreateIndex()) {
      puts("Create file is fail,program quit!");
      return false;
    } else {
      fpIndex = fopen("index.dat", "rb+");
    }
  }

	//�������ļ�
  fpData = fopen("data.dat", "r+");
  if (NULL == fpData) {
  //ʧ�ܱ�ʾ�ļ������� �򴴽��ļ�
    puts("Not found data offset file, auto add.");
    char ch = 0;
    if (false == boolCreateData()) {
      puts("Create file is fail,program quit!");
      return false;
    } else {
      fpData = fopen("data.dat", "rb+");
    }
  }

  fseek(fpIndex, 0, SEEK_SET);
  if (fread(p_arycBuf, sizeof(p_arycBuf), 1, fpIndex) == 0) {
    return false;
  }

  fseek(fpData, 0, SEEK_SET);
  if (fread(g_arycBuf, sizeof(g_arycBuf), 1, fpData) == 0) {
    return false;
  }

  return true;
}

//�ر��ļ�����
void funcCloseFile() {
  fclose(fpData);
  fclose(fpIndex);
}

int main() {
  if (false == boolInitFile()) {
    puts("Init error!");
  } else {
    funcPrintMenu();
  }
  funcCloseFile();
  system("pause");
  return 0;
}