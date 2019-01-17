/////////////////////////////////////////////////////////////////////////////
#if !defined(_TserIniFile_h_)
#define _TserIniFile_h_
//---------------------------------------------------------------------------
#include "KIniFile.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// class TTserIni
/////////////////////////////////////////////////////////////////////////////
class TTserIni : public TKIniFile
{
   typedef TKIniFile  inherited;
   
public:
   K_ctor TTserIni ();

   K_mf(void) RemoveSessionInfo (const std::string&);

   inline K_mf(std::string) GetAppiaIniPath ()
      { return inherited::ReadString(std::string("Summary"), std::string("AppiaIniPath"), std::string()); }

   inline K_mf(std::string) GetAppiaMWName (const std::string& AppGrp)
      { return inherited::ReadString(AppGrp, std::string("AppiaMWName"), std::string()); }

   inline K_mf(std::string) GetAppiaSvrAddr (const std::string& AppGrp)
      { return inherited::ReadString(AppGrp, std::string("AppiaSvrAddr"), std::string()); }

   inline K_mf(std::string) GetFixPassword (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("Password"), std::string()); }

   inline K_mf(std::string) GetAPCode (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("APCode"), std::string()); }

//   inline K_mf(std::string) GetPVCID (const std::string& sesID)
//      { return inherited::ReadString(sesID, std::string("PVCID"), std::string()); }

   inline K_mf(std::string) GetApplyDate (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("ApplyDate"), std::string()); }

   inline K_mf(std::string) GetApplySeqNo (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("SeqNo"), std::string()); }

   inline K_mf(std::string) GetTestNo (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("TestNo"), std::string()); }

   inline K_mf(std::string) GetSourceIP1 (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("SourceIP1"), std::string()); }

   inline K_mf(std::string) GetSourceIP2 (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("SourceIP2"), std::string()); }

   inline K_mf(std::string) GetSourcePort (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("remote_port"), std::string()); }

   inline K_mf(std::string) GetFwdGrp (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("FWGroup"), std::string()); }

   inline K_mf(std::string) GetAppGrp (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("AppiaGroup"), std::string()); }

   inline K_mf(std::string) GetDestIP (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("IP"), std::string()); }

   inline K_mf(std::string) GetListenPort1 (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("ListPort1"), std::string()); }

   inline K_mf(std::string) GetListenPort2 (const std::string& sesID)
      { return inherited::ReadString(sesID, std::string("ListPort2"), std::string()); }

   inline K_mf(void) SetIP1OK (const std::string& sesID)
      { inherited::WriteBool(sesID, std::string("IP1OK"), true); }

   inline K_mf(void) SetIP2OK (const std::string& sesID)
      { inherited::WriteBool(sesID, std::string("IP2OK"), true); }

   inline K_mf(bool) IsTestOK (const std::string& sesID)
      {  bool ip1ok, ip2ok;

      	if(GetSourceIP1(sesID) == std::string())
      	   ip1ok = true;
      	else
       	   ip1ok = inherited::ReadBool(sesID, std::string("IP1OK"), false);

      	if(GetSourceIP2(sesID) == std::string())
      	   ip2ok = true;
      	else
       	   ip2ok = inherited::ReadBool(sesID, std::string("IP2OK"), false);
      	return (ip1ok && ip2ok); }
};

/////////////////////////////////////////////////////////////////////////////
// class TAppiaIni
/////////////////////////////////////////////////////////////////////////////
class TAppiaIni : public TKIniFile
{
   typedef TKIniFile  inherited;
   
public:
   K_ctor  TAppiaIni (const std::string&);
   K_dtor ~TAppiaIni ();
      
   K_mf(void) RemoveSessionInfo (const std::string& sesID, const std::string& mwName);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TserIniFile_h_)
