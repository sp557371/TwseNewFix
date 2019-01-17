//---------------------------------------------------------------------------
#ifndef FixDefH
#define FixDefH
/////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <map>
#include "KwayBase.h"
#include "fixed_num.h"
namespace Kway {
namespace Tw   {
namespace Fix  {
namespace Stk  {
/////////////////////////////////////////////////////////////////////////////
#define FixSOH                      0x01
#define FixSOH_s                   "\x01"
// FIX Tags - Message Header
#define fTag_BeginString               8
#define fTag_BodyLength                9
#define fTag_MsgType                  35
#define fTag_SenderCompID             49
#define fTag_TargetCompID             56
#define fTag_MsgSeqNum                34
// for option tag
#define fTag_OnBehalfOfCompID        115
#define fTag_DeliverToCompID         128
//#define fTag_SecureDataLen           90
//#define fTag_SecureData              91
#define fTag_SenderSubID              50
#define fTag_SenderLocationID        142
#define fTag_TargetSubID              57
#define fTag_TargetLocationID        147
#define fTag_OnBehalfOfSubID         116
#define fTag_OnBehalfOfLocationID    144
#define fTag_DeliverToSubID          129
#define fTag_DeliverToLocationID     145
#define fTag_LastMsgSeqNumProcessed  369
#define fTag_OnBehalfOfSendingTime   370
// end option tag
#define fTag_PossDupFlag              43
#define fTag_SendingTime              52
#define fTag_OrigSendingTime         122
#define fTag_PossResend               97
// FIX Tags - Trailer
#define fTag_CheckSum                 10
/////////////////////////////////////////////////////////////////////////////
typedef long                             TFixMsgSeqNum;
typedef unsigned int                     TFixTag;
typedef std::pair<TFixTag, const char*>  TFixTagValue;  // .first  = tag
                                                        // .second = value
typedef std::vector<TFixTagValue>        TFixValues;
typedef TFixValues::iterator             TFixValueI;
typedef TFixValues::const_iterator       TFixValueCI;
//---------------------------------------------------------------------------
inline K_mf(TFixTagValue) MakeFixTagVal (TFixTag tag, int value, char* buf)
{  ltoa(value, buf, 10);   return TFixTagValue(tag, buf); }
//{  ltoa(tag, buf, 10);   return TFixTagValue(tag, buf); }
//---------------------------------------------------------------------------
struct SFixStr
{
   char  Msg_[1024*4];
   int   MsgLen_;

   K_ctor       SFixStr() : MsgLen_(0) {}
   K_mf(void)   Append(TFixTag, const char*);
   K_mf(void)   Append(TFixTag, int);
   K_mf(void)   Append(const TFixTagValue* begin, const TFixTagValue* end);
   K_mf(void)   Append(const TFixValues& a) { Append(&(*a.begin()), &(*a.end())); }
   K_mf(void)   Append(std::string& a);

   K_mf(char*)  SearchTag(const char* tag, int szTag) const;
   K_mf(char*)  AfterTag(const char* tag, int szTag) const;
   K_mf(int)    GetMsgSeqNum() const;  // Return -1 for not found
};
//---------------------------------------------------------------------------
K_class  TFixMsg
{
   friend class  TFixHeader;
   TFixValues  Values_;
   TFixValueI  iMsgType_;
   TFixValueI  iSenderCompID_;
   TFixValueI  iTargetCompID_;
   TFixValueI  iSenderSubID_;
   TFixValueI  iTargetSubID_;
   TFixValueI  iMsgSeqNum_;
   int         MsgSeqNum_;
   bool        PossDupFlag_;
   bool        PossResend_;
   TFixValueI  iSendingTime_;
   TFixValueI  iOrigSendingTime_;
   TFixValueI  iBodyBegin_;
   TFixValueI  iBodyEnd_;
public:
   K_ctor  TFixMsg();
   K_mf(bool)        Assign(class TStkSesFix*, SFixStr&);
   K_mf(TFixValueI)  GetiMsgType     () const    { return iMsgType_;      }
   K_mf(TFixValueI)  GetiSenderCompID() const    { return iSenderCompID_; }
   K_mf(TFixValueI)  GetiTargetCompID() const    { return iTargetCompID_; }
   K_mf(TFixValueI)  GetiSenderSubID () const    { return iSenderSubID_;  }
   K_mf(TFixValueI)  GetiTargetSubID () const    { return iTargetSubID_;  }
   K_mf(TFixValueI)  GetiSendingTime () const    { return iSendingTime_;  }
   K_mf(int)         GetMsgSeqNum    () const    { return MsgSeqNum_;     }
   K_mf(TFixValueI)  GetiMsgSeqNum   () const    { return iMsgSeqNum_;    }
   K_mf(bool)        GetPossDupFlag  () const    { return PossDupFlag_;   }
   K_mf(bool)        GetPossResend   () const    { return PossResend_;    }
   K_mf(TFixValueI)  GetiBodyBegin   () const    { return iBodyBegin_;    }
   K_mf(TFixValueI)  GetiBodyEnd     () const    { return iBodyEnd_;      }
   
   K_mf(const TFixValues&)  GetValues() const    { return Values_;        }

   K_mf(TFixValueI)  GetBodyTag(TFixTag, TStkSesFix* = NULL) const;
};
//---------------------------------------------------------------------------
/*K_class TFixHeader
{
    typedef std::map<TFixTag, std::string> TFixHeaderValues;
    typedef TFixHeaderValues::iterator     TFixHeaderValueI;
    typedef TFixHeaderValues::value_type   TFixHeaderValueV;
    TFixHeaderValues    Values_;
    K_mf(void) Assign (const TFixMsg&);
public:
    K_ctor  TFixHeader(const TFixMsg&);
    K_dtor ~TFixHeader() {}
    K_mf(std::string) GetValue(TFixTag);
    K_mf(bool)        TagExist(TFixTag);
    K_mf(void)        AssignAck(SFixStr&);
};*/
/////////////////////////////////////////////////////////////////////////////
#define MAC_Check_RequiredTagMissing(aSes, aFixMsg, cond, tagno)    \
   if(cond)                                                         \
   {                                                                \
      aSes->SendReject(aFixMsg, srrRequiredTagMissing, tagno);      \
      return;                                                       \
   }                                                                \
//--------------------------------------------------------------------
#define MAC_Check_RequiredTagMissing_r0(aSes, aFixMsg, cond, tagno) \
   if(cond)                                                         \
   {                                                                \
      aSes->SendReject(aFixMsg, srrRequiredTagMissing, tagno);      \
      return false;                                                 \
   }
//--------------------------------------------------------------------
#define MAC_Check_TagValueIncorrect(aSes, aFixMsg, cond, tagno)     \
   if(cond)                                                         \
   {                                                                \
      aSes->SendReject(aFixMsg, srrValueIsIncorrect, tagno); 		\
      return;                                                 		\
   }
//--------------------------------------------------------------------
#define MAC_Check_TagValueIncorrect_r0(aSes, aFixMsg, cond, tagno)  \
   if(cond)                                                         \
   {                                                                \
      aSes->SendReject(aFixMsg, srrValueIsIncorrect, tagno); 		\
      return false;                                                 \
   }
/////////////////////////////////////////////////////////////////////////////
}; // Stk
}; // Fix
}; // Tw
}; // Kway
#endif

