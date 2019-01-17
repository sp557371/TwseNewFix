//
// �q�Tclasses��¦: Session-Messager
//    Messager ���U
//
// ����: $Id: MesRegs.h,v 1.2 2003/12/29 07:45:15 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef MesRegsH
#define MesRegsH
//---------------------------------------------------------------------------
#include "ViewArea.h"
#include "Mes.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
struct TMesReg
{
   typedef TMesPtr  (*TMesCreater)  ();

   const char*    Name_;
   TMesCreater    MesFactory_;
   bool           IsServer_;

   //�]�w�s�边
   TVAGetEditProp*   SettingEditor_;

   //�YSetting���]�t�K�X��,�s��ɪ��r��,�P��ܪ��r��,���Ӥ��|�@�˧a!
   typedef std::string (*TGetShowSetting) (const std::string&);
   TGetShowSetting      GetShowSetting_;
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesRegs
{
   const TMesReg**   begin_;
   const TMesReg**   end_;

   inline K_ctor  TMesRegs  (const TMesReg** begin, const TMesReg** end)
                     : begin_(begin),
                       end_(end)
                     { }
public:
   static K_mf(TMesRegs)  GetDefaultMesRegs  ();  //�]�t������Mes: �tClient��,�PServer��
   static K_mf(TMesRegs)  GetClientMesRegs   ();
   static K_mf(TMesRegs)  GetServerMesRegs   ();

   inline K_mf(const TMesReg**)  Begin  () const    { return begin_; }
   inline K_mf(const TMesReg**)  End    () const    { return end_;   }

   K_mf(const TMesReg*)  Find  (const std::string& mesName) const;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
