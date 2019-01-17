//---------------------------------------------------------------------------
#ifndef bxLinkWorkerH
#define bxLinkWorkerH
//---------------------------------------------------------------------------
#include "bxHeadDef.h"
#include "bxLink.h"
#include "bxSes.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
K_class TbxLinkSes;
//連線子系統
//---------------------------------------------------------------------------
K_class TWork_Link : public TWorker
{
    typedef TWorker      inherited;
    typedef TWork_Link   this_type;
protected:
            K_mf(int)   GetPacketSize(TbxSesBase&, const TbxData&);
    virtual K_mf(bool)  APacket      (TbxSesBase&, const TbxData&);
    virtual K_mf(int)   BeforeAPacket(TbxSesBase&, const TbxData&) { return 0; }
    virtual K_mf(void)  AfterAPacket (TbxSesBase&, const TbxData&) {}
    virtual K_mf(bool)  DoJob        (TbxLinkSes&, const TControlHead&) = 0;
public:
            K_mf(bool)  DoReq        (TbxSesBase&, const TBxMsg, const TControlHead&);
    virtual K_mf(bool)  DoWorker     (TbxLinkSes&, const TBxMsg, const TControlHead&) = 0;
};
//---------------------------------------------------------------------------
K_class TWork_L1010 : public TWork_Link //開機通知作業(L010,L020)
{
    typedef TWork_Link   inherited;
    typedef TWork_L1010  this_type;
protected:
    K_mf(bool) DoJob   (TbxLinkSes&, const TControlHead&);
public:
    K_mf(bool) DoWorker(TbxLinkSes&, const TBxMsg, const TControlHead&);
};
//---------------------------------------------------------------------------
K_class TWork_L1020 : public TWork_Link //登入作業(L030,L040,L050,L060)
{
   typedef TWork_Link   inherited;
   typedef TWork_L1020  this_type;
protected:
    K_mf(bool) DoJob   (TbxLinkSes&, const TControlHead&);
public:
    K_mf(bool) DoWorker(TbxLinkSes&, const TBxMsg, const TControlHead&);
};
//---------------------------------------------------------------------------
K_class TWork_L1030 : public TWork_Link //離線作業(L070,l080)
{
   typedef TWork_Link   inherited;
   typedef TWork_L1030  this_type;
protected:
    K_mf(bool) DoJob   (TbxLinkSes&, const TControlHead&);
public:
    K_mf(bool) DoWorker(TbxLinkSes&, const TBxMsg, const TControlHead&);
};
//---------------------------------------------------------------------------
};  // Bx
};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif



