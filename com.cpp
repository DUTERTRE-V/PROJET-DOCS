//---------------------------------------------------------------------------

#pragma hdrstop

#include "com.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)



#include <string.h>
#include <iostream>
#include <tchar.h>

void errCom::setMsg(char* message)
{
    if (msg!=NULL) delete (msg); //destruction de l'ancienne variable
    msg=new char[strlen(message)+1];//creation de la variable msg
    strcpy(msg,message);
};

errCom::errCom()
{
    msg=NULL;
    setMsg("erreur non définie");
};

errCom::errCom(char* message)
{
    msg=NULL;
    setMsg(message);
};

errCom::~errCom()
{
    if (msg!=NULL) delete (msg);
};

const char* errCom::quoi(){
    return msg;
};

errCom::errCom(const errCom& cpy)
{
    msg=NULL;
    setMsg(cpy.msg);
};

com::com(int numPort,int vitesse)
{
    g_hCOM=NULL;

    g_cto.ReadIntervalTimeout = MAX_WAIT_READ;
    g_cto.ReadTotalTimeoutMultiplier=0;
    g_cto.ReadTotalTimeoutConstant=MAX_WAIT_READ;
    g_cto.WriteTotalTimeoutMultiplier=0;
    g_cto.WriteTotalTimeoutConstant=0;

    g_dcb.BaudRate=vitesse;//9600
    g_dcb.ByteSize=8;
    g_dcb.DCBlength=sizeof(DCB);
    g_dcb.EofChar=0x1A;
    g_dcb.ErrorChar='?';
    g_dcb.EvtChar=0x10;
    g_dcb.fAbortOnError=FALSE;
    g_dcb.fBinary=TRUE;
    g_dcb.fDsrSensitivity=FALSE;
    g_dcb.fDtrControl=DTR_CONTROL_ENABLE;
    g_dcb.fDummy2=0;
	g_dcb.fErrorChar=FALSE;
    g_dcb.fInX=FALSE;
    g_dcb.fNull=FALSE;
    g_dcb.fOutX=FALSE;
    g_dcb.fOutxCtsFlow=FALSE;
    g_dcb.fOutxDsrFlow=FALSE;
    g_dcb.fParity=FALSE;
    g_dcb.fRtsControl=RTS_CONTROL_ENABLE;
    g_dcb.fTXContinueOnXoff=FALSE;
    g_dcb.Parity=NOPARITY;
    g_dcb.StopBits=ONESTOPBIT;
    g_dcb.wReserved=0;
    g_dcb.wReserved1=0;//n'était pas définit ???
    g_dcb.XoffChar=0x13;
    g_dcb.XoffLim=0x100;
    g_dcb.XonChar=0x11;
    g_dcb.XonLim=0x100;

    if(!OpenCOM(numPort)) throw errCom("Erreur inconnue lors de l'ouverture");
}

com::~com()
{
    CloseCOM();
}

int com::ByteToRead()
{
    COMSTAT cs = {0};
    DWORD dwErrors;
    ClearCommError(g_hCOM, &dwErrors, &cs);
    return cs.cbInQue;
}

BOOL com::OpenCOM    (int nId)
{
    /* variables locales */
    char szCOM[16];

    /* construction du nom du port, tentative d'ouverture */
    sprintf(szCOM, "\\\\.\\COM%d", nId);//modifie pour prendre les port com>9
	g_hCOM = CreateFile(szCOM, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
    if(g_hCOM == INVALID_HANDLE_VALUE)
    {
        throw errCom("Erreur lors de l'ouverture du port com");
        return FALSE;
    }

    /* affectation taille des tampons d'émission et de réception */
    SetupComm(g_hCOM, RX_SIZE, TX_SIZE);

    /* configuration du port COM */
    if(!SetCommTimeouts(g_hCOM, &g_cto) || !SetCommState(g_hCOM, &g_dcb))
    {
        throw errCom("Erreur lors de la configuration du port com");
        CloseHandle(g_hCOM);
        return FALSE;
    }

    /* on vide les tampons d'émission et de réception, mise à 1 DTR */
    PurgeComm(g_hCOM, PURGE_TXCLEAR|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_RXABORT);
    EscapeCommFunction(g_hCOM, SETDTR);
    return TRUE;
}

BOOL com::CloseCOM   ()
{
    /* fermeture du port COM */
    CloseHandle(g_hCOM);
    return TRUE;
}

BOOL com::ReadCOM    (void* buffer, int nBytesToRead, int* pBytesRead)
{
    return ReadFile(g_hCOM, buffer, nBytesToRead, (unsigned long *)pBytesRead, NULL);
}

BOOL com::WriteCOM   (void* buffer, int nBytesToWrite, int* pBytesWritten)
{
    /* écriture sur le port */
    return WriteFile(g_hCOM, buffer, nBytesToWrite, (unsigned long *)pBytesWritten, NULL);
}