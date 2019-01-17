//
// 通訊classes基礎: Session-Messager
//    class TMesFile;
//
// 版本: $Id: MesFile.h,v 1.2 2005/04/28 05:51:46 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef MesFileH
#define MesFileH
//---------------------------------------------------------------------------
#include "Mes.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesFile : public TMesBase
{
   typedef TMesBase  inherited;
public:
   K_ctor  TMesFile ();
   K_dtor ~TMesFile  ();

   //setting: filename<;[c][rwa][opentime][-closetime][,speed]>
   //                  ^別漏掉分號「;」囉!
   // c = create     :當檔案不存在時建立新檔
   // r = read only  :唯讀模式開檔, Send() 傳回 -1
   // w = overrwrite :覆寫模式開檔, Recv() 傳回 -1
   // a = append     :附加模式開檔, Recv() 傳回 -1
   // time format = hh:mm:ss
   // filename 可用 %4y 代表西元年, %2y 代表民國年, %2m 代表月, %2d代表日
   // speed : read only時使用, format = <bytes> * <times/sec>
   //         例如: 100*10 = 每秒讀10次,每次100bytes
   // default = read only,立即開啟,永不關閉,每秒讀取一次1024bytes
   K_mf(bool)         SettingApply  (const std::string&);
   K_mf(std::string)  GetSetting    ();
   // UID = filename-實際開啟的檔案名稱
   K_mf(std::string)  GetUID        ();
   K_mf(int)          Send          (const void* buffer, size_t size);
   K_mf(int)          Recv          (      void* buffer, size_t size);

protected:
   K_mf(bool)         OpenImpl      ();
   K_mf(bool)         CloseImpl     ();

private:
   class TImpl;
   friend class TImpl;
   TImpl*         Impl_;
   std::string    Setting_;
};
/////////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------------------------
#endif
