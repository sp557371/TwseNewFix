/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvtDispRes_h_)
#define _Nsa_EvtDispRes_h_
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// enum EEvtDispID
/////////////////////////////////////////////////////////////////////////////
enum EEvtDispID
{
   edi_None,
      //-------------------------------------------
      // 系統內定
      edi_FormShow     =  1,
      edi_SysPreClose  =  2,
      
      edi_EvMapSync    = 11,
      
      edi_DailyOpen    = 21,
      edi_DailyClose   = 22,
      edi_DailyBfClear = 26,
      edi_DailyClear   = 27,
      edi_DailyAfClear = 28,

      //-------------------------------------------
      // KTS
      edi_Kts_Start       = 10000,
      //
      edi_Kts_Appia_Start = 10000,
      edi_Kts_Appia_OperatorCommand,
      edi_Kts_Appia_GetSessionInfo,
      edi_Kts_Appia_GetMiddlewareInfo,
      edi_Kts_Appia_StartMiddlwareEvents,
      edi_Kts_Appia_GetSessionList,
      edi_Kts_Appia_GetMiddlewareConfig,
      edi_Kts_Appia_RestoreMessageRequest,
      edi_Kts_Appia_OutboundMessage,

      //-------------------------------------------
      // Mds
      edi_Mds_Start     = 20000,
      //
      edi_Mds_Dbg_Start = edi_Kts_Start,
      edi_Mds_Dbg_GetTradeList,
};

/////////////////////////////////////////////////////////////////////////////
// enum EEvtDispResu
/////////////////////////////////////////////////////////////////////////////
enum EEvtDispResu
{
   edr_None,
      //-------------------------------------------
      // 系統內定
      edr_Continue = 1,
      edr_Finish   = 2,
      edr_Success  = 3,
      edr_Error    = 4,
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvtDispRes_h_)
