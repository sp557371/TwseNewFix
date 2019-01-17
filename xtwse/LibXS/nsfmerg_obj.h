/**
   ��@[�ɮצX��]����.
   �iŪ���ƭӨӷ���,�g�J�@�ӥت���.
*/
//---------------------------------------------------------------------------
#ifndef nsfmerg_objH
#define nsfmerg_objH
//---------------------------------------------------------------------------
#include "TextReader.h"
#include <map>
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMergObj
{
public:
   /** �غc
   */
   K_ctor  TMergObj  (TFile& mergLogFile, const char* destinFileName, size_t recSize);
   K_ctor  TMergObj  (TFile& mergLogFile, const char* destinFileName, const std::string& txtFormat);

   /** �C����ƪ��j�p, �Ǧ^ 0 �h����r��(�H����r����������).
   */
   inline K_mf(size_t)  GetRecSize  () const  { return RecSize_; }

   /** �Ǧ^��r�ɪ��榡.
   */
   inline K_mf(const std::string&)  GetTxtFormat  () const  { return TxtFormat_; }

   /** �ӷ��ɳB�z����
   */
   LIBNS_class TMergSource : protected TTextReader {
      typedef TTextReader  inherited;
      TMergObj*   MergObj_;
      size_t      LogFileIdx_;
      std::string TxtFormat_;
      std::string TxtBuffer_;
      K_mf(EOnLineResult)  OnTextLine  (const std::string&);
      K_mf(EOnLineResult)  OnNextLine  (const std::string&);
      K_mf(void)           SetNext     ();
   public:
      K_ctor  TMergSource  (const TMergSource&);
      K_ctor  TMergSource  (TMergObj*,
                            const char* fileName,
                            size_type   sourcePosFrom,
                            size_t      logFileIdx,
                            void*       userDefined = 0);

      using inherited::GetCurPos;
      using inherited::StartNotifyReadLN;
      using inherited::GetFullName;
      inline K_mf(size_t)  GetLogFileIdx  () const   { return LogFileIdx_; }

      void* UserDefined_;
   }; // class TMergSource

   /** ��ӷ��ɦ���[�@���r]��Ʈ�,�Ѧ��禡�B�z.���禡�|�i���r�榡���R:
       �Ǧ^ 0, ��ܦ���w�Q�B�z.
       �Ǧ^ >0, ����ٻݭn�A�� n(�Ǧ^��)bytes.
   */
   K_mf(size_t)  OnSourceText  (TMergSource*, const std::string&);

   /** ��ӷ��ɦ���@����Ʈ�,�Ѧ��禡�B�z.
       �g�J�X�֥ت��a�ɤ��e,�z�i�H���ݬݤ��e,
       �p�G�z�M�w���g�J�X����,�ЩI�s:WriteSourceIndex()
   */
   virtual K_mf(void)  OnSourceData  (TMergSource*, const std::string&);

   /** �W�[�@�Өӷ���.
       userDefined�i�H�ۭq�@�ӫ��Ы��O�����.
   */
   K_mf(bool)  AddSource  (const std::string& sourceFileName,
                           TFile::size_type   sourcePosFrom,
                           size_t             logFileIdx,
                           void*              userDefined = 0);
   /** ���o�X�֥ت��a�ɮ�.
   */
   K_mf(TFile&)  GetDestinFile  ()  { return Destin_; }

   /** ���o�ӷ��ɪ��ƶq.
   */
   K_mf(size_t)  GetSourceCount  () const   { return Sources_.size(); }

protected:
   K_mf(void) WriteSourceIndex (TMergSource*);

private:

   typedef std::map<std::string,TMergSource> TSources;

   TFile*      MergLogFile_;
   TFile       Destin_;
   size_t      RecSize_;
   std::string TxtFormat_;
   int         TkrIdx_;
   TSources    Sources_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

