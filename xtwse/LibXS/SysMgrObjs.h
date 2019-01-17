//
// �t�Ωw�q���򥻺޲z����, �i�q GetSysMgrObjs(); ���o
//
// ����: $Id: SysMgrObjs.h,v 1.6 2005/05/18 03:12:06 fonwin Exp $
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

   K_mf(void)  StartModuleLoader  ();//�Y���ƩI�s,�u���Ĥ@������,��l���|���������

   std::string                   ConfigPath_;
   std::auto_ptr<TSesMgr>        SesMgr_;
   std::auto_ptr<TViewAreaMgr>   SysVAMgr_;  //0.�t�κ޲z
   std::auto_ptr<TViewAreaMgr>   MainVAMgr_;
   class TModuleLoader;
   std::auto_ptr<TModuleLoader>  ModuleLoader_; //������b�̫�غc,�קK���J�Ҳզ��Ψ� SesMgr_,SysVAMgr_.
   word                          HostID_;//�D��ID, �q configPath + "local.ini" �� hostid=n ���J

   K_mf(TVACreater&)  GetModuleLoaderVACreater  () const;

   ///> �]�w KeyObject, �Y obj->GetName() ������, �h�]�w����, �Ǧ^ false.
   K_mf(bool) SetKeyObject (const std::string& name, TKeyObject* obj) const;
   ///> �M�����w�� KeyObject
   K_mf(bool) ClrKeyObject (const std::string& name, TKeyObject*) const;
   ///> ���o���w�� KeyObject, �Y���w�W�٪�KeyObject���s�b, �h�Ǧ^ NULL.
   K_mf(TKeyObject*) GetKeyObject (const std::string& name) const;
   template<class T>
      K_mf(T*) GetKeyObjectT (const std::string& name) const {
         return dynamic_cast<T*>(GetKeyObject(name));
      }
private:
   /** �T��ƻs.
   */
   explicit K_ctor    TSysMgrObjs (const TSysMgrObjs&);
   K_mf(TSysMgrObjs&) operator=   (const TSysMgrObjs&);

   typedef std::map<std::string, TKeyObject*>  TKeyObjects;
   mutable TKeyObjects KeyObjects_;
};
//---------------------------------------------------------------------------
/**
   �Y�z�ݭn�b TSysMgrObjs ���e,���ǨƱ�,�h�i�~�Ӧ�class.
   ���O,�Y�z�@���S���Ұ� GetSysMgrObjs(),
        �hAP������,���|���� OnSysMgrObjsDestroy() �ƥ�!!
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
LIBNS_gd(TPrtMsgFn) SysPrtVrbose;//��ܨt�αҰ�,�����T��
LIBNS_fn(void) SetDefault_SysPrtVrbose ();
//---------------------------------------------------------------------------
LIBNS_fn(const TSysMgrObjs&)  GetSysMgrObjs  ();
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

