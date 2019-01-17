//---------------------------------------------------------------------------
#ifndef PMachSesBaseH
#define PMachSesBaseH
//---------------------------------------------------------------------------
#include "PacketSes.h"
#include "bxSesMgr.h"
#include "char_ary.h"
#include "TwStkOrder.h"
#include "TimerThread.h"
#include "KFile.h"
#include <set>
//---------------------------------------------------------------------------
namespace Kway{
namespace Tw{
namespace Bx{
namespace Rpt{
//---------------------------------------------------------------------------
struct SPMach{
   Kstl::char_ary<char,6,' '>   StkNo_;
   Kstl::char_ary<char,8,' '>   MthQty_;
   Kstl::char_ary<char,6,' '>   MthPri_;
   Kstl::char_ary<char,6,' '>   MthTime_;
   Kstl::char_ary<char,2,' '>   MthTimems_;
   Kstl::char_ary<char,1,' '>   ExchageCode_;
   Kstl::char_ary<char,1,' '>   BSCode_;
   Kstl::char_ary<char,5,' '>   OrderNo_;
   Kstl::char_ary<char,7,' '>   IvacNo_;
   Kstl::char_ary<char,1,' '>   OrderType_;
   Kstl::char_ary<char,6,' '>   SeqNo_;
   Kstl::char_ary<char,4,' '>   BrokerID_;
   Kstl::char_ary<char,8,' '>   RecNo_;                 //成交總檔編號
   Kstl::char_ary<char,1,' '>   MarkS_;                 //補送註記,若為補送為"*"
};
//---------------------------------------------------------------------------
struct SPMachLN: public SPMach{
   Kstl::char_ary<char,1,'\n'>  NewLine_;               //換行字元
};
//---------------------------------------------------------------------------
template<class TPMach> 
K_class TPMachSesBase : public TPacketSes<TPMach, 0, sizeof(TPMach)>
{
   typedef TPacketSes<TPMach, 0, sizeof(TPMach)> inherited;
   typedef TPMachSesBase                         this_type;
   typedef std::set<long>                        TSeqSet;
   typedef TSeqSet::iterator                     TSeqSetI;
   typedef KStk::TMarket                         TMarket;
protected:
   typedef Kway::TTimer<this_type>               TRecoverTimer;
   Kway::TFile     Pmach_;
   TSesMgr*        SesMgr_;
   TSeqSet         SeqSet_;
   long            LastSeqNo_;     // 最後的成交回報序號
   bool            Recovering_;    // 補漏進行中
   TBrkID          BrkID_;
   TMarket         Market_;
   TRecoverTimer*  RecoverTimer_;
   std::string     FileName_;
   std::string     RecoverFName_;
   bool            IsLn_;
   K_mf(void)      OnPktRecv        (TPMach& rpt){
      char buf[64];
      long seqno = rpt.SeqNo_.as_int();
      if(rpt.RecNo_.as_int() == 0){   // 總檔編號為0 代表成交回報結束
         SeqSet_.insert(seqno+1);
         if(!Recovering_)
            CheckSetCont();
      }else{
         SeqSet_.insert(seqno);
         if((seqno - LastSeqNo_) > 1 || LastSeqNo_ > SeqSet_.size())  // 是否為接續的下一筆,否則啟動補漏機制
            if(!Recovering_)
               CheckSetCont();
         if(seqno > LastSeqNo_)   // 取連續的最後一筆
            LastSeqNo_ = seqno;       
      }
      sprintf(buf,"(%6d / %6d)",SeqSet_.size(), LastSeqNo_);
      OnStateChanged(std::string(buf));
   }
   K_mf(void)      OnMesLinkReady   (TMesBase* m)
   {
      inherited::OnMesLinkReady(m);
      RecoverFName_ = std::string();
   }
   K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&){};
   K_mf(void)      OnMesLinkFail    (TMesBase*, const std::string&){};
   K_mf(bool)      SettingApply     (const std::string& setting){
      std::string::size_type   pos1 = 0;
      std::string::size_type   pos2 = 0;
      for(int i=0;i<2;i++){
         pos1 = setting.find(',', pos2);
         if(pos1 != std::string::npos){
            pos2 = pos1 + 1;
            if(i == 0){
               BrkID_.assign(setting.substr(0, pos1));
               if(setting[pos2] == 'T')
                  Market_ = m_Tse;
               else
                  Market_ = m_Otc;
            }else
               FileName_ = &setting[pos2]; 
         }else{
            OnStateChanged("設定錯誤:BrokerID,Market,Filename");
            return false;
         }
      }
      Pmach_.Open(FileName_.c_str(), TFileMode(Kway::fmRandomAccess | Kway::fmOpenAlways | Kway::fmCreatePath));
      TPMach rpt;
      long seqno;
      char buf[64];
      while(!Pmach_.IsEOF()){
         if(Pmach_.Read(&rpt, sizeof(rpt)) == sizeof(rpt)){
            seqno = rpt.SeqNo_.as_int();
            if(rpt.RecNo_.as_int() == 0){   // 總檔編號為0 代表成交回報結束
            }else{
               if(seqno > LastSeqNo_)   // 取連續的最後一筆
                  LastSeqNo_ = seqno;       
               SeqSet_.insert(seqno);
            }
         }
      }
      sprintf(buf,"(%6d / %6d)",SeqSet_.size(), LastSeqNo_);
      OnStateChanged(std::string(buf));
      Pmach_.Close();
      ((TbxFTSesMgr*)SesMgr_)->RegPMachSes((TSesBase*)this, IsLn_);
      return true;
   }
virtual K_mf(void)      ResetRTimer      (int interval){};
   K_mf(void)      RecoverTimeOut   (TTimerBase*)
   {
      RecoverTimer_->Stop();
      if(RecoverFName_ == std::string()){
         Recovering_ = false;
         CheckSetCont();
      }else{
         DoImpRptFile(RecoverFName_);
         RecoverFName_ = std::string();
      }
   }
   K_mf(bool) CallRecover(long from, long end)
   {
      long recs = 0;
      if(from > end){
         recs = from;
        from = end;
         end  = recs;
      }
      recs = end - from +1;
      long range = (Market_ == KStk::m_Tse?200:50);  // OTC 的成交回補一次只能處理50筆, TSE則為200筆
      if(recs > range)  
        recs = range;
      ((TbxFTSesMgr*)SesMgr_)->RequestRpt(BrkID_, Market_, from, from+recs-1);
      Recovering_ = true;
      ResetRTimer(3*60);
      return true;
   }
   K_mf(void)      CheckSetCont     ()     // 檢查序號完整性
   {
      long lastSeq = 0;
      for(TSeqSetI i=SeqSet_.begin(); i!=SeqSet_.end(); ++i){
         if(*i > lastSeq+1){
            CallRecover(lastSeq + 1, *i - 1);
            break;  //一次只做一個要求, 等完成後再進行下一筆
         }
         lastSeq = *i;
      }
   }
public:
   K_ctor          TPMachSesBase    (TSesMgr* sm)/*:inherited(sm)*/{};
   K_mf(void)      ImpRptFile       (std::string& fname){
      RecoverTimer_->Stop();
      RecoverFName_ = fname;
      ResetRTimer(1);  // 停頓1秒後才進行讀取檔案的動作
   }
   K_mf(void)      DoImpRptFile     (std::string& fname)
   {
      char buf[64];
      Kway::TFile file;
      file.Open(fname.c_str(), TFileMode(Kway::fmRandomAccess | Kway::fmOpenAlways | Kway::fmCreatePath));
      int recs;
      TPMach rpt;
      long seqno;
      Pmach_.Open(FileName_.c_str(), TFileMode(Kway::fmRandomAccess | Kway::fmOpenAlways | Kway::fmCreatePath));
      Pmach_.Seek(0,fsEnd);
      if(file.IsOpened()){
         recs = (int)file.GetFileSize() / sizeof(SPMach);
         file.Seek(0,fsBegin);
         for(int i=0;i<recs;i++)
            if(file.Read(&rpt, sizeof(SPMach)) == sizeof(SPMach)){
               seqno = rpt.SeqNo_.as_int();
               if(SeqSet_.find(seqno) != SeqSet_.end())
                  continue;
               if(seqno > LastSeqNo_)
                  LastSeqNo_ = seqno;
               SeqSet_.insert(seqno);
               Pmach_.Write(&rpt, sizeof(TPMach));
            }
      }
      file.Close();
      Pmach_.Close();
      sprintf(buf,"(%6d / %6d)",SeqSet_.size(), LastSeqNo_);
      OnStateChanged(std::string(buf));
      Recovering_ = false;
      CheckSetCont();   // 回補完成, 再重新檢查是否有漏
   }
   K_mf(TMarket)   GetMarket        (){return Market_;}
   K_mf(TBrkID)    GetBrokerID      (){return BrkID_;}
};
//---------------------------------------------------------------------------
K_class TPMachSes : public TPMachSesBase<SPMach>
{
   typedef TPMachSesBase<SPMach>   inherited;
   typedef TPMachSes               this_type;
public:
   K_ctor          TPMachSes     (TSesMgr* sm);
   K_dtor          ~TPMachSes    ();
   K_mf(void)      ResetRTimer   (int interval);
};
//---------------------------------------------------------------------------
K_class TPMachSesLN : public TPMachSesBase<SPMachLN>
{
   typedef TPMachSesBase<SPMachLN> inherited;
   typedef TPMachSesLN             this_type;
public:
   K_ctor          TPMachSesLN   (TSesMgr* sm);
   K_dtor          ~TPMachSesLN  ();
   K_mf(void)      ResetRTimer   (int interval);
};
//---------------------------------------------------------------------------
};
};
};
};
//---------------------------------------------------------------------------
#endif
