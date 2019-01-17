//
// ��r��Ū����, �i�w�ɱ��y, �Τ@��Ū��.
// class TTextReader;
//
// ����: $Id: TextReader.h,v 1.9 2005/08/26 01:42:08 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TextReaderH
#define TextReaderH
//---------------------------------------------------------------------------
#include "TimerThread.h"
#include "KFile.h"
#include "SyncEv.h"
#include <map>
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSyncFileReader : protected TFile
                            , protected TSyncObj
                            , private   TTimerBase
{
   typedef TFile inherited;
public:
   //constructor/destructor.
   K_ctor  TSyncFileReader  ();
   K_ctor  TSyncFileReader  ( const char* fileName, TFileMode
                            , mode_t unixAccessMode = defaultAccessMode
                            , mode_t dirAccessMode  = defaultDirAccessMode);
   K_ctor  TSyncFileReader  ( const char* fileName, TFileMode, const char* fileDesp
                            , mode_t unixAccessMode = defaultAccessMode
                            , mode_t dirAccessMode  = defaultDirAccessMode);
   K_ctor  TSyncFileReader  (const TSyncFileReader&);
   //Open/Close
   using inherited::Open;
   using inherited::Close;
   using inherited::GetFileName;
   using inherited::IsOpened;
   using inherited::GetLastErr;

   //Start Notify ReadLN
   enum EStartMode {
      sm_Timer,
      sm_ReadAll,    //�p�G�̫�S������,�粣�ͤ@�Өƥ�
      sm_TimerAfReadAll,
      sm_Mask            = 0x0f,
      sm_TruncLeadSpaces = 0x80,
      sm_TruncTailSpaces = 0x40,
      sm_KeepSpaces      = 0,
   };
   K_mf(void)  StartNotify  (EStartMode);

   /** Ū�J n ����, �Ǧ^ false, ��ܦb�B�z ReadLines() �ɶǦ^ olr_Stop, ReadAll()�Q���_.
   */
   K_mf(bool)  ReadAll  (EStartMode);

   /** �Ȱ��q��. �Y�n�~��,�i�I�s: StartNotify().
   */
   K_mf(void)  StopNotify  ();

   inline K_mf(EStartMode)  GetStartMode  () const  { return Mode_; }
protected:
   /** �q�ɮײ{�b��m�}�l��_Ū�J n bytes, Ū�J��I�s OnNextLine().
       �I�s OnNextLine() ��, �|�]�t���㪺 n bytes.
   */
   K_mf(void)  SetNextLine  (size_t n);

   /** �Ǧ^true,��ܥ��b���԰���Sync(), �Ǧ^ false��ܤ��ݭnSync().
   */
   virtual K_mf(bool)  IsNeedSync ();
           K_mf(void)  Sync       ();

   enum EOnLineResult {
      olr_Stop,      //���� OnTextLine(), OnNextLine() �ƥ�.
      olr_Continue,  //�~��Ū���U�@��.
      olr_Break,     //���_������Ū���j��(�קK�j�q��Ƴy����delay).
   };

   /** Ū�� n ����.
   */
   virtual K_mf(EOnLineResult)  ReadLines  (size_t n) = 0;

   inline K_mf(TFile&)        GetSelfFile ()       { return *this; }
   inline K_mf(const TFile&)  GetSelfFile () const { return *this; }
private:
   K_mf(void)  OnTimer  ();

   EStartMode     Mode_;
};
//---------------------------------------------------------------------------
template <class T>
class TSyncRecReader : public TSyncFileReader {
   typedef TSyncFileReader inherited;
   K_mf(EOnLineResult)  ReadLines  (size_t)
      {  T           recs[100];
         size_type   rdsz = Read(recs, sizeof(recs));
         if(IsResultOK(rdsz)) {
            if(size_type modn = rdsz % sizeof(T)) {
               Seek(0-modn, fsCurrent);
               rdsz -= modn;
            }
            T* precs = recs;
            while(rdsz) {
               OnSyncRec(*precs);
               ++precs;
               rdsz -= sizeof(T);
            }
         }
         return olr_Continue;
      }

protected:
   virtual K_mf(void)  OnSyncRec  (T&) = 0;

public:
   K_ctor  TSyncRecReader  ()  {}
   K_ctor  TSyncRecReader  ( const char* fileName, TFileMode fileMode
                           , mode_t unixAccessMode = defaultAccessMode
                           , mode_t dirAccessMode  = defaultDirAccessMode)
      : inherited(fileName, fileMode, unixAccessMode, dirAccessMode)
      { }
   K_ctor  TSyncRecReader  ( const char* fileName, TFileMode fileMode, const char* fileDesp
                           , mode_t unixAccessMode = defaultAccessMode
                           , mode_t dirAccessMode  = defaultDirAccessMode)
      : inherited(fileName, fileMode, fileDesp, unixAccessMode, dirAccessMode)
      { }
   K_ctor  TSyncRecReader  (const TSyncRecReader& a)
      : inherited(a)
      { }
   K_dtor ~TSyncRecReader  ()
      { StopNotify();
      }
};
//---------------------------------------------------------------------------
LIBNS_class TTextReader : public TSyncFileReader
{
   typedef TSyncFileReader inherited;
public:
   //constructor/destructor.
   K_ctor  TTextReader  ();
   K_ctor  TTextReader  ( const char* fileName, TFileMode fileMode
                        , mode_t unixAccessMode = defaultAccessMode
                        , mode_t dirAccessMode  = defaultDirAccessMode);
   K_ctor  TTextReader  (const char* fileName, TFileMode, const char* fileDesp,
                         mode_t unixAccessMode = defaultAccessMode,
                         mode_t dirAccessMode  = defaultDirAccessMode);
   K_ctor  TTextReader  (const TTextReader&);
   K_dtor ~TTextReader  ();

   // public inherited member functions.
   using inherited::Open;
   using inherited::Close;
   using inherited::ReadAll;

   /** Start Notify ReadLN
   EStartMode: sm_ReadAll  //�p�G�̫�S������,�粣�ͤ@�Өƥ�
   */
   K_mf(void)  StartNotifyReadLN  (EStartMode);

protected:
   using inherited::StartNotify;

   /** �q�ɮײ{�b��m�}�l��_Ū�J n bytes, Ū�J��I�s OnNextLine().
       �I�s OnNextLine() ��, �|�]�t���㪺 n bytes.
   */
   K_mf(void)  SetNextLine  (size_t n);

   /** Ū�J n ����.
   */
   K_mf(EOnLineResult)  ReadLines  (size_t n);

private:
   std::string    Buffer_;
   size_t         NextSize_;

   /** �Ǧ^ false, ��ܼȰ��q��, ����A���I�s StartNotifyReadLN()
   */
   virtual K_mf(EOnLineResult)  OnTextLine  (const std::string&) = 0;
   virtual K_mf(EOnLineResult)  OnNextLine  (const std::string&) { return olr_Continue; }
           K_mf(EOnLineResult)  DoBuffer    ();
};
//---------------------------------------------------------------------------
LIBNS_class TTokenFile : public TTextReader
{
   typedef TTextReader  inherited;

   //LIBNSTMP template LIBNS_class std::map<std::string,std::string>;
   typedef std::map<std::string,std::string> TSymbs;
   TSymbs   Symbs_;
   bool     IsCheckParam_;

   class TTokenInclude;
   friend class TTokenInclude;

   virtual K_mf(void)  OnGetParam    (char* opname, char* param) = 0;
   virtual K_mf(void)  OnGetLabel    (const char* labelName) {}
   virtual K_mf(void)  OnUnknowLine  (const std::string&)    {}
protected:
   virtual K_mf(EOnLineResult)  OnTextLine     (const std::string&);
   inline  K_mf(void)           SetCheckParam  (bool v)  { IsCheckParam_ = v; }
public:
   //constructor/destructor.
   inline K_ctor  TTokenFile  () : IsCheckParam_(true) {}
   inline K_ctor  TTokenFile  (const TTokenFile& a)  : inherited(a), IsCheckParam_(true)  {}
          K_ctor  TTokenFile  (const char* fileName, TFileMode fileMode = fmRead,
                               mode_t unixAccessMode = defaultAccessMode,
                               mode_t dirAccessMode  = defaultDirAccessMode);
          K_ctor  TTokenFile  (const char* fileName, TFileMode fileMode, const char* fileDesp,
                               mode_t unixAccessMode = defaultAccessMode,
                               mode_t dirAccessMode  = defaultDirAccessMode);
   inline K_mf(void)  StartGetParam  ()   { StartNotifyReadLN(EStartMode(sm_ReadAll
                                                               | sm_TruncLeadSpaces
                                                               | sm_TruncTailSpaces)); }

   static K_mf(std::string)  GetDefaultPath  ();
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
#endif

