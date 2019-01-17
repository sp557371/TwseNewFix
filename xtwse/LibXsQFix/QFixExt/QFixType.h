/////////////////////////////////////////////////////////////////////////////
#if !defined(_QFixType_h_)
#define _QFixType_h_
//---------------------------------------------------------------------------
#include <string>
#include "KTime.h"
#include "wise_ptr.h"
#include "../src/Code/SessionID.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace FIX  {

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
   
/////////////////////////////////////////////////////////////////////////////
// enum EFixWFileSts
/////////////////////////////////////////////////////////////////////////////
enum EFixWFileSts
{
   wfs_Success,
      wfs_WriteError,
      wfs_FlushError,
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif


/////////////////////////////////////////////////////////////////////////////
// class TFieldValue
/////////////////////////////////////////////////////////////////////////////
K_class TFieldValue : public std::string
{
   typedef std::string  inherited;

public:
   K_mf(void)        SetNoTag  ();
   K_mf(bool)        IsNoTag   () const;
   K_mf(std::string) as_string () const;
};


/////////////////////////////////////////////////////////////////////////////
// class TFixSsuBase  (Session Status Updater)
/////////////////////////////////////////////////////////////////////////////
class TFixSsuBase : public Kstl::ref_counter<>
{
public:
   virtual K_mf(void) SenderMsgSeqNumChange (int oldVal, int newVal) = 0;
   virtual K_mf(void) TargetMsgSeqNumChange (int oldVal, int newVal) = 0;

   virtual K_mf(void) SetDesc          (const char*) = 0;
};
//---------------------------------------------------------------------------
typedef Kstl::wise_ref<TFixSsuBase>  TFixSsuBasePtr;


/////////////////////////////////////////////////////////////////////////////
// class TFixSnsBase  (SeqNumsStore)
/////////////////////////////////////////////////////////////////////////////
class TFixSesInfo;
//---------------------------------------------------------------------------
class TFixSnsBase : public Kstl::ref_counter<>
{
protected:
   TFixSsuBase*  SsuBase_;
   int           NextSenderMsgSeqNum_;
   int           NextTargetMsgSeqNum_;

public:
   inline K_ctor TFixSnsBase (TFixSsuBase* ssuBase)
      : SsuBase_            (ssuBase)
      , NextSenderMsgSeqNum_(1)
      , NextTargetMsgSeqNum_(1)
   {}
   
   virtual K_dtor ~TFixSnsBase ()  {}

   virtual K_mf(EFixWFileSts) SetNextSenderMsgSeqNum (int value)
      {
         int old = NextSenderMsgSeqNum_;
         NextSenderMsgSeqNum_ = value;
         
         if (SsuBase_) {
            SsuBase_->SenderMsgSeqNumChange(old, NextSenderMsgSeqNum_);
         }

         return wfs_Success;
      }
   
   virtual K_mf(EFixWFileSts) SetNextTargetMsgSeqNum (int value)
      {
         int old = NextTargetMsgSeqNum_;
         NextTargetMsgSeqNum_ = value;

         if (SsuBase_) {
            SsuBase_->TargetMsgSeqNumChange(old, NextTargetMsgSeqNum_);
         }

         return wfs_Success;
      }

   inline K_mf(void) Clear ()
      {
         int oldSender = NextSenderMsgSeqNum_;
         int oldTarget = NextTargetMsgSeqNum_;

         NextSenderMsgSeqNum_ = NextTargetMsgSeqNum_ = 1; 
         
         if (SsuBase_) {
            SsuBase_->SenderMsgSeqNumChange(oldSender, NextSenderMsgSeqNum_);
            SsuBase_->TargetMsgSeqNumChange(oldTarget, NextTargetMsgSeqNum_);
         }
      }

   inline K_mf(int) GetNextSenderMsgSeqNum () const  { return NextSenderMsgSeqNum_; }
   inline K_mf(int) GetNextTargetMsgSeqNum () const  { return NextTargetMsgSeqNum_; }
};
//---------------------------------------------------------------------------
typedef Kstl::wise_ref<TFixSnsBase>  TFixSnsBasePtr;


/////////////////////////////////////////////////////////////////////////////
// class TFixSesInfo
/////////////////////////////////////////////////////////////////////////////
K_class TFixSesInfo
{
   Kway::TKDate    TradeDate_;
   std::string     FixLogPath_;
   SessionID       FixSessionID_;
   TFixSnsBasePtr  FixSnsPtr_;
   TFixSsuBasePtr  FixSsuPtr_;

public:
   K_mf(void) SetFixLogPath (const char*);

   inline K_mf(void) SetTradeDate  (const Kway::TKDate& d)    { TradeDate_  = d;   }
   inline K_mf(void) SetFixSnsPtr  (TFixSnsBasePtr      ptr)  { FixSnsPtr_  = ptr; }
   inline K_mf(void) SetFixSsuPtr  (TFixSsuBasePtr      ptr)  { FixSsuPtr_  = ptr; }

   inline K_mf(SessionID&)     GetFixSessionID ()  { return FixSessionID_; }
   inline K_mf(TFixSnsBasePtr) GetFixSnsPtr    ()  { return FixSnsPtr_;    }
   inline K_mf(TFixSsuBasePtr) GetFixSsuPtr    ()  { return FixSsuPtr_;    }
   
   inline K_mf(const SessionID&)    GetFixSessionID () const  { return FixSessionID_; }
   inline K_mf(const Kway::TKDate&) GetTradeDate    () const  { return TradeDate_;    }
   inline K_mf(const std::string&)  GetFixLogPath   () const  { return FixLogPath_;   }
};


/////////////////////////////////////////////////////////////////////////////
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_QFixType_h_)
