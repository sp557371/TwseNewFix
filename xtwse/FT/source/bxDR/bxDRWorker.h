//---------------------------------------------------------------------------
#ifndef bxDRWorkerH
#define bxDRWorkerH
//---------------------------------------------------------------------------
#include "bxHeadDef.h"
#include "bxSes.h"
//---------------------------------------------------------------------------
namespace Kway  {
namespace Tw    {
namespace Bx    {
namespace DR    {
//����^���l�t��
K_class TbxDRSes;
/////////////////////////////////////////////////////////////////////////////
K_class TWork_RPT : public TWorker  // RPT Base
{
   typedef TWorker      inherited;
   typedef TWork_RPT    this_type;
protected:
    virtual K_mf(bool)  APacket      (TbxSesBase&, const TbxData&);
    virtual K_mf(void)  AfterAPacket (TbxSesBase&, const TbxData&) {}
    virtual K_mf(bool)  DoReq        (TbxSesBase&, const TBxMsg, const TControlHead&);
    virtual K_mf(bool)  OnRptPkt     (TbxDRSes&, const TControlHead&) = 0;
public:
    virtual K_mf(int)   GetPacketSize(TbxSesBase&, const TbxData&);
    virtual K_mf(int)   BeforeAPacket(TbxSesBase&, const TbxData&);
    virtual K_mf(bool)  DoRptReq     (TbxDRSes&, const TBxMsg, const TControlHead&) = 0;
};
/////////////////////////////////////////////////////////////////////////////
K_class TWork_R5000 : public TWork_RPT  //�s�u�T��(R1,R2,R4,R5)
{
   typedef TWorker      inherited;
   typedef TWork_R5000  this_type;
protected:
    virtual K_mf(bool)  OnRptPkt     (TbxDRSes&, const TControlHead&);
public:
    virtual K_mf(bool)  DoRptReq     (TbxDRSes&, const TBxMsg, const TControlHead&);
};
/////////////////////////////////////////////////////////////////////////////
K_class TWork_R5010 : public TWork_RPT  //����^�����(R3)
{
   typedef TWorker      inherited;
   typedef TWork_R5010  this_type;
protected:
    virtual K_mf(bool)  OnRptPkt     (TbxDRSes&, const TControlHead&);
public:
    // R3 �u���Q�ʺ�, ���ݭn����n�D
    virtual K_mf(bool)  DoRptReq     (TbxDRSes&, const TBxMsg, const TControlHead&) { return true; }
};
/////////////////////////////////////////////////////////////////////////////
K_class TWork_R5020 : public TWork_RPT  //����^�������T��(R6)
{
   typedef TWorker      inherited;
   typedef TWork_R5020  this_type;
protected:
    virtual K_mf(bool)  OnRptPkt     (TbxDRSes&, const TControlHead&);
public:
    // R6 �u���Q�ʺ�, ���ݭn����n�D
    virtual K_mf(bool)  DoRptReq     (TbxDRSes&, const TBxMsg, const TControlHead&) { return true; }
};
//---------------------------------------------------------------------------
}; // DR
}; // Bx
}; // Tw
}; // Kway
//---------------------------------------------------------------------------
#endif
