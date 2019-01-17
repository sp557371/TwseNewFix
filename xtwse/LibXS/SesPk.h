//
// �q�Tclasses��¦: Session-Messager
//    class TSesPk; �@�ذ򥻪��ʥ]���c
//    << �H�����ӥi�H��� template ���覡�]�p >>
//
// ����: $Id: SesPk.h,v 1.5 2006/04/04 03:27:31 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesPkH
#define SesPkH
//---------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma warning ( disable : 4200)
#endif
//---------------------------------------------------------------------------
#include "Ses.h"
#include "HiLoTrans.h"
#include "TimerThread.h"
#include "Signon/Policy/PolicyBase.h"
//---------------------------------------------------------------------------
#include <stdio.h> //for sprintf()
//---------------------------------------------------------------------------
//
// �w�ϥΪ� MsgCode �аѦ� "pkMsgCode.txt"
//
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TPolicyBaseRef;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSesPk;
//---------------------------------------------------------------------------
//#include "PkHeartbeat.h"
class THeartbeat;
/////////////////////////////////////////////////////////////////////////////
LIBNS_fn(word)  SetPolicyNames  (char* buf, const char** policyNamesBegin, size_t policyNamesCount);
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma warning ( disable : 4200 )
#endif
LIBNS_class TPolicyAck
{
   byte     PolicyNameLen_;
   word     PolicyDataLen_;
   char     Dat_[];
   //char   PolicyName_[];
   //byte   PolicyData_[];
public:
   //----- for server -----
   inline K_mf(word)  Set  (const std::string& policyName, const TPolicyBaseRef& ref)
         { memcpy(Dat_,                policyName.c_str(), PolicyNameLen_ = static_cast<byte>(policyName.length()));
           memcpy(Dat_+PolicyNameLen_, ref.first,          PolicyDataLen_ = ref.second);
           return GetSize(); }
   //----- for client -----
   inline K_mf(word)         GetSize       () const  { return static_cast<word>(sizeof(*this) + PolicyNameLen_ + PolicyDataLen_); }
   inline K_mf(std::string)  GetPolicyName () const  { return std::string(Dat_, PolicyNameLen_); }
   inline K_mf(const void*)  GetPolicyData () const  { return(Dat_ + PolicyNameLen_); }
   inline K_mf(word)         GetPolicyLen  () const  { return PolicyDataLen_; }
   inline K_mf(void)       TransPolicyLen  (THiLoTransRef ref)   { PolicyDataLen_ = HiLoTransT(PolicyDataLen_,ref); }
};
/////////////////////////////////////////////////////////////////////////////
struct TVer  //�{������
{
   word  Major_;
   word  Minor_;
   word  Release_;
   word  Build_;

   inline K_ctor  TVer  (word ma=0, word mi=0, word r=0, word b=0)
            : Major_(ma), Minor_(mi), Release_(r), Build_(b)
            { }

   inline K_mf(bool) IsNull () const {
      return Major_==0 && Minor_==0 && Release_==0 && Build_==0;
   }

   inline K_mf(std::string)  as_string  () const
            { char   buf[128];
#if _MSC_VER >= 1400
			  sprintf_s(buf, sizeof(buf), "%d.%d.%d.%d", Major_, Minor_, Release_, Build_);
#else
			  sprintf(buf, "%d.%d.%d.%d", Major_, Minor_, Release_, Build_);
#endif
              return std::string(buf); }
};
/////////////////////////////////////////////////////////////////////////////
typedef word   TSize;   //�]�t THead ���T������
typedef word   TMsgCode;
//---------------------------------------------------------------------------
class THead
{  //�ʥ]�j�p1.5byte:0��0xfff(4095)
   //�T���N�X1.5byte
   typedef THead   this_type;
   union {
      byte     Header_[3];
      struct {
         byte  DatSz_LoByte_;
         byte  DatSz_HiByte_x0f;
      };
      struct {
         byte  MsgCode_NullByte_;
         byte  MsgCode_HiByte_xf0;
         byte  MsgCode_LoByte_;
      };
   };

public:
   inline K_mf(TSize)  GetPkSize () const       { return static_cast<TSize>(DatSz_LoByte_ | (((word)DatSz_HiByte_x0f & 0x0f) << 8)); }
   inline K_mf(void)   SetPkSize (TSize size)   { DatSz_LoByte_    = static_cast<byte>(size);
                                                  DatSz_HiByte_x0f = static_cast<byte>((size >> 8) & 0x0f)
                                                                   | static_cast<byte>(DatSz_HiByte_x0f & 0xf0); }

   inline K_mf(TMsgCode) GetMsgCode () const          { return(word(word(MsgCode_HiByte_xf0 & 0xf0)<<4)
                                                                  | word(MsgCode_LoByte_));      }
   inline K_mf(void)     SetMsgCode (TMsgCode code)   { MsgCode_HiByte_xf0 = byte((code & 0xf00) >> 4)
                                                                           | byte(MsgCode_HiByte_xf0 & 0x0f);
                                                        MsgCode_LoByte_ = byte(code);        }
   //��Ʊq�o��}�l
   //byte  Dat_[];
};
//---------------------------------------------------------------------------
struct TPkRec : public THead
{
   byte  Dat_[0x1000-sizeof(THead)];  //�C����Ƴ̦h 4k bytes
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
static const TSize      HeadSize = sizeof(THead);
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TWorker;                 //�T���B�z��:��ڳB�z�T��������
//---------------------------------------------------------------------------
typedef TWorker**  TWorkerRegs;  //�T���B�z�̵��U��,�HMsgCode��@�}�C����,�d�߹�ڳB�z�T��������
//---------------------------------------------------------------------------
LIBNS_class TWorker
{
public:
   inline  K_ctor   TWorker  (TWorkerRegs regs, TMsgCode mc)   { regs[mc] = this; }
   inline  K_ctor   TWorker  ()                                {}
   virtual K_dtor  ~TWorker  ()                                {}

   enum TCheckResult
   {
      cpr_HeadOK,    //���T��OK,�N�T��������,�� OnPkRecv() �q��,���ݭn�A�I�s OnPkCheck()
      cpr_Continue,  //���T���ثe�L�k�P�_�O�_OK,������s��ƮɦA�I�s OnPkCheck() �ˬd�@��
      cpr_Error,     //���T�������D,�N�w���쪺��ƲM��
   };

   //�ˬd�T���O�_OK, OnPkCheck()�b�I�s OnPkRecv()���e,�ܤַ|�I�s�@��!
   //  �p�G�z���T���@�w���� pk.GetPkSize(), �h�i�H�����мg�� method
   //  ��method�D�n�O���z�i�H�ˬd pk.GetPkSize() �O�_�O�z�Q�n��,
   //  ��p��:�p�G�z���檺�T���j�p�� 10 bytes,�i�O pk.GetPkSize() �Ǧ^ 23,
   //         ��ܦ��T���L��, ���Ǧ^ cpr_Error
   virtual K_mf(TCheckResult)  OnPkCheck  (TSesPk&, const THead& pk, TSize recvSize) = 0;

   //��ڸ�ƺ򱵦b THead ����
   virtual K_mf(void)  OnPkRecv  (TSesPk&, const THead&) = 0;

   //���Ѥ@��²���ˬd�T���O�_���T������:
   template <class TPk>
   static K_mf(TCheckResult)  PkCheck  (const TPk& pk, TSize recvSize)
            { if(pk.GetPkSize() < pk.GetMinSize()) // [�T���j�p] �p�󦹰T�� [�̤p�������]
                 return cpr_Error;                 //    => cpr_Error
              if(recvSize < pk.GetMinSize())       // ���T���w���쪺���,�|�L�k�P�_ PkSize �O�_�X�z
                 return cpr_Continue;              //    => cpr_Continue
              // �T���j�p(PkSize) �P [���檺�j�p] �ۦP�h�� cpr_HeadOK, �_�h cpr_Error
              return(pk.GetPkSize()==pk.GetExpectSize() ? cpr_HeadOK : cpr_Error); }
   template <class TPk>
   static K_mf(TCheckResult)  PkCheckHiLoRef  (const TPk& pk, TSize recvSize, THiLoTransRef hiLoRef)
            { if(pk.GetPkSize() < pk.GetMinSize())
                 return cpr_Error;
              if(recvSize < pk.GetMinSize())
                 return cpr_Continue;
              return(pk.GetPkSize()==pk.GetExpectSize(hiLoRef) ? cpr_HeadOK : cpr_Error); }
};
//---------------------------------------------------------------------------
LIBNS_class TWorkersBase
{
public:
   virtual K_dtor ~TWorkersBase  ()    {}

   //���XWorker���U��
   virtual K_mf(TWorkerRegs)   GetWorkerRegs    () const = 0;
   //���XWorker���U���j�p
   virtual K_mf(TMsgCode)      GetWorkersCount  () const = 0;
   //���X�Y MsgCode �� Worker
   virtual K_mf(TWorker*)      GetMsgWorker     (TMsgCode mc) const  { return(mc >= GetWorkersCount() ? 0 : GetWorkerRegs()[mc]); }
   //�]�w�Y MsgCode �� Worker, �Ǧ^�]�w�e�� Worker
   K_mf(TWorker*)              SetMsgWorker     (TMsgCode, TWorker* newWorker);
   //��Y��Ses���`��,�I�s��mf�q��
   virtual K_mf(void) OnSesPkDestroy (TSesPk*) {
   }
};
//---------------------------------------------------------------------------
template <TMsgCode maxCount = 0x1000>
class TWorkers : public TWorkersBase
{
protected:
   TWorker* Workers_[maxCount];

public:
   K_ctor             TWorkers        ()                   { memset(Workers_, 0, sizeof(Workers_));    }
   K_mf(TWorkerRegs)  GetWorkerRegs   () const             { return const_cast<TWorkerRegs>(Workers_); }
   K_mf(TMsgCode)     GetWorkersCount () const             { return numofele(Workers_); }
   K_mf(TWorker*)     GetMsgWorker    (TMsgCode mc) const  { return mc >= numofele(Workers_) ? 0 : Workers_[mc]; }
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class  TSesPk : public TSesBase
                    , protected TWorker//�M���B�z�����U���T��
{
   typedef TSesBase  inherited;
   typedef TSesPk    this_type;
public:
   typedef TWorker::TCheckResult   TCheckResult;

   //----- �غc/�Ѻc
   K_ctor  TSesPk  (TWorkersBase&);
   K_dtor ~TSesPk  ();

   //----- �T���B�z/�ǰe
   virtual K_mf(bool)  SendPk       (const THead&);      //�z��ڭn�ǰe�����,�����򱵦b THead ����
   virtual K_mf(void)  OnHeartbeat  (const THeartbeat&); //����Heartbeat()��

   //----- ���A�B�z
   enum EState {
      sp_Creating,
      sp_Destroying,
      sp_Error,
      sp_WaitLinkReady,     //Session�s�u��
      sp_WaitConnected,     //�s�u�n�D�ǰe��,��Server�^��
      sp_WaitSignonInput,   //�s�uOK,���ԨϥΪ�ñ�J
      sp_WaitSignonResult,  //�n�J�n�D�ǰe��,��Server�^��
      sp_Ready,             //�w�n�J����
      sp_Signout,           //�t�εn�X
   };
          K_mf(void)    SetState  (EState newState, const std::string& reason);
   inline K_mf(EState)  GetState  () const   { return State_; }

protected:
   K_mf(void) OnBfDestroy ();
   //����S�����U���T����,�|�z�L OnPkCheck() �q���l�ͥX�Ӫ�class
   //���B�w�]�B�z THeartbeat ���T��, �Y�� THeartbeat, �h�|�I�s OnHeartbeat()
   virtual K_mf(TCheckResult)  OnPkCheck  (TSesPk&, const THead&, TSize recvSize);
   virtual K_mf(void)          OnPkRecv   (TSesPk&, const THead&);

   //�Ұʭp�ɾ�,��z�]�w���ɶ���F,�h�|�ϥ� OnTimer() �q���z
   K_mf(void)  StartTimer  (dword ms);
   //����p�ɾ�,���A���� OnTimer() ���ƥ�
   K_mf(void)  StopTimer   ();

   K_mf(void)  OnMesLinkReady   (TMesBase*);// SetState(sp_WaitConnected)
   K_mf(void)  OnMesLinkBroken  (TMesBase*, const std::string&);//�M�������w�İ�
   K_mf(void)  OnMesLinkFail    (TMesBase*, const std::string&);//SetState(sp_Error);

private:
   //��T���B�z������,�|�z�L OnPkWorkFinished() �ӧi���l�ͥX�Ӫ�class
   virtual K_mf(void)  OnPkWorkFinished  (const THead&);//do nothing
   //�Y�z���Ұʭp�ɾ�,�h�|�z�L OnTimer() �ӧi���l�ͥX�Ӫ�class
   virtual K_mf(void)  OnTimer           (TTimerBase*);//do nothing
   //�� Mes �������Ʈ�,�z�L OnMesRecvReady() �q��
   //  �b���B�|�����@�ӧ���T��,�éI�s Worker �B�z
   K_mf(void)  OnMesRecvReady    (TMesBase*, size_t size);//�����

   TWorkersBase*           Workers_;
   dword                   RecvPos_;   //�w���쪺��Ƥj�p
   TWorker::TCheckResult   PkResult_;
   dword                   SkipBytes_;
   TPkRec                  RecvPk_[2]; //RecvPk_[1]�@�O�@��!
   TTimer<this_type>       Timer_;
   EState                  State_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

