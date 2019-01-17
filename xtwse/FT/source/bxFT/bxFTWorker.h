//---------------------------------------------------------------------------
#ifndef bxFTWorkerH
#define bxFTWorkerH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "bxSes.h"
#include "bxFT.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
K_class TbxFTSes;
//---------------------------------------------------------------------------
K_class TWork_FHead : public TWorker
{
    typedef TWorker      inherited;
    typedef TWork_FHead  this_type;
protected:
    virtual K_mf(bool) APacket      (TbxSesBase&, const TbxData&);
    virtual K_mf(void) AfterAPacket (TbxSesBase&, const TbxData&) {}
    virtual K_mf(bool) DoReq        (TbxSesBase&, const TBxMsg, const TControlHead&);
    // call
    virtual K_mf(bool) BodyPacket   (TbxFTSes&,   const TFTHead&) = 0;
public:
    virtual K_mf(bool) DoFtReq      (TbxFTSes&, const TBxMsg, const TFTHead&) { return true; }
    virtual K_mf(int)  GetPacketSize(TbxSesBase&, const TbxData&);
    virtual K_mf(int)  BeforeAPacket(TbxSesBase&, const TbxData&);
};
//---------------------------------------------------------------------------
K_class TWork_F2000 : public TWork_FHead    //起始作業(F010,F020)
{
    typedef TWorker      inherited;
    typedef TWork_F2000  this_type;
protected:
    K_mf(bool) BodyPacket(TbxFTSes&, const TFTHead&);
public:
    K_mf(bool) DoFtReq   (TbxFTSes&, const TBxMsg, const TFTHead&);
};
//---------------------------------------------------------------------------
K_class TWork_F2001 : public TWork_FHead    //檔案傳輸作業(F010,F020,F030,F040)
{
    typedef TWorker      inherited;
    typedef TWork_F2001  this_type;
protected:
    K_mf(bool) BodyPacket(TbxFTSes&, const TFTHead&);
public:
    K_mf(bool) DoFtReq   (TbxFTSes&, const TBxMsg, const TFTHead&);
};
//---------------------------------------------------------------------------
K_class TWork_F2002 : public TWork_FHead    //單筆訊息作業(F050,F060)
{
    typedef TWorker      inherited;
    typedef TWork_F2002  this_type;
protected:
    K_mf(bool) BodyPacket(TbxFTSes&, const TFTHead&);
public:
    K_mf(bool) DoFtReq   (TbxFTSes&, const TBxMsg, const TFTHead&);
};
//---------------------------------------------------------------------------
K_class TWork_F2003 : public TWork_FHead    //結束作業(F070,F080)
{
    typedef TWorker      inherited;
    typedef TWork_F2003  this_type;
protected:
    K_mf(bool) BodyPacket(TbxFTSes&, const TFTHead&);
public:
    K_mf(bool) DoFtReq   (TbxFTSes&, const TBxMsg, const TFTHead&);
};
//---------------------------------------------------------------------------
K_class TWork_F2004 : public TWork_FHead    //確定連線作業(F170,F180)
{
    typedef TWorker      inherited;
    typedef TWork_F2004  this_type;
protected:
    K_mf(bool) BodyPacket(TbxFTSes&, const TFTHead&);
public:
    K_mf(bool) DoFtReq   (TbxFTSes&, const TBxMsg, const TFTHead&);
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
