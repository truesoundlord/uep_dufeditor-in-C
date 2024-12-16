/* 
 * File:   uep_dufeditor.c.c
 * Author: Dimitri "Hurukan" <soundlord@gmail.com>
 *
 * Créé le December 12, 2024, 9:32 PM
 */

// ****************************************************************************
// HISTORIQUE
// ****************************************************************************

// **************************************************************************** 
// Fichiers de définitions
// **************************************************************************** 

#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>      
#include <unistd.h>
#include <string.h>

#include <wchar.h>
#include <dirent.h>

#include "uep_dufeditor.h"

#include <uepdebugtool.h>
#include <uep_wide.h>
#include <uepthreads.h>
#include <linkedlist.h>

// **************************************************************************** 
// Déclaration des constantes symboliques
// **************************************************************************** 

// **************************************************************************** 
// Déclaration des variables globales, externes, ...
// **************************************************************************** 

PositionCadre FenetrePrincipale;

LinkedList		*Parametres;

FILE					*logFile;

char					*logMessage;

// ****************************************************************************
// SECTION : prototypes des fonctions en test pour CE code source
// ****************************************************************************

unsigned short VerifyParameters(LinkedList*);
bool compareme(void*,void*);

/*
 * FONCTION PRINCIPALE
 */
int main(int argc,char** argv)
{
	logFile=InitLog("./logs","dufeditor.log",LOGW);
	InitUEPWIDE("fr_BE.UTF8");
	
	logMessage=calloc(80,sizeof(char));
	
	EffacerEcran();
	
	Parametres=lc_init();
	
	sprintf(logMessage,"uep_dufeditor STARTS -- lecture des paramètres...");
	Log(logFile,logMessage);
	
	if(argc>1)
	{
		int cpt=argc-1;
		while(cpt>=1)
		{
			lc_insert(argv[cpt],Parametres,uepuserdef,strlen(argv[cpt])+1);
			sprintf(logMessage,"Paramètre: %s",argv[cpt]);
			Log(logFile,logMessage);
			cpt--;
		}	
	}
	if(Parametres->NbElem==1)
	{
		wprintf(L"No file to process...\n");
		return(EXIT_SUCCESS);
	}
	
	sprintf(logMessage,"File(s) to process: %02d",Parametres->NbElem);
	Log(logFile,logMessage);
	
	// Verify the different parameters...
	
	// USAGE:
	// uep_dufeditor -w <working folder> -t <temporary folder> -f <filename1> <filename2>
	// uep_dufeditor -t <temporary folder> -f <filename1> <filename2>
	
	unsigned params=VerifyParameters(Parametres);
	if(params)
	{
		wprintf(L"Parameters verified:.. OK :}\n");
						
	}
	else
	{
		wprintf(L"Parameters verified... KO :{\n");
		wprintf(L"USAGE:\n");
		wprintf(L"\tuep_dufeditor -w <working directory> -t <temp directory> -f <filename1> <filename2>\n");
		wprintf(L"\tuep_dufeditor -w <working directory> -f <filename1> <filename2>\n");
		wprintf(L"\tuep_dufeditor -t <temp directory> -f <filename1> <filename2>\n");
	}
	
	// Computing params
	
	wprintf(L"[D] %4b\n",params);
	if ((params & TEMP) == TEMP)
	{
		wprintf(L"temp set... (");
		int tempID=lc_FindByValue(Parametres,"-t",compareme);
		if(tempID>1000)
		{
			lc_Datas *tempdir=lc_search(Parametres,tempID-1);				// risqué mais pas le choix
			wprintf(L"%s)\n",(char*)tempdir->value);
		}
	}
	if ((params & WORKING) == WORKING)
	{
		wprintf(L"working set... (");
		int workingID=lc_FindByValue(Parametres,"-w",compareme);
		if(workingID>1000)
		{
			lc_Datas *workingdir=lc_search(Parametres,workingID-1);				// risqué mais pas le choix
			wprintf(L"%s)\n",(char*)workingdir->value);
		}
	}
	
	wprintf(L"\n");
	return(EXIT_SUCCESS);
}

// ****************************************************************************
// SECTION : implémentation des fonctions
// ****************************************************************************

unsigned short VerifyParameters(LinkedList *param)
{
	unsigned short us_NbVerified=0;
	lc_Datas	*pExtracted=param->pHead;
	
	while(pExtracted!=NULL)
	{
		char *pUnwrap = (char*)pExtracted->value;
		
		// wprintf(L"[DEBUG] %s\n",pUnwrap);
		
		if(strcmp("-w",pUnwrap)==0)
		{
			us_NbVerified|=WORKING;
			wprintf(L"Working directory...\n");
		}
		if(strcmp("-t",pUnwrap)==0)
		{
			us_NbVerified|=TEMP;
			wprintf(L"Temp directory...\n");
		}
		if(strcmp("-f",pUnwrap)==0)
		{
			us_NbVerified|=FILES;
			pExtracted=pExtracted->pNext;											// skip -f
			
			wprintf(L"Files:\n");
			while(pExtracted != NULL)
			{
				pUnwrap=(char*)pExtracted->value;
				wprintf(L"\t%s\n",pUnwrap);
				pExtracted=pExtracted->pNext;
			}
			return us_NbVerified;
		}
		pExtracted=pExtracted->pNext;
	}
	if(us_NbVerified==0) return 0;
	return us_NbVerified;
}

bool compareme(void *candidat,void *target)
{
	char *strTarget=(char*)target;
	char *strCandidat=(char*)candidat;
	
	//wprintf(L"[D] cherche -> %s\n",strTarget);
	//wprintf(L"[D] courant -> %s\n",strCandidat);
	
	if(strcmp(strCandidat,strTarget)==0) return true;
	return false;
}
