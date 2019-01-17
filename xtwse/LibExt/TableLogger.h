//---------------------------------------------------------------------------
#ifndef TableLoggerH
#define TableLoggerH
//---------------------------------------------------------------------------
#include "VALogger.h"
#include "KFile.h"
#include "SesServer.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
template<class TFMaker>
class TTableLogger : public TTableLog,
                       public TTableLog2d
{
private:
   TFile               Log_;
   char                Buf_[256];
   TFMaker             FMaker_;
public:
           K_ctor      TTableLogger(const char* fname){
              Log_.Open(fname, (TFileMode)(fmOpenAlways|fmAppend));
           }
   virtual K_dtor     ~TTableLogger (){
              Log_.Close();
           }
   virtual K_mf(void)  WriteMod  (const std::string& key, size_t fieldNo, const std::string& bf,
                                  const std::string& af, TViewArea* va){
              sprintf(Buf_,"Mod {%s}:[%s-%s] %s ==> %s", GetTreeID(va).c_str(), key.c_str(), 
                                                         FMaker_.GetFieldDesp(fieldNo).Title_, bf.c_str(), af.c_str());
              WriteBuf(Buf_);
           }
   virtual K_mf(void)  WriteAdd  (const std::string& key,TViewArea* va){
              sprintf(Buf_,"Add {%s}:[%s]", GetTreeID(va).c_str(), key.c_str());
              WriteBuf(Buf_);
           }
   virtual K_mf(void)  WriteDel  (const std::string& key,TViewArea* va){
              sprintf(Buf_,"Del {%s}:[%s]", GetTreeID(va).c_str(), key.c_str());
              WriteBuf(Buf_);
           }
   virtual K_mf(void)  WriteMod  (const std::string& masterKey, const std::string& detailKey, size_t fieldNo,
                                  const std::string& bf, const std::string& af, TViewArea* va){
              sprintf(Buf_,"Mod {%s->%s}:[%s-%s] %s ==> %s", GetTreeID(va).c_str(), masterKey.c_str(), detailKey.c_str()
                                                  , FMaker_.GetFieldDesp(fieldNo).Title_, bf.c_str(), af.c_str());
              WriteBuf(Buf_);
           }
   virtual K_mf(void)  WriteAdd  (const std::string& masterKey, const std::string& detailKey,TViewArea* va){
              sprintf(Buf_,"Add {%s->%s}:[%s]", GetTreeID(va).c_str(), masterKey.c_str(), detailKey.c_str());
              WriteBuf(Buf_);
           }
   virtual K_mf(void)  WriteDel  (const std::string& masterKey, const std::string& detailKey,TViewArea* va){
              sprintf(Buf_,"Del {%s->%s}:[%s]", GetTreeID(va).c_str(), masterKey.c_str(), detailKey.c_str());
              WriteBuf(Buf_);
           }
   virtual K_mf(void)  WriteBuf  (const char* buf){
              char tmp[512];
              std::string uid = Kway::Pk::TSesServer::GetLastOpUser();
              if(uid.empty() || (!Log_.IsOpened()))    // 不是由VA 所產生的修改或開檔失敗則不做記錄
                 return;
              sprintf(tmp,"%s (%-12s) %s\n", TKDateTime::Now().as_string(TKDate::sf_ELong,0).c_str(), uid.c_str(), buf);
              Log_.Seek(0,fsEnd);
              Log_.Write(tmp);
           }
           K_mf(std::string)     GetTreeID(TViewArea* va){
              std::string treeId;
              if(va){
//                 treeId = va->GetIdName();
                 while(TViewArea* p = va->GetParentVA()){
                    treeId = p->GetCurRow()->GetStr(0)+(treeId.empty()?"":"->")+treeId;
                    va = p;
                 }
                 treeId = va->GetIdName()+"->"+treeId;
              }
              return treeId;
           }
};
//---------------------------------------------------------------------------
}; // namespace kway
//---------------------------------------------------------------------------
#endif
