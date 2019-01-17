//
// �q�Tclasses��¦: Session-Messager
//    class TMesFile;
//
// ����: $Id: MesFile.h,v 1.2 2005/04/28 05:51:46 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef MesFileH
#define MesFileH
//---------------------------------------------------------------------------
#include "Mes.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesFile : public TMesBase
{
   typedef TMesBase  inherited;
public:
   K_ctor  TMesFile ();
   K_dtor ~TMesFile  ();

   //setting: filename<;[c][rwa][opentime][-closetime][,speed]>
   //                  ^�O�|�������u;�v�o!
   // c = create     :���ɮפ��s�b�ɫإ߷s��
   // r = read only  :��Ū�Ҧ��}��, Send() �Ǧ^ -1
   // w = overrwrite :�мg�Ҧ��}��, Recv() �Ǧ^ -1
   // a = append     :���[�Ҧ��}��, Recv() �Ǧ^ -1
   // time format = hh:mm:ss
   // filename �i�� %4y �N��褸�~, %2y �N�����~, %2m �N���, %2d�N���
   // speed : read only�ɨϥ�, format = <bytes> * <times/sec>
   //         �Ҧp: 100*10 = �C��Ū10��,�C��100bytes
   // default = read only,�ߧY�}��,�ä�����,�C��Ū���@��1024bytes
   K_mf(bool)         SettingApply  (const std::string&);
   K_mf(std::string)  GetSetting    ();
   // UID = filename-��ڶ}�Ҫ��ɮצW��
   K_mf(std::string)  GetUID        ();
   K_mf(int)          Send          (const void* buffer, size_t size);
   K_mf(int)          Recv          (      void* buffer, size_t size);

protected:
   K_mf(bool)         OpenImpl      ();
   K_mf(bool)         CloseImpl     ();

private:
   class TImpl;
   friend class TImpl;
   TImpl*         Impl_;
   std::string    Setting_;
};
/////////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------------------------
#endif
