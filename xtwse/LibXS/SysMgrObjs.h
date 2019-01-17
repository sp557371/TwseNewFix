//
// 系統定義的基本管理物件, 可從 GetSysMgrObjs(); 取得
//
// 版本: $Id: SysMgrObjs.h,v 1.6 2005/05/18 03:12:06 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SysMgrObjsH
#define SysMgrObjsH
//---------------------------------------------------------------------------
#include <memory>
#include <string>
#include <map>
#include <stdio.h>
#include "KwayBase.h"
//---------------------------------------------------------------------------
namespace Kway {
LIBNS_class TSesMgr;
LIBNS_class TViewAreaMgr;
LIBNS_class TVACreater;
LIBNS_class TSysObj;
/////////////////////////////////////////////////////////////////////////////
/** 
*/
LIBNS_class TKeyObject
{
public:
   virtual K_dtor ~TKeyObject () {
   }
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSysMgrObjs
{
public:
   explicit K_ctor  TSysMgrObjs  (const std::string& configPath);
            K_dtor ~TSysMgrObjs  ();

   K_mf(void)  StartModuleLoader  ();//若重複呼叫,只有第一次有效,其餘不會有任何改變

   std::string                   ConfigPath_;
   std::auto_ptr<TSesMgr>        SesMgr_;
   std::auto_ptr<TViewAreaMgr>   SysVAMgr_;  //0.系統管理
   std::auto_ptr<TViewAreaMgr>   MainVAMgr_;
   class TModuleLoader;
   std::auto_ptr<TModuleLoader>  ModuleLoader_; //必須放在最後建構,避免載入模組有用到 SesMgr_,SysVAMgr_.
   word                          HostID_;//主機ID, 從 configPath + "local.ini" 的 hostid=n 載入

   K_mf(TVACreater&)  GetModuleLoaderVACreater  () const;

   ///> 設定 KeyObject, 若 obj->GetName() 有重複, 則設定失敗, 傳回 false.
   K_mf(bool) SetKeyObject (const std::string& name, TKeyObject* obj) const;
   ///> 清除指定的 KeyObject
   K_mf(bool) ClrKeyObject (const std::string& name, TKeyObject*) const;
   ///> 取得指定的 KeyObject, 若指定名稱的KeyObject不存在, 則傳回 NULL.
   K_mf(TKeyObject*) GetKeyObject (const std::string& name) const;
   template<class T>
      K_mf(T*) GetKeyObjectT (const std::string& name) const {
         return dynamic_cast<T*>(GetKeyObject(name));
      }
private:
   /** 禁止複製.
   */
   explicit K_ctor    TSysMgrObjs (const TSysMgrObjs&);
   K_mf(TSysMgrObjs&) operator=   (const TSysMgrObjs&);

   typedef std::map<std::string, TKeyObject*>  TKeyObjects;
   mutable TKeyObjects KeyObjects_;
};
//---------------------------------------------------------------------------
/**
   若您需要在 TSysMgrObjs 死前,做些事情,則可繼承此class.
   但是,若您一直沒有啟動 GetSysMgrObjs(),
        則AP結束時,不會產生 OnSysMgrObjsDestroy() 事件!!
*/
LIBNS_class TSysObj
{
   TSysObj* Next_;
   friend LIBNS_class TSysMgrObjs;
   virtual K_mf(void)  OnSysMgrObjsDestroy  (const TSysMgrObjs&) = 0;
   virtual K_mf(void)  OnSysMgrObjsCreated  (const TSysMgrObjs&) = 0;
protected:
   K_mf(void) CheckSysMgrObjsCreated ();
public:
           K_ctor  TSysObj ();
   virtual K_dtor ~TSysObj ();
};
//---------------------------------------------------------------------------
typedef void (*TPrtMsgFn) (const char* fmt, ...);
LIBNS_gd(TPrtMsgFn) SysPrtVrbose;//顯示系統啟動,結束訊息
LIBNS_fn(void) SetDefault_SysPrtVrbose ();
//---------------------------------------------------------------------------
LIBNS_fn(const TSysMgrObjs&)  GetSysMgrObjs  ();
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

