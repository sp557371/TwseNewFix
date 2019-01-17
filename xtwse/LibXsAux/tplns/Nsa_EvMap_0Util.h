/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_Util_h_)
#define _Nsa_EvMap_Util_h_
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMapUtil
/////////////////////////////////////////////////////////////////////////////
class TEvMapUtil
{
public:
   template <class TMap>
   static K_mf(void) Sync (TMap* SMap, TMap* DMap, bool IsAll);

   template <class TMap, class TDat>
   static K_mf(void) Sync_OrgDat (TMap* SMap, TMap* DMap);

   template <class TMap1, class TMap2>
   static K_mf(void) Sync_Key (TMap1* SMap, TMap2* DMap);
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMapUtil impl
/////////////////////////////////////////////////////////////////////////////
template <class TMap>
K_mf(void) TEvMapUtil::Sync (TMap* SMap, TMap* DMap, bool IsAll)
{
   typedef typename TMap::iterator  TMapI;
   typedef typename TMap::updater   TMapU;

   TMapI itaS, itaD, itaDTmp;

   // SrcMap �������, DstMap �]�ݭn��
   for (itaS=SMap->begin(); itaS!=SMap->end(); ++itaS) {
      itaD = DMap->fetch(itaS->first);
      if (IsAll) {
         if ((itaS->second == itaD->second) == false) {
            TMapU updD = DMap->begin_update(itaD);
            *updD = itaS->second;
         }
      }
   }

   // SrcMap �S�������, DstMap �ݭn�M����
   itaD = DMap->begin();
   while (itaD != DMap->end()) {
      itaDTmp = itaD;
      ++itaD;

      itaS = SMap->find(itaDTmp->first);
      if (itaS == SMap->end()) {
         DMap->erase(itaDTmp);
      }
   }
}
//---------------------------------------------------------------------------

template <class TMap, class TDat>
K_mf(void) TEvMapUtil::Sync_OrgDat (TMap* SMap, TMap* DMap)
{
   typedef typename TMap::iterator  TMapI;
   typedef typename TMap::updater   TMapU;

   TMapI itaS, itaD, itaDTmp;

   // SrcMap �������, DstMap �]�ݭn��
   for (itaS=SMap->begin(); itaS!=SMap->end(); ++itaS) {
      itaD = DMap->fetch(itaS->first);

      TDat& OrgDatS = itaS->second.GetDat();
      TDat& OrgDatD = itaD->second.GetDat();

      if ((OrgDatS == OrgDatD) == false) {
         TMapU updD = DMap->begin_update(itaD);
         OrgDatD = OrgDatS;
      }
   }

   // SrcMap �S�������, DstMap �ݭn�M����
   itaD = DMap->begin();
   while (itaD != DMap->end()) {
      itaDTmp = itaD;
      ++itaD;

      itaS = SMap->find(itaDTmp->first);
      if (itaS == SMap->end()) {
         DMap->erase(itaDTmp);
      }
   }
}
//---------------------------------------------------------------------------

template <class TMap1, class TMap2>
K_mf(void) TEvMapUtil::Sync_Key (TMap1* SMap, TMap2* DMap)
{
   typedef typename TMap1::iterator  TMap1I;
   typedef typename TMap2::iterator  TMap2I;

   TMap1I itaS;
   TMap2I itaD, itaDTmp;

   // SrcMap �������, DstMap �]�ݭn��
   for (itaS=SMap->begin(); itaS!=SMap->end(); ++itaS) {
      itaD = DMap->fetch(itaS->first);
   }

   // SrcMap �S�������, DstMap �ݭn�M����
   itaD = DMap->begin();
   while (itaD != DMap->end()) {
      itaDTmp = itaD;
      ++itaD;

      itaS = SMap->find(itaDTmp->first);
      if (itaS == SMap->end()) {
         DMap->erase(itaDTmp);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_Util_h_)
