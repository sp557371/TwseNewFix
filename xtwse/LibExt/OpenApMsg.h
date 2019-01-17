//---------------------------------------------------------------------------
#ifndef OpenApMsgH
#define OpenApMsgH
//---------------------------------------------------------------------------
#include "SesMgrR.h"
#include "KFile.h"
#include "PacketSes.h"
#include "TimerThread.h"
#include "StdRecorder.h"
#include "ViewAreaMgr.h"
//---------------------------------------------------------------------------
namespace Kway {
using namespace Kstl;
//---------------------------------------------------------------------------
#define OpenHBTime  25
#define OpenAckTime 10
//---------------------------------------------------------------------------
template <class CharT, unsigned N, CharT NullChar>
class char_aryn : public char_ary_base<CharT,N>
{
   typedef char_aryn<CharT,N,NullChar>  this_type;
   typedef char_ary_base<CharT,N> inherited;
#ifndef __BORLANDC__
protected:
   using inherited::data_; //for 避免 gcc error.
public:
   using inherited::begin;
   using inherited::end;
#endif
public:
   //Types.


   //取出
   K_mf(std::string)  as_string  () const  { std::string             str(data_,N);
                                             CharT                   tmpFind = NullChar;
                                             std::string::size_type  lpos = str.find_last_not_of(&tmpFind,N,1);
                                             if(lpos == std::string::npos)//全部都是NullChar
                                                return std::string();
                                             if(++lpos < N)
                                                str.erase(lpos);
                                             return str;
                                           }
   K_mf(bool)         empty      () const  { const CharT* p = data_;
                                             for(int L = N;  L > 0;  --L)
                                                if(*p++ != NullChar)
                                                   return false;
                                                return true;
                                           }
   //填入
   inline K_mf(void)  assign  (const std::string& str)  { assign(str.begin(), str.length()); }
   inline K_mf(void)  assign  (const char* str)         { assign(str, strlen(str));          }
   template <class InputIterator>
    inline K_mf(void) assign  (InputIterator begin, unsigned len)
                                                { if(N > len)
                                                     std::fill_n(std::copy(begin, begin+len, data_),
                                                                 N-len, NullChar);
                                                  else
                                                     std::copy(begin, begin+N, data_);
                                                }
   inline K_mf(void)  clear  (CharT ch = NullChar)  { fill(ch); }
   //比較 compare
   inline K_mf(bool)  operator<  (const this_type& dst) const
                                 { return std::lexicographical_compare(begin(),end(),
                                                                       dst.begin(),dst.end()); }
   inline K_mf(bool)  operator== (const this_type& dst) const
                                 { return !std::lexicographical_compare(begin(),end(),
                                                                        dst.begin(),dst.end(),
                                                                        std::not_equal_to<CharT>()); }
   inline K_mf(bool)  operator!= (const this_type& dst) const  { return !operator==(dst); }
   inline K_mf(void)         fill        (CharT ch)              { std::fill_n(begin(), N, ch); }
};
//---------------------------------------------------------------------------
enum  EOpenFCodeS{
   efs_Init = 0,
   efs_SendData,
   efs_RstCnt,
   efs_HB,
   efs_Finish,
   efs_None,
};
//---------------------------------------------------------------------------
enum  EOpenFCodeR{
   efr_InitAck = 0,
   efr_SendDataAck,
   efr_RstCntAck,
   efr_HBAck,
   efr_FinishAck,
   efr_None,
};
//---------------------------------------------------------------------------
struct TOpenHead{
   char_ary<char, 2,0xFE>   Head_;
   char_ary<char, 2,' '>    Func_;
};
#define HdSize  sizeof(TOpenHead);
//---------------------------------------------------------------------------
struct TInit{
   char_aryn<char, 2,' '>   Msg_Record_Type;
   char_aryn<char, 3,' '>   Msg_Record_Length;
};
#define Init_sz 4 + sizeof(TInit);
//---------------------------------------------------------------------------
struct TInitAck{
   char_aryn<char, 8,' '>   Record_Number;
   char_aryn<char, 4,' '>   Status;
};
#define InitAck_sz 4 + sizeof(TInitAck);
//---------------------------------------------------------------------------
struct TDataHead{
   char_aryn<char, 5,' '>   Packet_Total_Length;
   char_aryn<char, 4,' '>   Batch_Record_Count;
};
struct TDataBody{
   char_aryn<char, 8,' '>   Date_;
   char_aryn<char, 6,' '>   Time_;
   char_aryn<char, 1,' '>   Dot_;
   char_aryn<char, 3,' '>   MS_;
   char_aryn<char, 1,' '>   Market_;
   char_aryn<char,20,' '>   HostName_;
   char_aryn<char,20,' '>   SysName_;
   char_aryn<char,20,' '>   SubSys_;
   char_aryn<char,30,' '>   ProgName_;
   char_aryn<char, 4,' '>   MsgType_;
   char_aryn<char, 6,' '>   EvCode_;
   char_aryn<char,240,' '>  Msg_;
};
struct TSendData:public TDataHead, public TDataBody{};
#define SendData_sz 4 + sizeof(TSendData);
//---------------------------------------------------------------------------
struct THB{
   char_aryn<char, 8,' '>   Date_;
   char_aryn<char, 1,' '>   Dash_;
   char_aryn<char, 6,' '>   Time_;
   char_aryn<char, 1,' '>   Dot_;
   char_aryn<char, 2,' '>   MS_;
};
#define HB_sz 4 + sizeof(THB);
#define Ack_sz 4 + 4;
//---------------------------------------------------------------------------
struct TOpenPkt : public TOpenHead{
   union{
      TInit                  Init_;
      TInitAck               InitAck_;
      TSendData              SendData_;
      THB                    HB_;
      char_aryn<char, 4,' '> Status;
      char                   Data[1024];
   };
   K_ctor TOpenPkt(){};
   K_ctor TOpenPkt(EOpenFCodeS ef){
      char buf[16];
      sprintf(buf,"%02d",ef);
      Func_.assign(buf);
   }
};
//---------------------------------------------------------------------------
K_class TOpenSesMgr;
//---------------------------------------------------------------------------
typedef TPacketSes<TOpenPkt,4,sizeof(TOpenHead)> TOpenSes_Base;
K_class TOpenCliSes : public TOpenSes_Base
{
   typedef TOpenCliSes      this_type;
   typedef TOpenSes_Base    inherited;
   typedef Kway::TTimer<this_type> TOpenTimer;
protected:
   TOpenSesMgr*       SesMgr_;
   TOpenTimer         HBTimer_, AckTimer_;
   std::string        IP_;
   bool               Ready_;
   char*              SrcPort_;
   K_mf(void)         OnHBTimer      (TTimerBase*);
   K_mf(void)         OnAckTimer     (TTimerBase*);
   K_mf(void)         ResetHBTimer   ();
   K_mf(void)         SendP          (TOpenPkt&);
   K_mf(int)          GetPacketSize  (TOpenPkt*,bool);
   K_mf(void)         OnMesLinkBroken(TMesBase*, const std::string&);
   K_mf(void)         OnMesLinkFail  (TMesBase*, const std::string&){Ready_ = false;}
   K_mf(void)         OnMesLinkReady (TMesBase*);
   K_mf(void)         OnPktRecv      (TOpenPkt&);
public:
   K_ctor             TOpenCliSes    (Kway::TSesMgr* sm);
   K_dtor             ~TOpenCliSes   ();
   K_mf(bool)         IsReady        (){return Ready_;}
   K_mf(void)         SendAlert      (TSendData&);
   K_mf(void)         SendReset      ();
   K_mf(void)         Disconnect     ();
};
//---------------------------------------------------------------------------
struct  TDataRec{
   TSendData     Data_;
   bool          SendAlready_;
   K_ctor TDataRec(){
      SendAlready_ = false;
   }
};
typedef fixed_num<Kway::dword>            TRecSeq;
typedef ev_map<TRecSeq, TDataRec>         TDataRecs;
typedef TDataRecs::iterator               TDataReci;
typedef TStdRecorder<TDataRecs, TDataRec> TDataRecorder;
//---------------------------------------------------------------------------
K_class TOpenMsgFieldMaker                              //第2層的顯示欄位
{
private:
public:
   K_ctor TOpenMsgFieldMaker(){};
   K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
   K_mf(size_t) GetFieldCount() const { return 10; }
   K_mf(std::string) Get(const TDataReci&, size_t) const;
   K_mf(void) Put(TDataRecs&, TDataReci&, size_t, const std::string&){};
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TDataRecs,TOpenMsgFieldMaker> TOpenMsgViewer;    
//---------------------------------------------------------------------------
K_class TOpenMsgVACr:public Kway::TVACreater            //第2層的VAC
{
 typedef Kway::TVACreater inherited;
 typedef TOpenMsgVACr     this_type;
private:
   TDataRecs*             DataRecs_;
public:
   K_ctor TOpenMsgVACr(TDataRecs* evd):DataRecs_(evd){};
   K_mf(Kway::TaViewArea) CreateNewVA(){
      TViewControls   vcs;
      return Kway::TaViewArea(new TOpenMsgViewer(DataRecs_, TOpenMsgFieldMaker(), vcs));
   }
};
//---------------------------------------------------------------------------
K_class TOpenSesMgr : public TSesMgrR
{
private:
   typedef std::vector<TOpenCliSes*> TOpenSess;
   TOpenSess                  OpenSess_;
   Kway::dword                MsgCount_;
   Kway::TFile                Log_;
   std::string                SystemName_,HostName_;
   TDataRecs*                 DataRecs_;
   TDataRecorder*             DataRecorder_;
protected:
public:
   TOpenMsgVACr*   OpenMsgVACr_;
   K_ctor          TOpenSesMgr();
   K_dtor          ~TOpenSesMgr();
   K_mf(bool)      SendOpenMsg(TSendData&);
   K_mf(void)      WriteLog(const char*);
   K_mf(void)      RegOpenCliSes(TOpenCliSes* obj, Kway::dword);
   K_mf(void)      DelOpenCliSes(TOpenCliSes* obj);
   K_mf(void)      SendNext();
   K_mf(void)      NewMsg(TSendData&);
   K_mf(void)      Disconnect();
};
//---------------------------------------------------------------------------
};//namespace Kway
#endif
