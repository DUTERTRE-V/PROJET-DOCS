//---------------------------------------------------------------------------

#ifndef comH
#define comH
//---------------------------------------------------------------------------


#include <fstream>//pour disposer des flux ofstream et compagnie
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <tchar.h>
#include <fileapi.h>
#include <iostream>


using namespace std;


#define RX_SIZE         4096    /* taille tampon d'entrée  */
#define TX_SIZE         4096    /* taille tampon de sortie */
#define MAX_WAIT_READ   5000    /* temps max d'attente pour lecture (en ms) */

class errCom
{
private:
    char* msg;
    void setMsg(char*);
public:
    errCom();
    errCom(char*);
    errCom(const errCom&);//constructeur de copie pour les exceptions!
    ~errCom();
    const char* quoi();
};

class com
{
private:
    HANDLE g_hCOM;
    COMMTIMEOUTS g_cto;
    DCB g_dcb;
public:
    com(int numPort=0,int vitesse=9600);
    ~com();

    BOOL OpenCOM    (int nId);
    BOOL CloseCOM   ();
    BOOL ReadCOM    (void* buffer, int nBytesToRead, int* pBytesRead);
    BOOL WriteCOM   (void* buffer, int nBytesToWrite, int* pBytesWritten);
    int ByteToRead();
};
#endif
