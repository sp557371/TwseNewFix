/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseIniFile_h_)
#define _TwseIniFile_h_
//---------------------------------------------------------------------------
#include "KIniFile.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// class TTwseMainIni
/////////////////////////////////////////////////////////////////////////////
class TTwseMainIni : public TKIniFile
{
   typedef TKIniFile  inherited;
   
public:
   struct SSecMain
   {
      int          TmpFailCount_;
      int          SocketRetryInterval_;    // ��o�� LinkFail ����,���s�����
      int          AppiaMwSesLogonItvl_;
      int          AppiaMwSesBfHbItvl_;
      int          AppiaMwSesAfHbItvl_;
      int          AppiaMwSesAfBrokenItvl_; // ApaMwSes �s�u�]������]���_,���s�����
      int          AppiaMwSesReopenItvl_;   // ApaMwSes �]�Y�ح�]�۰��_�u��,���s�����
      bool         AutoFlushCoids_;
      bool         AutoFlushExecIDGene_;
      bool         AutoFlushOrdCenter_;
      bool         AutoFlushOrdTrack_;
      bool         AutoFlushRptCenter_;
      std::string  AppiaSvrIP_;
      int          FixVer_;                 // �o�� Session �� Fix ����
   };

   struct SCont
   {
      SSecMain  Main_;
   };

private:
   // ���F�קKŪ�� Main.ini �Ӧh��, �ҥH�@�}�lŪ������N�s�_���o
   static SCont  Cont_;

public:
   K_ctor TTwseMainIni ();

   K_mf(void) InitCont (const std::string& sesId = std::string());

   K_mf(std::string) GetAppiaSvrPort ();

   inline K_mf(std::string) GetAppiaSvrIP ()
      { return inherited::ReadString(std::string("Main"), std::string("AppiaSvrIP"), std::string()); }

   // GetAppiaSvrPortList
   inline K_mf(int) GetAppiaSvrPortList (TStrings& strs)
      { return inherited::ReadStrings("Main", std::string("AppiaMWPort"), strs); }

   // GetFixPassword
   inline K_mf(std::string) GetFixPassword (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("Password"), std::string()); }

   // GetFixVer
   inline K_mf(int) GetFixVer (const std::string& sesID)
      { return inherited::ReadInt(sesID, std::string("connection_type"), 42); }
   
   // GetTmpFailCount
   inline K_mf(size_t) GetTmpFailCount ()
      { return (size_t)inherited::ReadInt(std::string("Main"), std::string("TMPFailCount"), 3); }

   // GetMaxOrderCount
   inline K_mf(int) GetMaxOrderCount ()
      { return inherited::ReadInt(std::string("Main"), std::string("MaxOrderCount"), 0); }
   
   // GetSocketRetryInterval (��)(�w�] 5 ��)
   inline K_mf(int) GetSocketRetryInterval ()
      { return inherited::ReadInt(std::string("Main"), std::string("SocketRetryInterval"), 5); }

   // GetAppiaMwSesLogonItvl (��)(�w�] 5 ��)
   inline K_mf(int) GetAppiaMwSesLogonItvl ()
      { return inherited::ReadInt(std::string("Main"), std::string("AppiaMwSesLogonItvl"), 5); }

   // GetAppiaMwSesBfHbItvl (��)(�w�] 11 ��)
   inline K_mf(int) GetAppiaMwSesBfHbItvl ()
      { return inherited::ReadInt(std::string("Main"), std::string("AppiaMwSesBfHbItvl"), 11); }

   // GetAppiaMwSesAfHbItvl (��)(�w�] 2 ��)
   inline K_mf(int) GetAppiaMwSesAfHbItvl ()
      { return inherited::ReadInt(std::string("Main"), std::string("AppiaMwSesAfHbItvl"), 2); }

   // GetAppiaMwSesAfBrokenItvl (��)(�w�] 0 ��)
   inline K_mf(int) GetAppiaMwSesAfBrokenItvl ()
      { return inherited::ReadInt(std::string("Main"), std::string("AppiaMwSesAfBrokenItvl"), 0); }

   // GetAppiaMwSesReopenItvl (��)(�w�] 2 ��)
   inline K_mf(int) GetAppiaMwSesReopenItvl ()
      { return inherited::ReadInt(std::string("Main"), std::string("AppiaMwSesReopenItvl"), 2); }

   // GetVACSvrAddr
   inline K_mf(int) GetVACSvrAddr (TStrings& vacips)
      { return inherited::ReadStrings("Main", "VACSvrAddr", vacips); }

   // GetXCtrlVACAddr
   inline K_mf(int) GetXCtrlVACAddr (TStrings& vacips)
      { return inherited::ReadStrings("Main", "XCtrlVACAddr", vacips); }

   // GetXTesterVACAddr
   inline K_mf(int) GetXTesterVACAddr (TStrings& vacips)
      { return inherited::ReadStrings("Main", "XTesterVACAddr", vacips); }

   // IsAutoFlushCoids
   inline K_mf(bool) IsAutoFlushCoids ()
      { return inherited::ReadBool(std::string("Main"), std::string("AutoFlushCoids"), false); }

   // IsAutoFlushExecIDGene
   inline K_mf(bool) IsAutoFlushExecIDGene ()
      { return inherited::ReadBool(std::string("Main"), std::string("AutoFlushExecIDGene"), false); }

   // IsAutoFlushFixSeqNo
   inline K_mf(bool) IsAutoFlushFixSeqNo ()
      { return inherited::ReadBool(std::string("Main"), std::string("AutoFlushFixSeqNo"), true); }

   // IsAutoFlushOrdCenter
   inline K_mf(bool) IsAutoFlushOrdCenter ()
      { return inherited::ReadBool(std::string("Main"), std::string("AutoFlushOrdCenter"), false); }

   // IsAutoFlushOrdTrack
   inline K_mf(bool) IsAutoFlushOrdTrack ()
      { return inherited::ReadBool(std::string("Main"), std::string("AutoFlushOrdTrack"), false); }

   // IsAutoFlushRptCenter
   inline K_mf(bool) IsAutoFlushRptCenter ()
      { return inherited::ReadBool(std::string("Main"), std::string("AutoFlushRptCenter"), false); }

   static inline K_mf(SCont&)       GetCont  ()  { return Cont_; }
   static inline K_mf(const SCont&) GetContC ()  { return Cont_; }
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseAppendNoIni
/////////////////////////////////////////////////////////////////////////////
class TTwseAppendNoIni : public TKIniFile
{
   typedef TKIniFile  inherited;
   
   TStrings  ItemList_;

public:
   K_ctor TTwseAppendNoIni ();

   K_mf(bool)        IsRepeat   (const std::string&);
   K_mf(std::string) GetListStr () const;
   K_mf(void)        SetList    (const std::string&, int fixMsgSeqNum);
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseDrIni
/////////////////////////////////////////////////////////////////////////////
class TTwseDrIni : public TKIniFile
{
   typedef TKIniFile  inherited;
   
public:
   K_ctor TTwseDrIni ();

   inline K_mf(bool) IsDR ()  { return inherited::ReadBool(std::string("Conf"), std::string("DR"), false); }
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseDrInfoIni
/////////////////////////////////////////////////////////////////////////////
class TTwseDrInfoIni : public TKIniFile
{
   typedef TKIniFile  inherited;
   
public:
   K_ctor TTwseDrInfoIni ();

   inline K_mf(void) SetProcFinish ()
      { inherited::WriteBool(std::string("Conf"), std::string("ProcFinish"), true); }

   inline K_mf(bool) IsProcFinish ()
      { return inherited::ReadBool(std::string("Conf"), std::string("ProcFinish"), false); }
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseIniFile_h_)
