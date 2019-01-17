/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_PktSes_h_)
#define _Nsa_PktSes_h_
//---------------------------------------------------------------------------
#include "Ses.h"
#include "Nsa_UtilComm.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TSesBaseExt
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TSesBaseExt : public TSesBase
{
   typedef TSesBase  inherited;

public:
   // Send
   K_mf(int) Send (const void* buffer, size_t size)
   {
      TMesBase* Mes = inherited::GetMes();

      if (Mes == NULL || Mes->IsLinkReady() == false) {
         return 0;
      }

      return Mes->Send(buffer, size);
   }

protected:
   virtual K_mf(void) OnMesLinkReady  (TMesBase*)                     {}
   virtual K_mf(void) OnMesLinkFail   (TMesBase*, const std::string&) {}
   virtual K_mf(void) OnMesLinkBroken (TMesBase*, const std::string&) {}
};

/////////////////////////////////////////////////////////////////////////////
// class TPacketSes
/////////////////////////////////////////////////////////////////////////////
template <class Packet, int HeadSize, class SesBase = TSesBaseExt>
class TPacketSes : public SesBase
{
public:
   typedef SesBase  inherited;
   typedef Packet   TPacket;
   typedef SesBase  TSesBase;

protected:
   char      Buffer_[sizeof(TPacket) * 4];
   TPacket*  Packet_;
   word      CurrPos_;

protected:
   //==============================================
   // overwrite TSesBase virtual function
   //virtual K_mf(bool) SettingApply (const std::string& str);
   //virtual K_mf(void) OnMesLinkBroken (TMesBase*, const std::string&);
   //virtual K_mf(void) OnMesLinkFail   (TMesBase*, const std::string&);
   virtual K_mf(void) OnMesLinkReady (TMesBase*);
   virtual K_mf(void) OnMesRecvReady (TMesBase*, size_t size);

   //==============================================
   // TPacketSes virtual function
   virtual K_mf(int)  GetPacketSize (TPacket*, bool IsSend = false)   = 0;
   virtual K_mf(void) OnPktRecv     (TPacket&, int PktSize)           = 0;
   virtual K_mf(void) OnMesDataIn   (const char* buffer, size_t size)  {}
   virtual K_mf(void) OnSendPkt     (TPacket&, int pktSize)            {}

public:
           K_ctor  TPacketSes ();
   virtual K_dtor ~TPacketSes ();

   K_mf(bool) SendPkt (TPacket& Pkt);
};

/////////////////////////////////////////////////////////////////////////////
// class TLineSes
/////////////////////////////////////////////////////////////////////////////
template <class SesT = TSesBaseExt>
class TLineSes : public SesT
{
   typedef SesT  inherited;

   enum {
      Buf_Size = 2048,
   };
   
protected:
   char    RecvBuf_[Buf_Size];
   size_t  CurrPos_;
   size_t  ScanPos_;
   char    SplitChar_;
   
public:
   K_ctor TLineSes (const char SplitChar);

protected:
   //==============================================
   // overwrite TSes virtual function
   //K_mf(bool) SettingApply    (const std::string& str)                       { return inherited::SettingApply(str);  }
   //K_mf(void) OnMesLinkBroken (TMesBase* mes, const std::string& str)  { inherited::OnMesLinkBroken(mes, str); }
   //K_mf(void) OnMesLinkFail   (TMesBase* mes, const std::string& str)  { inherited::OnMesLinkFail  (mes, str); }
   K_mf(void) OnMesLinkReady  (TMesBase*);
   K_mf(void) OnMesRecvReady  (TMesBase*, size_t);

   //==============================================
   // TLineSes virtual function
   virtual K_mf(void) OnPktRecv   (void*, size_t) = 0;
   virtual K_mf(void) OnMesDataIn (const char* buffer, size_t size) {};
};

/////////////////////////////////////////////////////////////////////////////
// class TSplitSes
/////////////////////////////////////////////////////////////////////////////
template <class TSes = TSesBaseExt>
class TSplitSes : public TSes
{
   typedef TSes  inherited;
   
protected:
   char    RecvBuf_[10240*10];
   size_t  CurrPos_;
   size_t  ScanPos_;
   char    SplitChar_[10];
   size_t  SplitNum_;

public:
   K_ctor TSplitSes (const char* splitChar, size_t splitNum);

protected:
   //==============================================
   // TSplitSes virtual function
   virtual K_mf(bool) OnPktRecv   (void*, size_t) = 0;
   virtual K_mf(void) OnMesDataIn (const char* buffer, size_t size) {};

   //==============================================
   // overwrite TSes virtual function
   //K_mf(bool) SettingApply    (const std::string& str)                       { return inherited::SettingApply(str);  }
   //K_mf(void) OnMesLinkBroken (TMesBase* mes, const std::string& str)  { inherited::OnMesLinkBroken(mes, str); }
   //K_mf(void) OnMesLinkFail   (TMesBase* mes, const std::string& str)  { inherited::OnMesLinkFail  (mes, str); }
   K_mf(void) OnMesLinkReady (TMesBase*);
   K_mf(void) OnMesRecvReady (TMesBase*, size_t);
};

#include "Nsa_PktSes.cc"

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_PktSes_h_)
