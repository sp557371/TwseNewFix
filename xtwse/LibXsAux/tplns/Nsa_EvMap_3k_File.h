/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_3k_File_h_)
#define _Nsa_EvMap_3k_File_h_
//---------------------------------------------------------------------------
#include "ev_container.h"
#include "Nsa_EvMap_2k_File.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMap3kFile
/////////////////////////////////////////////////////////////////////////////
template <class Map1T, class FileDat3T = typename Map1T::mapped_type::TMap::mapped_type::TMap::mapped_type,
                       class FileDat2T = typename Map1T::mapped_type::TMap::mapped_type::TDat,
                       class FileDat1T = typename Map1T::mapped_type::TDat>

class TEvMap3kFile : public container_ev_handler
{
public:
   //==============================================
   // Container
   typedef          Map1T               TMap1;
   typedef          FileDat1T           TFileDat1;
   typedef          FileDat2T           TFileDat2;
   typedef          FileDat3T           TFileDat3;
   typedef typename TMap1::key_type     TKey1;
   typedef typename TMap1::iterator     TMapI1;
   typedef typename TMap1::mapped_type  TMappedType1;
   typedef typename TMappedType1::TMap  TMap2;

   //==============================================
   // Recorder type
   typedef          TEvMapReco  <TMap1, TFileDat1>             TRecorder1;
   typedef          TEvMap2kFile<TMap2, TFileDat3, TFileDat2>  TDetailFile;
   typedef typename TDetailFile::TRecorder1                    TRecorder2;
   typedef typename TDetailFile::TRecorder2                    TRecorder3;

   struct SRecoSet
   {
      TRecorder2*  Reco2_;
      TRecorder3*  Reco3_;

      inline K_ctor SRecoSet ()
         : Reco2_(NULL)
         , Reco3_(NULL)
      {}

      K_dtor ~SRecoSet ()
      {
         if (Reco3_)  delete Reco3_;
         if (Reco2_)  delete Reco2_;
      }
   };

   typedef          Kstl::ev_map<TKey1, SRecoSet>  TRecoSet;
   typedef typename TRecoSet::iterator             TRecoSetI;

protected:
   TMap1*       Map1_;
   TRecorder1*  Recorder1_;
   TRecoSet     RecoSet_;
   std::string  DirPath_;
   bool         IsUseReco2_;

public:
   //==============================================
   // constructor/destructor

   // 當 dat 是 000, 101 時, isUseReco2 可設為 false
   K_ctor  TEvMap3kFile (TMap1*, const char* dirPath, const char* idxFile, bool isUseReco2 = true);

   K_dtor ~TEvMap3kFile ();

protected:
   //==============================================
   // overwrite container_ev_handler virtual function
   K_mf(void) OnContainerBfClear  (const Kstl::ev_container&);
   K_mf(void) OnContainerAfInsert (const Kstl::ev_container&, const Kstl::ev_iterator&, size_type);
   K_mf(void) OnContainerBfErase  (const Kstl::ev_container&, const Kstl::ev_iterator&, size_type);

   //==============================================
   // TEvMap3kFile function
   K_mf(std::string) GetRecorder2Path (const TKey1&);
   K_mf(std::string) GetRecorder3Path (const TKey1&);
   K_mf(void)        EraseDetailReco  (const TKey1&);
   K_mf(void)        EraseDetailReco  (TRecoSetI&);
};
//////////////////////////////////////////////////////////////////////////////////////
// class TEvMap3kFile impl
//////////////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMap3kFile_Impl(return_type)                                          \
   template <class Map1T, class FileDat3T, class FileDat2T, class FileDat1T>        \
   K_mf(return_type) TEvMap3kFile<Map1T, FileDat3T, FileDat2T, FileDat1T>::         \
//------------------------------------------------------------------------------------

#if defined(__BORLANDC__)
   #define Nsa_TEvMap3kFile_Impl_S(return_type)                                     \
      template <class Map1T, class FileDat3T, class FileDat2T, class FileDat1T>     \
      TEvMap3kFile<Map1T, FileDat3T, FileDat2T, FileDat1T>::return_type __fastcall  \
      TEvMap3kFile<Map1T, FileDat3T, FileDat2T, FileDat1T>::
#else //------------------------------------------------------------------------------
   #define Nsa_TEvMap3kFile_Impl_S(return_type)                                     \
      template <class Map1T, class FileDat3T, class FileDat2T, class FileDat1T>     \
      typename TEvMap3kFile<Map1T, FileDat3T, FileDat2T, FileDat1T>::return_type    \
      TEvMap3kFile<Map1T, FileDat3T, FileDat2T, FileDat1T>::
#endif
//------------------------------------------------------------------------------------

template <class Map1T, class FileDat3T, class FileDat2T, class FileDat1T>
K_ctor TEvMap3kFile<Map1T, FileDat3T, FileDat2T, FileDat1T>::TEvMap3kFile (TMap1*      map1,
                                                                           const char* dirPath,
                                                                           const char* idxFile,
                                                                           bool        isUseReco2)
   : Map1_      (map1)
   , DirPath_   (dirPath)
   , IsUseReco2_(isUseReco2)
{
   if (DirPath_.empty() == false) {
      char ch = *(DirPath_.rbegin());

      if (ch == '/' || ch == '\\') {
         DirPath_.erase(DirPath_.length()-1);
      }
   }

   Map1_->add_ev_handler(this);

   //==============================================
   // idx file
   char buf[256];

   sprintf(buf, "%s/%s", DirPath_.c_str(), idxFile);
   Recorder1_ = new TRecorder1(Map1_, buf);
}
//---------------------------------------------------------------------------

template <class Map1T, class FileDat3T, class FileDat2T, class FileDat1T>
K_dtor TEvMap3kFile<Map1T, FileDat3T, FileDat2T, FileDat1T>::~TEvMap3kFile ()
{
   Map1_->del_ev_handler(this);

   if (Recorder1_)  delete Recorder1_;
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kFile_Impl(std::string) GetRecorder2Path (const TKey1& key1)
{
   char buf[256];

   sprintf(buf, "%s/%s", DirPath_.c_str(), key1.as_string().c_str());
   return std::string(buf);
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kFile_Impl(std::string) GetRecorder3Path (const TKey1& key1)
{
   char buf[256];

   sprintf(buf, "%s/%s_Det", DirPath_.c_str(), key1.as_string().c_str());
   return std::string(buf);
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kFile_Impl(void) OnContainerBfClear (const Kstl::ev_container&)
{
   TRecoSetI itaTemp;
   TRecoSetI ita = RecoSet_.begin();

   while (ita != RecoSet_.end()) {
      itaTemp = ita;
      ++ita;
      EraseDetailReco(itaTemp);
   }
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kFile_Impl(void) OnContainerAfInsert (const Kstl::ev_container& c, const Kstl::ev_iterator& eviter, size_type)
{
   TMapI1    iter     = static_cast<const TMapI1&>(eviter);
   TRecoSetI iterReco = RecoSet_.fetch(iter->first);
   TMap2*    map2     = iter->second.GetContainer(map2);

   if (IsUseReco2_) {
      iterReco->second.Reco2_ = new TRecorder2(map2, GetRecorder2Path(iter->first));
   }
   iterReco->second.Reco3_ = new TRecorder3(map2, GetRecorder3Path(iter->first));
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kFile_Impl(void) OnContainerBfErase (const Kstl::ev_container& c, const Kstl::ev_iterator& eviter, size_type)
{
   TMapI1 iter = static_cast<const TMapI1&>(eviter);

   EraseDetailReco(iter->first);
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kFile_Impl(void) EraseDetailReco (const TKey1& key1)
{
   TRecoSetI itaReco = RecoSet_.find(key1);
   if (itaReco != RecoSet_.end()) {
      EraseDetailReco(itaReco);
   }
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kFile_Impl(void) EraseDetailReco (TRecoSetI& iter)
{
   std::string strPath2 = GetRecorder2Path(iter->first);
   std::string strPath3 = GetRecorder3Path(iter->first);

   RecoSet_.erase(iter);

   unlink(strPath2.c_str());
   unlink(strPath3.c_str());
}

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_3k_File_h_)

