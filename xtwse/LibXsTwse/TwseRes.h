/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseRes_h_)
#define _TwseRes_h_
//---------------------------------------------------------------------------
#include "QFixExt/QFixTagDef.h"
#include "TwStkOrder.h"
#include "FixStkRes.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Tw::Stk;

/////////////////////////////////////////////////////////////////////////////
// enum
/////////////////////////////////////////////////////////////////////////////
enum ETwseFixRejectCode
{
   tfrc_None = FIX::Stk::wrc_None,

   // �Ъ`�N, tfrc_Start ���� >= FIX::Stk::EWorkRejCode::wrc_UserBegin
   tfrc_Start = FIX::Stk::wrc_UserBegin,
      // �s�u�l�t��
      tfrc_Conn_Start,
         tfrc_AppendNoError,        // 01.�ˬd�ç�APPEND-NO
         tfrc_KeyValueError,        // 04.�ˬd�ç�KEY-VALUE          2013/12/17 miguel change to 02.
         tfrc_AppendNoEqual0,       // 70.�ˬd�ç�APPEND NO          2013/12/17 miguel change to 03.
         tfrc_RawDataLenNotEqual5,  // 71.�ˬd�ç�RAWDATA LENGTH     2013/12/17 miguel change to 08.
         tfrc_ApCodeErr,            // 72.�ˬd�ç�AP-CODE  (FIX���TargetSubID)
         tfrc_SenderCompIDError,    // 73.���~��SenderCompID (FIX���SenderCompID)
         tfrc_MsgTypeError,         // 78.���䴩���T�����O   (FIX���MsgType)
         tfrc_TradeSuspended,       // 86.��PVC����@�~�A�i�J���u���A
      tfrc_Conn_End,

      // ����l�t��
      tfrc_Trade_Start,
         tfrc_BrokerIDErr,          // 12.�ˬd�ç��Ҩ�ӥN��          (FIX���SenderSubID)
         /* tfrc_SeqNoError,           // 18.�ˬd�ç󥿩e�U�ѽs��          (FIX���OrderID��|�X) */
         tfrc_QuantityError,        // 22.�ˬd�ç󥿩e�U�ƶq            (FIX���OrderQty)
         tfrc_QuantityWasCut,       // 31.�~��R�i�έɨ��e�U�ƶq�Q�R��
         // for fix
                                    // 70.�ˬdFIX ClOrdID�O�_����
         tfrc_IvacNoLenErr,         // 71.�ˬd�ç󥿧��H�b��          (FIX���Account)
         tfrc_SeqNoLenErr,          // 72.�ˬd�ç󥿩e�U�ѽs��          (FIX���OrderID)
         tfrc_StkNoLenErr,          // 73.�ˬd�ç󥿪Ѳ��N��            (FIX���Symbol)
         tfrc_PriLenErr,            // 74.�ˬd�ç󥿳��
         tfrc_QtyLenErr,            // 75.�ˬd�ç󥿩e�U�ƶq            (FIX���OrderQty)
         tfrc_ErrorOverLimit,       // 77.���~���ƶW�L����,�гq���ҥ��
         tfrc_WarrantLPErr,         // 78.�ˬd�ç�WARRANT-LP          (FIX���TwseWarrantLP)
         tfrc_IvacNoFlagErr,        // 79.�ˬd�ç�IVACNO-FLAG         (FIX���TwseIvacnoFlag)
         tfrc_TwseOrdTypeErr,       // 80.�ˬd�ç�ORDER-TYPE          (FIX���TwseOrdType)
         tfrc_OrigClOrdIDErr,       // 81.�ˬd�ç�OrigClOrdID         (FIX���OrigClOrdID)
         tfrc_ClOrdIDErr,           // 82.�ˬd�ç�ClOrdID             (FIX���ClOrdID)
         tfrc_OrdTypeErr,           // 83.�ˬd�ç�OrdType             (FIX���OrdType)
                                    // 84.�R��d�P��e�U���e���� (FIX.Stk �̭��w���w�q wrc_OrdContNotMatch)
         tfrc_TwseOrdIdDup,         // 85.�e�U�ѽs������(��ӥN��+�e�U�Ѹ�)
   
         tfrc_FrontEndSystemError,  // 98.�e�ݨt�β��`�A�Э��s�e�U
         tfrc_CallCenter,           // 99.�d�ߩe�U�O�_�w��J���\�Υ��q�ܨ��ҥ�Ҹ߰�

         //tfrc_ApCodeErr,      // 76.�ˬd�ç�AP-CODE             (FIX���TargetSubID)
         //tfrc_MsgTypeError,   // 78.���䴩���T�����O              (FIX���MsgType)
      tfrc_Trade_End,

   tfrc_Total
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseRes
/////////////////////////////////////////////////////////////////////////////
class TTwseRes
{
   typedef FIX::TFixTargetSubID  TFixTargetSubID;

   static K_mf(std::string) GetErrCodeHdr (int rejCode, const TMarket&, const TTrxSegment&);

public:
   // ���~�N�X - �ϥ� ETwseFixRejectCode ���o StatusCode
   // ==> �p�G�u�O�n�� [�s�u�l�t��], �u�n�ǤJ (rejCode, TMarket) �Y�i
   static K_mf(std::string) GetStsCodeByRejCode (int rejCode, const TMarket&, const TTrxSegment& seg = TTrxSegment(ts_Early));

   // ���~�N�X - �ϥ� ETwseFixRejectCode ���o StatusCode
   static K_mf(std::string) GetStsCodeByRejCode (int rejCode, const TMarket&, const TFixTargetSubID&);

   // ���~�N�X - �ϥ� ETwseFixRejectCode ���o StatusCode
   static K_mf(std::string) GetStsCodeByTwseCode (int twseCode, const TMarket&, const TTrxSegment&);

   // ���~�N�X - �ϥ� ETwseFixRejectCode ���o StatusCode
   static K_mf(std::string) GetStsCodeByTwseCode (int twseCode, const TMarket&, const TFixTargetSubID&);

   //==============================================
   // �l�t�ΥN��
   static const char*  TseConn;
   static const char*  TseFull;
   static const char*  TseFixed;
   static const char*  TseOdd;
   //--------------------
   static const char*  OtcConn;
   static const char*  OtcFull;
   static const char*  OtcFixed;
   static const char*  OtcOdd;
   
   //==============================================
   // ���~�X
   //------------------------------------
   // �s�u�l�t��
   static const char*  AppendNoError;        // 01.�ˬd�ç�APPEND-NO
   static const char*  KeyValueError;        // 04.�ˬd�ç�KEY-VALUE          2013/12/17 miguel change to 02.
   static const char*  AppendNoEqual0;       // 70.�ˬd�ç�APPEND NO          2013/12/17 miguel change to 03.
   static const char*  RawDataLenNotEqual5;  // 71.�ˬd�ç�RAWDATA LENGTH     2013/12/17 miguel change to 08.
   static const char*  ApCodeErr;            // 72.�ˬd�ç�AP-CODE  (FIX���TargetSubID)
   static const char*  SenderCompIDError;    // 73.���~��SenderCompID (FIX���SenderCompID)
   static const char*  MsgTypeError;         // 78.���䴩���T�����O   (FIX���MsgType)
   static const char*  TradeSuspended;       // 86.��PVC����@�~�A�i�J���u���A
   //------------------------------------
   // ����l�t��
   //--------------------------
   
   /*
   // 18.�ˬd�ç󥿩e�U�ѽs��(FIX���OrderID��|�X)(���q���,�L��w��)
   // 17.�ˬd�ç󥿩e�U�ѽs��(FIX���OrderID��|�X)(�s��)
   static const char*  SeqNoError;
   static const char*  SeqNoErrorOdd;
   */

   //--------------------------
   // 22.�ˬd�ç󥿩e�U�ƶq(FIX���OrderQty)
   static const char*  QuantityError;        
   //--------------------------
   // �~��R�i�έɨ��e�U�ƶq�Q�R��
   static const char*  QuantityWasCut;       // 31:���q,�L��
   static const char*  QuantityWasCutOdd;    // 32:�s��

   static const char*  ClOrdIDRepeat;        // 70.�ˬdFIX ClOrdID�O�_����
   static const char*  IvacNoLenErr;         // 71.�ˬd�ç󥿧��H�b��
   static const char*  SeqNoLenErr;          // 72.�ˬd�ç󥿩e�U�ѽs��
   static const char*  StkNoLenErr;          // 73.�ˬd�ç󥿪Ѳ��N��
   static const char*  PriLenErr;            // 74.�ˬd�ç󥿳��
   static const char*  QtyLenErr;            // 75.�ˬd�ç󥿩e�U�ƶq
   static const char*  BrokerIDErr;          // 76.�ˬd�ç�BrokerID            (FIX���SenderSubID)
   static const char*  ErrorOverLimit;       // 77.���~���ƶW�L����,�гq���ҥ��
   static const char*  WarrantLPErr;         // 78.�ˬd�ç�WARRANT-LP          (FIX���TwseWarrantLP)
   static const char*  IvacNoFlagErr;        // 79.�ˬd�ç�IVACNO-FLAG         (FIX���TwseIvacnoFlag)
   static const char*  TwseOrdTypeErr;       // 80.�ˬd�ç�ORDER-TYPE          (FIX���TwseOrdType)
   static const char*  OrigClOrdIDErr;       // 81.�ˬd�ç�OrigClOrdID         (FIX���OrigClOrdID)
   static const char*  ClOrdIDErr;           // 82.�ˬd�ç�ClOrdID             (FIX���ClOrdID)
   static const char*  OrdTypeErr;           // 83.�ˬd�ç�OrdType             (FIX���OrdType)
   static const char*  OrdContNotMatch;      // 84.�R��d�P��e�U���e����
   static const char*  TwseOrdIdDup;         // 85.�e�U�ѽs������(��ӥN��+�e�U�Ѹ�)
   static const char*  FrontEndSystemError;  // 98.�e�ݨt�β��`�A�Э��s�e�U
   static const char*  CallCenter;           // 99.�d�ߩe�U�O�_�w��J���\�Υ��q�ܨ��ҥ�Ҹ߰�
   //----------------------------------------------
   static const int    IntQuantityWasCut;    // 31.�~��R�i�έɨ��e�U�ƶq�Q�R��
   static const int    IntQuantityWasCutOdd; // 32.�~��R�i�e�U�ƶq�Q�R��

   //static const char*  ApCodeErr;      // 76.�ˬd�ç�AP-CODE
   //static const char*  MsgTypeError;   // 78.���䴩���T�����O              (FIX���MsgType)
};

/////////////////////////////////////////////////////////////////////////////
} // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseRes_h_)
