//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------






void __fastcall TForm1::ServerSocket1ClientDisconnect(TObject *Sender, TCustomWinSocket *Socket)

{
	ServerSocket1->Active = False;
}
//---------------------------------------------------------------------------






void __fastcall TForm1::Button1Click(TObject *Sender)
{

	ServerSocket1->Port = 23;
	ServerSocket1->Active = True;
	if (ServerSocket1->Active == True) {

	   Label2->Caption = "connecté";

	}
	
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ServerSocket1ClientConnect(TObject *Sender, TCustomWinSocket *Socket)

{


   /*	if(ClientSocket1->Active == True)
		{
		Label1->Caption = "connecté" ;
		 }  */
}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
		ServerSocket1->Active = True;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
	ServerSocket1->Port = 23;
	ServerSocket1->Active = True;
		if (ServerSocket1->Active == True) {
	   Label2->Caption = "connecté";
	}
}
//---------------------------------------------------------------------------

