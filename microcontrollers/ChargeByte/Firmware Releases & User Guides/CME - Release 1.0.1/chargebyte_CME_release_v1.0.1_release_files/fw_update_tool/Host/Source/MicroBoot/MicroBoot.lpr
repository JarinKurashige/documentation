program MicroBoot;
//***************************************************************************************
//  Description: Contains the main program entry.
//    File Name: MicroBoot.lpr
//
//---------------------------------------------------------------------------------------
//                          C O P Y R I G H T
//---------------------------------------------------------------------------------------
//   Copyright (c) 2018 by Feaser    http://www.feaser.com    All rights reserved
//
//   This software has been carefully tested, but is not guaranteed for any particular
// purpose. The author does not offer any warranties and does not guarantee the accuracy,
//   adequacy, or completeness of the software and is not responsible for any errors or
//              omissions or the results obtained from use of the software.
//
//---------------------------------------------------------------------------------------
//                            L I C E N S E
//---------------------------------------------------------------------------------------
// This file is part of OpenBLT. It is released under a commercial license. Refer to
// the license.pdf file for the exact licensing terms. It should be located in
// '.\Doc\license.pdf'. Contact Feaser if you did not receive the license.pdf file.
// 
// In a nutshell, you are allowed to modify and use this software in your closed source
// proprietary application, provided that the following conditions are met:
// 1. The software is not released or distributed in source (human-readable) form.
// 2. These licensing terms and the copyright notice above are not changed.
// 
// This software has been carefully tested, but is not guaranteed for any particular
// purpose. Feaser does not offer any warranties and does not guarantee the accuracy,
// adequacy, or completeness of the software and is not responsible for any errors or
// omissions or the results obtained from use of the software.
//
//***************************************************************************************
{$IFDEF FPC}
{$MODE objfpc}{$H+}
{$ENDIF}

//***************************************************************************************
// Includes
//***************************************************************************************
uses
  {$IFDEF UNIX}{$IFDEF UseCThreads}
    cthreads,
    cmem, // the c memory manager is on some systems much faster for multi-threading
  {$ENDIF}{$ENDIF}
  Interfaces, // this includes the LCL widgetset
  Forms, MainUnit, CurrentConfig, ConfigGroups, SettingsDialog,
  SessionXcpDialog, CustomUtil, TransportXcpTcpIpDialog, MiscellaneousDialog,
  FirmwareUpdate, StopWatch, FileLogger
  { you can add units after this };

{$R *.res}

begin
  RequireDerivedFormResource:=True;
  Application.Initialize;
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
//******************************** end of MicroBoot.lpr *********************************

