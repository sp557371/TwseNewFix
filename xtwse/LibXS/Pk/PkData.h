//---------------------------------------------------------------------------
#ifndef PkDataH
#define PkDataH
//---------------------------------------------------------------------------
#include "kpersistent.h"
#include "SesClient.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
//---------------------------------------------------------------------------
static const TMsgCode   mcData = 0x003;
/////////////////////////////////////////////////////////////////////////////
/** �qServer�Ӫ��ʥ],�z�L���U���{�Ǳ���.
*/
template <class TPkFrSvr, class TPkFrSvrFunc, TMsgCode mcBase>
class TWorkerPkFrSvr_Base : public TWorker
{
   typedef TWorker   inherited;
   K_mf(TCheckResult)  OnPkCheck (TSesPk& ses, const THead& pk, TSize recvSize)
      {  return PkCheckHiLoRef( static_cast<const TPkFrSvr&>(pk), recvSize
                              , static_cast<TSesClient&>(ses).GetServerHiLoRef() );
      }
public:
   inline static K_mf(unsigned)  GetDatLen  (TSesPk& ses, const TPkFrSvr& pk)
      {  return pk.GetDatLen(static_cast<TSesClient&>(ses).GetServerHiLoRef());
      }

   typedef TPkFrSvr     TPkType;
   typedef TPkFrSvrFunc TPkFunc;

   K_ctor  TWorkerPkFrSvr_Base  (TWorker* regs[], TPkFunc func)
      :  inherited(regs, TMsgCode(mcBase+func))
      {  }

   K_ctor  TWorkerPkFrSvr_Base  (TWorker* regs[], TMsgCode mc)
      :  inherited(regs, mc)
      {  }
};
//--------------------------------------------------------------------------
/** �qClient�Ӫ��ʥ],�z�L���U���{�Ǳ���.
*/
template <class TPkFrCli, class TPkFrCliFunc, TMsgCode mcBase>
class TWorkerPkFrCli_Base : public TWorker
{
   typedef TWorker   inherited;
protected:
   K_mf(TCheckResult)  OnPkCheck (TSesPk&, const THead& pk, TSize recvSize)
      {  return PkCheck(static_cast<const TPkFrCli&>(pk), recvSize);
      }
public:
   inline static K_mf(unsigned)  GetDatLen  (const TSesPk&, const TPkFrCli& pk)
      {  return pk.GetDatLen();
      }

   typedef TPkFrCli     TPkType;
   typedef TPkFrCliFunc TPkFunc;

   K_ctor  TWorkerPkFrCli_Base  (TWorker* regs[], TPkFunc func)
      :  inherited(regs, TMsgCode(mcBase+func))
      {  }

   K_ctor  TWorkerPkFrCli_Base  (TWorker* regs[], TMsgCode mc)
      :  inherited(regs, mc)
      {  }
};
//--------------------------------------------------------------------------
/** �j�q��ƫʥ]���B�z(FuncCode = mcData ���B�z�禡):
      �ʥ]����; PkFunc(mcData) + PkFunc(mcData) +... + PkFunc(���FuncCode).
   TPkWorkerBase �q�`�O TWorkerPkFrCli_Base<>, TWorkerPkFrSvr_Base<>.
*/
template<class TPkWorkerBase>
class TWorkerPk_Data : public TPkWorkerBase
{
   typedef TPkWorkerBase                  inherited;
   typedef typename inherited::TPkType    TPkType;
   typedef typename inherited::TPkFunc    TPkFunc;

   Kstl::persistent_auto   RecvData_;
   Kstl::persistent        LastPer_;
protected:
   K_mf(void)  OnPkRecv  (TSesPk& ses, const THead& pk)
      {  const TPkType& pkt = static_cast<const TPkType&>(pk);
         RecvData_.save(pkt.GetDat(), GetDatLen(ses,pkt), false);
      }
   K_mf(void)  ResetRecv  ()
      {  RecvData_.rewind();
      }
   K_mf(Kstl::persistent&)  FetchRecvData  (const TPkType& pkt, unsigned len)
      {  if(RecvData_.get_saved_size()) {
            RecvData_.save(static_cast<const char*>(pkt.GetDat()), len, false);
            LastPer_.reset(RecvData_.getbuf(), RecvData_.get_saved_size());
            RecvData_.rewind();
         } else
            LastPer_.reset(const_cast<char*>(static_cast<const char*>(pkt.GetDat())), len);
         return LastPer_;
      }
public:
   /** �غc.
   */
   K_ctor TWorkerPk_Data (Kway::Pk::TWorker* regs[], TMsgCode msgCode = mcData)
      :  inherited(regs, msgCode)
      ,  RecvData_(1024*4)
      ,  LastPer_(0,0)
      {  }

   K_ctor TWorkerPk_Data (Kway::Pk::TWorker* regs[], TPkFunc pkFunc)
      :  inherited(regs, pkFunc)
      ,  RecvData_(1024*4)
      ,  LastPer_(0,0)
      {  }
   /** Server�ݨ��o���쪺���.
   */
   inline K_mf(Kstl::persistent&)  FetchRecvData  (const TPkType& pkt)
      {  return FetchRecvData(pkt, pkt.GetDatLen());
      }
   /** Client�ݨ��o���쪺���.
   */
   inline K_mf(Kstl::persistent&)  FetchRecvData  (const TPkType& pkt, Kway::THiLoTransRef hiLoTrans)
      {  LastPer_.resetHiLoTrans(hiLoTrans);
         return FetchRecvData(pkt, pkt.GetDatLen(hiLoTrans));
      }
};
//-----------------------------------------------------------
/** �w�q�Y FuncCode ���B�z�禡.
    Base �q�`�O TWorkerPkFrCli_Base<>, TWorkerPkFrSvr_Base<>.
*/
#define TWorkerPk_DEFINE(eFunc,Base)                        \
class TWorkerPk_##eFunc : public Base                       \
{                                                           \
   typedef Base                  inherited;                 \
   typedef inherited::TPkType    TPkType;                   \
   K_mf(void) OnPkRecv (TSesPk& ses, const THead& pk)       \
      {  On_##eFunc(ses, static_cast<const TPkType&>(pk));  \
      }                                                     \
   virtual K_mf(void) On_##eFunc (TSesPk&,const TPkType&)=0;\
public:                                                     \
   K_ctor TWorkerPk_##eFunc (Kway::Pk::TWorker* regs[])     \
      :  inherited(regs, TPkFunc(eFunc))                    \
      {  }                                                  \
};                                                          \
/////////////////////////////////////////////////////////////////////////////
/** ���j�q��Ʈ�, �@�� TPkXXOMS �i�ण����, ���ɽХ�:
    �z�� TPk �����䴩:
      K_ctor      TPk       ();//�w�]�غc.
      K_mf(void)  SetDatLen (TSize, THiLoTransRef);
      K_mf(void*) GetDat    ();
*/
template <class TPk>
class TPkData : public Kstl::persistent
{
   typedef Kstl::persistent   inherited;
   TPk      Pk_;
   TMsgCode MsgCode_;
   TSesPk*  Ses_;
   bool     mcDataSent_;//2009/03/26 �ѨM�T���j�p��4090�����Ʈ�,�L�k�e�X�����D

   /** Buffer�s���U�F,���e�XPk,�M��NPk�^���A�Q��.
   */
   K_mf(void*) ReallocWrBuf (unsigned& sz)
      {  Pk_.SetMsgCode(mcData);
         Pk_.SetDatLen(TSize(get_saved_size()), GetHiLoTrans());
         Ses_->SendPk(Pk_);
         void* newBuf = Pk_.GetDat();
         reset(newBuf, sz = TPk::GetMaxDatSize());
         return newBuf;
      }

public:
   /** msgCode = ��ڭn�B�z�� MsgCode.
   */
   inline K_ctor TPkData (TMsgCode msgCode, TSesPk& ses, THiLoTransRef hiLoTrans = THiLoTransRef(THiLoTransRef::DwordLocalMachine_))
      :  inherited(Pk_.GetDat(), TPk::GetMaxDatSize(), hiLoTrans)
      ,  Pk_()
      ,  MsgCode_(msgCode)
      ,  Ses_(&ses)
      ,  mcDataSent_(false)
      {  }

   /** �Ѻc��,�YPk�|���ǰe,�h�e�X.
   */
   K_dtor ~TPkData ()
      {  if(mcDataSent_ || get_saved_size() > 0) {
            Pk_.SetMsgCode(MsgCode_);
            Pk_.SetDatLen(TSize(get_saved_size()), GetHiLoTrans());
            Ses_->SendPk(Pk_);
         }
      }

   K_mf(bool) save (const void* vsrc, unsigned srcSz, bool chkTrans) {
      if(inherited::save(vsrc, srcSz, chkTrans))
         return true;
      if(chkTrans || get_saved_size()!=0)
         return false;
      //�����s���t�x�s��,��������!
      unsigned bufsz = getbuf_size();
      if(srcSz > bufsz) {
         mcDataSent_ = true;
         Pk_.SetMsgCode(mcData);
         Pk_.SetDatLen(TSize(bufsz), GetHiLoTrans());
         void* pkbuf = Pk_.GetDat();
         for(const char* psrc=(const char*)vsrc;;) {
            memcpy(pkbuf, psrc, bufsz);
            Ses_->SendPk(Pk_);
            if((int)(srcSz -= bufsz) <= 0)
               break;
            psrc += bufsz;
            if(bufsz > srcSz)
               return inherited::save(psrc, srcSz, chkTrans);
         }
         return true;
      }
      return false;
   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

