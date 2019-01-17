//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixDef.h"
#include <stdio.h>
#include <functional>
#include <bitset>
#include "SesFix.h"
using namespace  Kway::Tw::Fix::Stk;
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
K_mf(void)  SFixStr::Append(TFixTag tag, const char* value)
{
   MsgLen_ += sprintf(Msg_+MsgLen_, "%d=%s" FixSOH_s, tag, value);
}
//---------------------------------------------------------------------------
K_mf(void)  SFixStr::Append(TFixTag tag, int value)
{
   MsgLen_ += sprintf(Msg_+MsgLen_, "%d=%d" FixSOH_s, tag, value);
}
//---------------------------------------------------------------------------
K_mf(void)  SFixStr::Append(const TFixTagValue* begin, const TFixTagValue* end)
{
   for( ; begin != end; ++begin)
      Append(begin->first, begin->second);
}
//---------------------------------------------------------------------------
K_mf(void)  SFixStr::Append(std::string& str)
{
   MsgLen_ += sprintf(Msg_+MsgLen_, "%s", str.c_str());
}
//---------------------------------------------------------------------------
K_mf(char*)  SFixStr::SearchTag(const char* tag, int szTag) const
{
   const char*  msgEnd = Msg_ + MsgLen_;
   const char*  msgTag = std::search(Msg_, msgEnd, tag, tag+szTag);
   return (char*)(msgTag == msgEnd ? NULL:msgTag);
}
//---------------------------------------------------------------------------
K_mf(char*)  SFixStr::AfterTag(const char* tag, int szTag) const
{
   char*  msgTag = SearchTag(tag, szTag);
   return (msgTag ? (msgTag + szTag):NULL);
}
//---------------------------------------------------------------------------
K_mf(int)  SFixStr::GetMsgSeqNum() const
{
   static const char  FIXMsgSeqNumTag[] = FixSOH_s "34=";
   static const int   szFIXMsgSeqNumTag = sizeof(FIXMsgSeqNumTag) - 1;  
   char*  msgSeqNumTag = AfterTag(FIXMsgSeqNumTag, szFIXMsgSeqNumTag);
   return (msgSeqNumTag ? atoi(msgSeqNumTag):-1);
}
/////////////////////////////////////////////////////////////////////////////
K_ctor  TFixMsg::TFixMsg()
{
}
//---------------------------------------------------------------------------
K_mf(bool)  TFixMsg::Assign(TStkSesFix* aSes, SFixStr& aStr)
{
   Values_.clear();
   Values_.reserve(128);

   iMsgType_      = TFixValueI(0);
   iSenderCompID_ = TFixValueI(0);
   iTargetCompID_ = TFixValueI(0);
   iSenderSubID_  = TFixValueI(0);
   iTargetSubID_  = TFixValueI(0);
   iSendingTime_  = TFixValueI(0);
   iBodyBegin_    = TFixValueI(0);
   iBodyEnd_      = TFixValueI(0);
   iMsgSeqNum_    = TFixValueI(0);

   TFixValueI   iPossDupFlag = TFixValueI(0);
   TFixValueI   iPossResend  = TFixValueI(0);
   TFixValueI   iCheckSum    = TFixValueI(0);
   char*        strMsg       = aStr.Msg_;
   typedef std::bitset<2048>  TtagsTest;
   TtagsTest    tags;
   TFixTag      dupTag = 0;
   
   for(;;) {
      TFixTagValue  tagValue;
      tagValue.first = atoi(strMsg);    
      if(tagValue.first < tags.size()) {
         TtagsTest::reference  ref = tags[tagValue.first];
         if(ref) {
            dupTag = tagValue.first;
            break;
         }
         ref = 1;
      }
      strMsg = strchr(strMsg, '=');
      if(strMsg == NULL)
         break;
      tagValue.second = strMsg+1;
      Values_.push_back(tagValue);
      
      switch(tagValue.first) {
      case fTag_MsgType:         iMsgType_         = Values_.end()-1;   break;
      case fTag_MsgSeqNum:       iMsgSeqNum_       = Values_.end()-1;  	break;
      case fTag_PossDupFlag:     iPossDupFlag      = Values_.end()-1;  	break;
      case fTag_PossResend:      iPossResend       = Values_.end()-1;  	break;
      case fTag_SenderCompID:    iSenderCompID_    = Values_.end()-1;  	break;
      case fTag_TargetCompID:    iTargetCompID_    = Values_.end()-1;  	break;
      case fTag_SenderSubID:     iSenderSubID_     = Values_.end()-1;  	break;
      case fTag_TargetSubID:     iTargetSubID_     = Values_.end()-1;  	break;
      case fTag_SendingTime:     iSendingTime_     = Values_.end()-1;  	break;
      case fTag_OrigSendingTime: iOrigSendingTime_ = Values_.end()-1; 	break;
      case fTag_CheckSum:        iCheckSum         = Values_.end()-1;  	break;
      case fTag_BeginString:	   break;
      case fTag_BodyLength:	   break;
      default:			   
         //if(iBodyBegin_==TFixValueI(0))
         //   iBodyBegin_ = Values_.end()-1;
         break;
      }
      
      strMsg = strchr(strMsg, FixSOH);
      if(strMsg == NULL)
         break;
      *strMsg = 0;
      ++strMsg;
      if(strMsg == NULL)
        break;
   }
//   if(iBodyBegin_ == TFixValueI(0))
//     iBodyBegin_ = Values_.begin();
   iBodyBegin_  = (Values_.begin()!= TFixValueI(0) ? Values_.begin()             : Values_.end());
   iBodyEnd_    = (iCheckSum     != TFixValueI(0) ? iCheckSum                    : Values_.end());
   MsgSeqNum_   = (iMsgSeqNum_   != TFixValueI(0) ? atoi(iMsgSeqNum_->second)    : 0);
   PossDupFlag_ = (iPossDupFlag  != TFixValueI(0) ? *(iPossDupFlag->second)=='Y' : false);
   PossResend_  = (iPossResend   != TFixValueI(0) ? *(iPossResend->second) =='Y' : false);
   /*if(dupTag)
   {
      aSes->SendReject(*this, srrTagAppearsMoreThenOnce, dupTag);
      return false;
   }*/
   return true;
}
//---------------------------------------------------------------------------
K_mf(TFixValueI)  TFixMsg::GetBodyTag(TFixTag aTag, TStkSesFix* aSes) const
{
   for(TFixValueI tag = iBodyBegin_; tag != iBodyEnd_; ++tag)
      if(tag->first == aTag)
         return tag;
   if(aSes)
      aSes->SendReject(*this, srrRequiredTagMissing, aTag);
   return TFixValueI(0);
}
/////////////////////////////////////////////////////////////////////////////
/*K_ctor TFixHeader::TFixHeader (const TFixMsg& aMsg)
{
  Assign(aMsg);
}
//---------------------------------------------------------------------------
K_mf(void) TFixHeader::Assign (const TFixMsg& aMsg)
{
  for(TFixValueCI iter=aMsg.Values_.begin(); iter!=aMsg.Values_.end(); iter++)
    switch(iter->first)
    {
      case fTag_OnBehalfOfCompID:
      case fTag_DeliverToCompID:
      case fTag_SenderSubID:
      case fTag_SenderLocationID:
      case fTag_TargetSubID:
      case fTag_TargetLocationID:
      case fTag_OnBehalfOfSubID:
      case fTag_OnBehalfOfLocationID:
      case fTag_DeliverToSubID:
      case fTag_DeliverToLocationID:
      case fTag_SendingTime:
      case fTag_OnBehalfOfSendingTime:
        if(iter->second)
          Values_.insert(TFixHeaderValueV(iter->first, iter->second));
        break;
    }
}
//---------------------------------------------------------------------------
K_mf(std::string) TFixHeader::GetValue(TFixTag tag)
{
  TFixHeaderValueI iter = Values_.find(tag);
  return (iter==Values_.end() ? std::string():iter->second);
}
//---------------------------------------------------------------------------
K_mf(bool) TFixHeader::TagExist(TFixTag tag)
{
  return (Values_.find(tag) != Values_.end());
}
//---------------------------------------------------------------------------
K_mf(void) TFixHeader::AssignAck(SFixStr& aMsg)
{
  for(TFixHeaderValueI iter=Values_.begin(); iter!=Values_.end(); iter++)
    switch(iter->first)
    {
      case fTag_DeliverToCompID:    aMsg.Append(fTag_OnBehalfOfCompID,     iter->second.c_str());   break;
      case fTag_SenderSubID:        aMsg.Append(fTag_TargetSubID,          iter->second.c_str());   break;
      case fTag_SenderLocationID:   aMsg.Append(fTag_TargetLocationID,     iter->second.c_str());   break;
      case fTag_DeliverToSubID:     aMsg.Append(fTag_OnBehalfOfSubID,      iter->second.c_str());   break;
      case fTag_DeliverToLocationID:aMsg.Append(fTag_OnBehalfOfLocationID, iter->second.c_str());   break;
      case fTag_SendingTime:
        if(TagExist(fTag_DeliverToCompID))
          aMsg.Append(fTag_OnBehalfOfSendingTime,iter->second.c_str());
        break;
    }
}*/
/////////////////////////////////////////////////////////////////////////////


