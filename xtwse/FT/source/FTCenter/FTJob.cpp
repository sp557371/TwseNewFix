//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FTJob.h"
#include <stdlib.h>
#include "FTSysSetting.h"
#include "bxSesMgr.h"
#include "FTSchedule.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
/////////////////////////////////////////////////////////////////////////////
K_dtor TFTMsgReq::~TFTMsgReq()
{
}
//---------------------------------------------------------------------------
K_mf(TMsgData&) TFTMsgReq::GetReqMsg()
{
  Flow_ = effSendMsg;
  return Work_.SrcData_;
}
//---------------------------------------------------------------------------
K_mf(void)        TFTMsgReq::SetReqMsg(const TMsgData& data)
{
  Flow_ = effRecvMsg;
  Work_.SrcData_ =  data;
}
//---------------------------------------------------------------------------
K_mf(TMsgData&) TFTMsgReq::GetRpyMsg()
{
  Flow_ = effSendMsg;
  return Work_.DstData_;
}
//---------------------------------------------------------------------------
K_mf(void)        TFTMsgReq::SetRpyMsg(const TMsgData& data)
{
  Flow_ = effRecvMsg;
  Work_.DstData_ = data;
}
//---------------------------------------------------------------------------
K_mf(void)        TFTMsgReq::OnFinish()
{
}
/////////////////////////////////////////////////////////////////////////////
K_dtor TFTFileReq::TFTFileReq(const TFTJob& job)
    : inherited(job),
    UpLoad_(NULL),
    DnLoad_(NULL)
{
}
//---------------------------------------------------------------------------
K_dtor TFTFileReq::~TFTFileReq()
{
  FreeDownLoad();
  FreeUpLoad  ();
}
//---------------------------------------------------------------------------
K_mf(void) TFTFileReq::InitDownLoad (size_t blocksize, size_t filesize)
{
  if(DnLoad_ == NULL)
  {
    TFTSysSetting& set = const_cast<TFTSysSetting&>(GetSysSetting());
    std::string path = set.GetSetting(ssi_TempSave);
    MakePath(path);
    std::string fname= path + Work_.DstData_.as_string();
    DnLoad_   = new TFileWrite(fname.c_str(), blocksize, filesize, 
                               set.IsAppendMode(GetFileCode()),set.GetRecLength(GetFileCode()));
    FileSize_ = filesize;
  }
}
//---------------------------------------------------------------------------
K_mf(bool) TFTFileReq::InitUpLoad   (size_t blocksize, size_t recsize)
{
  if(UpLoad_ == NULL)
  {
    TFTSysSetting& set = const_cast<TFTSysSetting&>(GetSysSetting());
    std::string srcpath = set.GetSetting(ssi_SaveTo);
    std::string dstpath = set.GetSetting(ssi_TempSave);
    MakePath(srcpath);
    MakePath(dstpath);
    std::string src = srcpath + Work_.SrcData_.as_string();
    std::string dst = dstpath + Work_.SrcData_.as_string();
    TFileFilter filter(src.c_str(), dst.c_str());
    if(filter.Transfer(recsize) > 0)
    {
      UpLoad_   = new TFileRead(dst.c_str(), blocksize);
      FileSize_ = UpLoad_->GetFileSize();
      return true;
    }
  }
  return false;
}
//---------------------------------------------------------------------------
K_mf(void) TFTFileReq::FreeDownLoad ()
{
  if(DnLoad_)
  {
    delete DnLoad_;
    DnLoad_ = NULL;
  }
}
//---------------------------------------------------------------------------
K_mf(void) TFTFileReq::FreeUpLoad   ()
{
  if(UpLoad_)
  {
    delete UpLoad_;
    UpLoad_ = NULL;
  }
}
//---------------------------------------------------------------------------
K_mf(int)  TFTFileReq::DownLoad     (const char* data, size_t size)
{
  return (DnLoad_ ? (int)DnLoad_->WriteTo(data, size):-1);
}
//---------------------------------------------------------------------------
K_mf(int)  TFTFileReq::UpLoad       (char* buffer, size_t size)
{
  return (UpLoad_ ? (int)UpLoad_->ReadFrom(buffer, size):-1);
}
//---------------------------------------------------------------------------
K_mf(TFileSize) TFTFileReq::GetFileSize()
{
  TFileSize sz;
  char buf[16];
  sprintf(buf, "%0*d", sizeof(TFileSize), FileSize_);
  sz.assign(buf);
  return sz;
}
//---------------------------------------------------------------------------
K_mf(void)      TFTFileReq::OnFinish()
{
  TDispatchMgr&    mgr = const_cast<TDispatchMgr&>(GetDispatchMgr());
  TFTSysSetting&   set = const_cast<TFTSysSetting&>(GetSysSetting());
  std::string  srcpath = set.GetSetting(ssi_TempSave); 
  MakePath(srcpath);
  if(Work_.Mode_ == ftm_Schedule){
     if(Work_.Type_ == ftt_Msg)    // 下載要求
        (const_cast<TScheduleCenter*>(&GetSchedule()))->UpdateSchedule(Work_.SrcSeqNo_);
     if(Work_.Type_ == ftt_File)   //  上傳要求
        (const_cast<TScheduleCenter*>(&GetSchedule()))->UpdateULoadSch(Work_.SrcSeqNo_);
  }
  std::string srcfile = srcpath + Work_.DstData_.as_string();
  if(!Work_.DstData_.empty())
     mgr.DoDispatch(Work_, srcfile);  // dispatch
  if(Work_.FileCode_.as_string() == "R40")
    GetBxMgr()->RptRecover((const TSysID&)SysOrdID_, srcfile, Work_.SourceID_, Work_.Market_);
  if(Work_.FileCode_.as_string() == "B36")  // tse pvc 配置表
    GetBxMgr()->B36Echo(srcfile, Work_.SourceID_, Work_.Market_);
  if(Work_.FileCode_.as_string() == "B37")  // tse&otc 變更密碼&回覆
    GetBxMgr()->B37Echo(srcfile, Work_.SourceID_, Work_.Market_);
  if(Work_.FileCode_.as_string() == "B38")  // otc pvc 配置表
    GetBxMgr()->B36Echo(srcfile, Work_.SourceID_, Work_.Market_);
}
//---------------------------------------------------------------------------
