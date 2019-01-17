//---------------------------------------------------------------------------
#ifndef FTJobH
#define FTJobH
//---------------------------------------------------------------------------
#include "FTJobDef.h"
#include "FileCtrl.h"
#include "DspMgr.h"
#include "TwStkSysOrd.h"
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
// 一筆作業要求
K_class TFTReqBase
{
    typedef Kway::Tw::Stk::TSysOrdID  TSysOrdID;
protected:
    enum eFTFlow
    {
      effError,     // 錯誤
      effSendMsg,   // 送出單筆訊息
      effRecvMsg,   // 接收單筆訊息
      effSending,   // 傳送資料中
      effRecving,   // 接收資料中
      effFinish     // 作業結束
    };
    TSysOrdID   SysOrdID_;
    eFTFlow     Flow_;
    TFTJob      Work_;
public:
 inline K_ctor          TFTReqBase  (const TFTJob& job) : Work_(job) {}
virtual K_dtor          ~TFTReqBase ()   {}
virtual K_mf(void)       OnFinish   () = 0;
 inline K_mf(TFileCode&) GetFileCode()   { return Work_.FileCode_;   }
 inline K_mf(void)       SetFileCode(const TFileCode& code) { Work_.FileCode_ = code; }
 inline K_mf(TBrkID&)    GetSourceID()   { return Work_.SourceID_;   }
 inline K_mf(TBrkID&)    GetDestID  ()   { return Work_.DestID_;     }
 inline K_mf(void)       SetSysOrdID(const TSysOrdID& id) { SysOrdID_ = id; }
};
//---------------------------------------------------------------------------
K_class TFTMsgReq : public TFTReqBase
{
    typedef TFTReqBase  inherited;
    typedef TFTMsgReq   this_type;
protected:
public:
 inline K_ctor TFTMsgReq(const TFTJob& job) : inherited(job) {}
virtual K_dtor ~TFTMsgReq();
        K_mf(TMsgData&) GetReqMsg();
        K_mf(void)      SetReqMsg(const TMsgData&);
        K_mf(TMsgData&) GetRpyMsg();
        K_mf(void)      SetRpyMsg(const TMsgData&);
virtual K_mf(void)      OnFinish ();
};
//---------------------------------------------------------------------------
K_class TFTFileReq : public TFTReqBase
{
    typedef TFTReqBase  inherited;
    typedef TFTFileReq  this_type;
protected:
    TFileRead*  UpLoad_;    // 上傳
    TFileWrite* DnLoad_;    // 下載
    size_t      FileSize_;
public:
        K_ctor  TFTFileReq(const TFTJob&);
virtual K_dtor ~TFTFileReq();
virtual K_mf(void) OnFinish     ();
        K_mf(void) InitDownLoad (size_t blocksize, size_t filesize);
        K_mf(bool) InitUpLoad   (size_t blocksize, size_t recsize);
        K_mf(void) FreeDownLoad ();
        K_mf(void) FreeUpLoad   ();
        K_mf(int)  DownLoad     (const char* data, size_t size);
        K_mf(int)  UpLoad       (char* buffer, size_t size);
        K_mf(bool) IsEof()      { return(UpLoad_ ? UpLoad_->IsWorkOK():true); }
        K_mf(TFileSize) GetFileSize();
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
