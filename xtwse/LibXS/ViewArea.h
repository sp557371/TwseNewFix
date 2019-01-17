//
// [ev_�e��] ����ܾ���
//    class TViewArea;
//
// ����: $Id: ViewArea.h,v 1.12 2006/04/03 14:27:58 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ViewAreaH
#define ViewAreaH
/////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <memory>
#include "ViewCursor.h"
//---------------------------------------------------------------------------
//��920817:
//    1. OnViewAfRowInsert(TViewArea&, size_type, size_type, const ev_iterator& ifrom)    �W�[ifrom
//    2. �W�[ OnViewBfRowErase(TViewArea&, size_type, size_type, const ev_iterator& ifrom) �ƥ�
//��920818:
//    1.�ǳƼW�[ TViewControl ���䴩 (�P��[�H�F�[]����[�����e��]���Q�k)
//        ����@�_�ӤӹL�O��, �Ȯɥ��b ViewArea ���ѿﶵ(TViewControls)
//        �M��� TevcViewer �ھڿﶵ�Ӳ��͹�����UI����
//    2.�N TViewArea, TViewCursor, TViewControl ����3���ɮ�
//��920925:
//    1.�W�[ SetMasterVA() �� virtual member function, �� TVACrControl:: �I�s
//    2.�ק� TVADetailLinker<>
//
//��20081021:
//    �W�[ TSettingVA<>
//    �W�[ LIBNS_fn(bool) AssignVA (TViewArea& dst, TViewArea& src);
namespace Kway {
using namespace Kstl;
class TViewArea;
//---------------------------------------------------------------------------
enum TViewControl {
         vco_InputKey,
         vco_Erase,
         vco_Insert,
         vco_CopyCurrent,
         vco_MoveCurrentUpDn,
         vco_Setting,
         vco_ReadOnly,           ///<�Y���]�w��flag,�h�H�Wvco_Erase..vco_Setting�ҵL��.
         vco_AlwaysShowLastRow,  ///<�p�G��а��b�̫�@��,���ݷs�W��Ʈ�,�h�۰ʲ���̫�.

         vco_DataExport,         ///<���\�ϥΪ̱N��ƶץX.
         vco_SettingDoubleCheck, ///<��ϥΪ̫��U�T�w��,�n�D��J���f���v���̪��b���K�X.

         vco_KeepKeySpaces,      ///<�O�dKey���e��ť�.
         vco_DetailAtBottom,     ///<�w�]�NDetail��b�U��.

         vco_End
};
typedef std::bitset<vco_End>  TViewControls;
/////////////////////////////////////////////////////////////////////////////
typedef std::vector<std::string>    TStrings;
//---------------------------------------------------------------------------
struct TVAGetEditProp   //���o�s����ݩ�
{
   #ifdef K_EnumPacked_Bug
      typedef byte   TEditStyle;
      enum TEditStyle_t
   #else
      enum TEditStyle
   #endif
   {
      es_NotSupported,  //���䴩
      es_AllRecsEqual,  //����Rec�������,���ϥάۦP���C�|
      es_EnumsByKey,    //�ھڨC����ƪ�Key,�Ӧ����P���C�|
   } K_EnumPacked;
   //���o�s���J�ɪ�[�C�|����], selectOnly[out]=�ȯ����C�|������(default=false)
   // result==0��ܴ��լO�_�䴩������
   virtual K_mf(TEditStyle)   GetEnums  (const TViewArea&, const TViewCursor*, size_t fieldNo, TStrings* result, bool& selectOnly)
                                          { return es_NotSupported; }

   //����[�ٲ�]����, �Y result==0,��ܴ��լO�_�䴩���\��,
   //  �Y result != 0, �h����[���o�ٲ�]����:
   //  �Ҧp: ���ɱz�i�H�X�{�@�ӹ�ܵ���, �n�D�ϥΪ̿�ܤ@���ɮ�
   //        ���ɶǦ^true��ܷs����Ʃ�bresult����, �_�h�Ǧ^false,��ܤ��@�������!
   virtual K_mf(bool)  GetEllipsis  (const TViewArea&, const TViewCursor*, size_t fieldNo, std::string* result)
                                          { return false; }

   //�O�_���\�i�J�s��Ҧ�
   virtual K_mf(bool)  CanEdit  (const TViewArea&, const TViewCursor*, size_t fieldNo)    { return true; }
};
//---------------------------------------------------------------------------
template <class Container>
class TVAEditProp_GetKeys : public TVAGetEditProp
{  // ���X�Y�� Container ������ keys ��@��J�ɪ�[�C�|]
   Container&  Container_;
protected:
   K_mf(TEditStyle)  GetEnums  (const TViewArea&, const TViewCursor*, size_t, TStrings* result, bool&)
                                    { if(result) {
                                         typename Container::iterator   iend = Container_.end();
                                         for(typename Container::iterator  i = Container_.begin();  i != iend;  ++i)
                                            result->push_back(Container_.get_key(i).as_string());
                                      }
                                      return es_AllRecsEqual;
                                    }
public:
   K_ctor  TVAEditProp_GetKeys  (Container& c)  : Container_(c)  { }
};
/////////////////////////////////////////////////////////////////////////////
#ifdef K_EnumPacked_Bug
   typedef byte   TAlignment;
   enum TAlignment_t
#else
   enum TAlignment
#endif
{
      //����(x�b)��m�վ�
      ta_xLeft   = 0x00,
      ta_xCenter = 0x01,
      ta_xRight  = 0x02,
      ta_xMask   = 0x0f,

      //����(y�b)��m�վ�
      ta_yTop    = 0x00,
      ta_yCenter = 0x10,
      ta_yBottom = 0x20,
      ta_yMask   = 0xf0,

      //�`�Ϊ��w�q
      ta_xLeft_yCenter   = ta_xLeft  |ta_yCenter,
      ta_xCenter_yCenter = ta_xCenter|ta_yCenter,
      ta_xRight_yCenter  = ta_xRight |ta_yCenter,
} K_EnumPacked;
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef word   TFieldStyle;
   enum TFieldStyle_t
#else
   enum TFieldStyle
#endif
{
      tfs_Null     = 0,
      tfs_Calc     = 0x0001,  //�p��X�Ӫ����,�S����ڪ��x�s�Ŷ�(���ä���ܤ��i�s��)
      tfs_CanEdit  = 0x0002,  //�i�s�誺���,��GUI���󪺰t�X(�Ҧp:VAGrid�ݳ]�w Options �� goEditing)
      tfs_Hide     = 0x0004,  //������(�����)
      tfs_Fixed    = 0x0008,  //����T�w���
      tfs_MenuItemBit = 0x0010,  //��涵��, vac�|�N�����[�J[PopupMenu]����,�W�h�аѦ�: FevcViewer.cpp
      tfs_MenuItem = tfs_MenuItemBit|tfs_Hide,
      tfs_Button   = 0x0020,  //��줺�e = [0]=tfsmi, [1..n]=ButtonText, ����s�Q���U��, �|�]�w��줺�e��: "C"

      tfs_nouse    = 0xffff,
} K_EnumPacked;
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   TFieldStyleMenuItem;
   enum TFieldStyleMenuItem_t
#else
   enum TFieldStyleMenuItem
#endif
{
   tfsmi_Enabled       = 0x01,
   tfsmi_Checked       = 0x02,
   tfsmi_Hide          = 0x04,
   tfsmi_HasGroupIndex = 0x08,   // + GroupIndex = "0".."255"
   tfsmi_RadioItem     = 0x10,
   tfsmi_DialogBit     = 0x20,
   tfsmi_Dialog        = 0x20|tfsmi_Enabled,
                        // + "Title" + '\n' + "DefaultString" + '\n' + "Prompt"
                        // for button message confirmation:
                        //    DefaultString = '\0x01' + TFieldStyleDialogButtons + TFieldStyleDialogIcon
} K_EnumPacked;
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   TFieldStyleDialogButtons;
   enum TFieldStyleDialogButtons_t
#else
   enum TFieldStyleDialogButtons
#endif
{
   fsdb_None   = 0x00,
   fsdb_OK     = 0x01,
   fsdb_Yes    = 0x02,
   fsdb_No     = 0x04,
   fsdb_Cancel = 0x08,
   fsdb_Abort  = 0x10,
   fsdb_Retry  = 0x20,
   fsdb_Ignore = 0x40,

   fsdb_AbortRetryIgnore = fsdb_Abort|fsdb_Retry|fsdb_Ignore,
   fsdb_OKCancel         = fsdb_OK|fsdb_Cancel,
   fsdb_RetryCancel      = fsdb_Retry|fsdb_Cancel,
   fsdb_YesNo            = fsdb_Yes|fsdb_No,
   fsdb_YesNoCancel      = fsdb_Yes|fsdb_No|fsdb_Cancel,
} K_EnumPacked;

#ifdef K_EnumPacked_Bug
   typedef byte   TFieldStyleDialogIcon;
   enum TFieldStyleDialogIcon_t
#else
   enum TFieldStyleDialogIcon
#endif
{
   fsdi_Warning,     //A message box containing a yellow exclamation point symbol.
   fsdi_Error,       //A message box containing a red stop sign.
   fsdi_Information, //A message box containing a blue "i".
   fsdi_Confirmation,//A message box containing a green question mark.
} K_EnumPacked;

enum TVADlgNotify {
   vadn_OnShow      = 0x01,
   vadn_OnClose     = 0x02,
   vadn_KeepVisible = 0x04,//�@����ܿ�J��ܵ�, ������U[����] �� [��s���@���ܵ�] ����.
   vadn_None        = 0x00,
};

LIBNS_fn(std::string) FieldMakeDialogString(const std::string& title, const std::string& prompt, const std::string& defaultString, TVADlgNotify vadn = vadn_None);
LIBNS_fn(std::string) FieldMakeDialogString(const std::string& title, const std::string& prompt, TFieldStyleDialogButtons, TFieldStyleDialogIcon, TVADlgNotify vadn = vadn_None);
//---------------------------------------------------------------------------
struct TFieldDesp {
   //��ܮɪ��R�A�ݩ�:
   const char*    Title_;
   //word         TitleDisplayWidth_;  //�����ܼe��(pixels) = TitleDisplayWidth_ * �r���e��(pixels) (0=default)
   word           FieldDisplayWidth_;  //�����ܼe��(pixels) = FieldDisplayWidth_ * �r���e��(pixels) (���i��0)

   TAlignment     TitleAlignment_;     //���D������覡
   TAlignment     FieldAlignment_;     //����ƪ�����覡
   TFieldStyle    FieldStyle_;

   char           PasswordChar_;

   //�i�J�s���J�Ҧ��ɪ��ʺA�ݩ�:
   TVAGetEditProp*   GetEditProp_;
};
/////////////////////////////////////////////////////////////////////////////
enum TVAEvMask {
      vaem_OnViewAfEndUpdate, //��Ʋ���
      vaem_OnViewAfEndUpdate2,//�ϥ� TViewCursor �q������.
      vaem_OnViewRowMoved,  //TopRow�BCurRow [�G�̤��@]��[�G�̦P��]������

      vaem_OnViewClear,     //�M�����������,TopRow�BCurRow�Ҥw�L��,������ ViewRowMoved �ƥ�
      vaem_OnViewAssigned,  //�s��Ƥw�Ƨ�(���e����ƥ����L��),TopRow�BCurRow�������VBegin,������ ViewRowMoved �ƥ�
                            //���� OnViewClear() �� OnViewAssigned() ��, ���Title �]�������s�B�z!!
      vaem_OnViewBfDestroy,

      //�b���� OnViewAfRowInsert�BOnViewAfRowErase ���e,�|�NTopRow�BCurRow�վ�n(�������ͨƥ�),
      //�b���� OnViewAfRowInsert�BOnViewAfRowErase ����,�YTopRow��CurRow���ܰ�,�~�|���� OnViewRowMoved() �ƥ�
      //�� OnViewAfRowInsert�BOnViewAfRowErase ���b�o�ͮ�, �����\�վ� TopRow�BCurRow ����m
      vaem_OnViewAfRowInsert,
      vaem_OnViewAfRowErase,
      vaem_OnViewBfRowErase,

      vaem_OnViewRepaint,  //�j����s�e��(�q����Ʀ�����,��SettingVA��,�h�Ұ�[�M��]���s)

      vaem_End
};
//---------------------------------------------------------------------------
LIBNS_class TVAEvHandler : public ev_handler_base<vaem_End>
{
public:
   typedef size_t    size_type;

private:
   friend class TViewArea;
   // (size_type frombegin)
   virtual K_mf(void) OnViewAfEndUpdate (TViewArea&, size_type, const ev_updater_impl&) { ev_mask_.reset(vaem_OnViewAfEndUpdate);  }
   virtual K_mf(void) OnViewAfEndUpdate2(TViewArea&, const TViewCursor&)                { ev_mask_.reset(vaem_OnViewAfEndUpdate2); }

   //�Y(const TViewCursor* oldCur, const TViewCursor* oldTop) �P�ɬ�0
   //�h��ܨS��old�i�H�Ѧ�,�����]�w�s�� TopRow�BCurRow
   virtual K_mf(void) OnViewRowMoved    (TViewArea&, const TViewCursor*, const TViewCursor*)
                                                      { ev_mask_.reset(vaem_OnViewRowMoved);  }

   virtual K_mf(void) OnViewClear       (TViewArea&)  { ev_mask_.reset(vaem_OnViewClear);     }
   virtual K_mf(void) OnViewAssigned    (TViewArea&)  { ev_mask_.reset(vaem_OnViewAssigned);  }
   virtual K_mf(void) OnViewBfDestroy   (TViewArea&)  { ev_mask_.reset(vaem_OnViewBfDestroy); }

   // (size_type frombegin, size_type count)
   virtual K_mf(void) OnViewAfRowInsert (TViewArea&, size_type, size_type, const ev_iterator&)
                                                                           { ev_mask_.reset(vaem_OnViewAfRowInsert); }
   virtual K_mf(void) OnViewAfRowErase  (TViewArea&, size_type, size_type) { ev_mask_.reset(vaem_OnViewAfRowErase);  }
   virtual K_mf(void) OnViewBfRowErase  (TViewArea&, size_type, size_type, const ev_iterator&)
                                                                           { ev_mask_.reset(vaem_OnViewBfRowErase);  }

   //�j����s�e��(�q����Ʀ�����,��SettingVA��,�h�Ұ�[�M��]���s)
   virtual K_mf(void) OnViewRepaint (TViewArea&)  { ev_mask_.reset(vaem_OnViewRepaint); }
};

/// Kstl::ev_container ������˵���.
LIBNS_class TViewArea : private container_ev_handler
{
// TViewArea ���B�@�޿�:
//  1.�����d�� = [TopRow �� TopRow+VisibleRowCount)
//  2.�� TopRow ���ܮ�, ���|���� CurRow, �ҥH CurRow ���i��b[�����d��]���~
//  3.�� CurRow ���ܮ�, �|�N CurRow �վ��[�����d��]��,�]�N�O���i����� TopRow
//  4.���� OnViewRowMoved() �ƥ��, ���i�� cur,top �P�ɵo��,�Υu���䤤���@
//  5.���� OnViewRowMoved() �ƥ��, ���׬O�]�� CurRow �� TopRow �����ܦӲ��ͪ��ƥ�,
//    �������\�A���� CurRow �� TopRow, �קK�ƥ󻼰j�o��, �j�T���C�ƥ�B�z�{����������
//
//
   typedef container_ev_handler  inherited;
   typedef TViewArea             this_type;
public:
   typedef size_t                field_n_type;
   typedef inherited::size_type  size_type;

   static TViewControls  DefaultViewControls  ();  // vco_InputKey,vco_Erase,vco_Insert,vco_CopyCurrent

           K_ctor  TViewArea  (TViewCursor* top, TViewCursor* cur, const TViewControls&);
   virtual K_dtor ~TViewArea  ();

   inline  K_mf(const TViewCursor*)   GetTopRow        () const  { return TopRow_;       }
   inline  K_mf(const TViewCursor*)   GetCurRow        () const  { return CurRow_;       }
   inline  K_mf(const TViewControls&) GetViewControls  () const  { return ViewControls_; }
   inline  K_mf(void)                 SetViewControls  (const TViewControls& vac)            { ViewControls_ = vac; }
   inline  K_mf(void)                 SetViewControls  (TViewControl vco, bool value=true)   { ViewControls_.set(vco, value); }

   inline  K_mf(size_type)  GetVisibleRowCount  () const              { return VisibleRowCount_;     }
   inline  K_mf(void)       SetVisibleRowCount  (size_type rowCount)  { VisibleRowCount_ = rowCount; }
   virtual K_mf(size_type)  GetRowCount         () const = 0;
   virtual K_mf(bool)       IsEmpty             () const = 0;

//���o���y�z
   virtual K_mf(field_n_type)       GetFieldCount  () const = 0;
   virtual K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type) const = 0;
   virtual K_mf(std::string)        GetFieldStr    (const ev_iterator&, field_n_type) const = 0;

//���ʦ�m,�Ǧ^:�q���w��m(begin,end,current)���F�X�B(�i��<0,��steps�өw)
//         steps�i�H >0 <0 �� =0 �ҥi
   K_mf(bool) IsRowChanging  () const  { return RowChanging_; }
   K_mf(int)  MoveCurRow  (int steps, TViewCursorMoveFrom from) //�b MoveCurRow() ����, TopRow���i��|����
                              { return MoveCursor(CurRow_, steps, from, &this_type::CurRowChanged); }
   K_mf(int)  MoveTopRow  (int steps, TViewCursorMoveFrom from)
                              { return MoveCursor(TopRow_, steps, from, &this_type::TopRowChanged); }
//�M����,�Y���h����CurRow
virtual K_mf(bool)  Find  (const char* key, bool isLowerBound);

//�W�[/�R��, �@�����
virtual K_mf(bool)  Fetch  (const char* key, bool copyCurrent)  { return false; }
virtual K_mf(bool)  Erase  (const char* key)                    { return false; }
virtual K_mf(bool)  Clear  ()                                   { return false; }

//�N CurRow �����,���W(direction>0) or ���U(direction<0)����:
//   �P�e�@��,�ΤU�@����Ƨ@�洫(swap), ��direction==0��,��ܸ߰ݱz�� ViewArea �O�_�䴩 SwapCurrent()
virtual K_mf(bool)  SwapCurrent  (int direction)   { return false; }

//�ƥ���U����
   inline K_mf(void)  DelEvHandler  (TVAEvHandler* a)    { evs_.del(a); }
   inline K_mf(void)  AddEvHandler  (TVAEvHandler* a)    { evs_.add(a);
                                                           if(TopRow_  &&  CurRow_)
                                                              a->OnViewRowMoved(*this,0,0); }
//SettingApply() / SettingCancel() ���Ѥ@�� [�s��ɪ�Container] �P [��ڧ@�~�ɪ�Container] �������S��
// �i�Ѧ� SesMgr.cpp ���� TSesMgr::TSetting �����k
virtual K_mf(bool)  SettingApply   ()  { return false; }
virtual K_mf(bool)  SettingCancel  ()  { return false; }

virtual K_mf(void)  SetMasterVA    (TViewArea*)  {}

/** IdName: �ѧO��VA�Ϊ��W�r,�i�Ψ��x�s��VA���@�ǯS��: �Ҧp[�ۭq���e��].
*/
K_mf(void)               SetIdName (const std::string& v)   { IdName_ = v;    }
K_mf(const std::string&) GetIdName () const                 { return IdName_; }

///> �Y��Setting,�h�|�i�J�����ʪ����A, FevcViewer::bbApply �|�ܦ� enabled.
inline K_mf(void) SetChanged ()  { Repaint(); }

protected:
   inline K_mf(void)  RegTo      (ev_container& c)  { c.add_ev_handler(this); }
   inline K_mf(void)  UnregFrom  (ev_container& c)  { c.del_ev_handler(this); }

   K_mf(void)  AfChangeContainer  (ev_container* bf, ev_container* af,
                                   TViewCursor* top, TViewCursor* cur);

   K_mf(void)  call_OnBfDestroy        ();
   K_mf(void)  call_OnContainerAfClear ();

   typedef void (K_FASTCALL this_type::*TCursorChanged) (TViewCursor& old);
   K_mf(int)   MoveCursor     (TViewCursor*, int steps, TViewCursorMoveFrom, TCursorChanged);
   K_mf(void)  CurRowChanged  (TViewCursor& oldCurRow);
   K_mf(void)  TopRowChanged  (TViewCursor& oldTopRow);
   K_mf(void)  call_OnViewRowMoved (TViewCursor* oldcur, TViewCursor* oldtop);
   K_mf(void)  AdjTopRow      ();

   typedef pod_vector<TVAEvHandler*>  TVAEvHandlers;
   typedef TVAEvHandlers::iterator    TVAEvHandleri;
   TVAEvHandlers  evs_;

   TViewCursor*   TopRow_;
   TViewCursor*   CurRow_;
   TViewCursor*   UpdRow_;
   size_type      VisibleRowCount_;
   bool           RowChanging_;
   TViewControls  ViewControls_;
   std::string    IdName_;

//handle container events
   K_mf(void)  OnContainerBfDestroy  (const ev_container&);
// K_mf(void)  OnContainerBfClear    (const ev_container&);
   K_mf(void)  OnContainerAfClear    (const ev_container&);
// K_mf(void)  OnContainerBfAssign   (const ev_container&);
   K_mf(void)  OnContainerAfAssign   (const ev_container&);

// K_mf(void)  OnContainerBfInsert   (const ev_container&, const ev_iterator&, size_type);
   K_mf(void)  OnContainerAfInsert   (const ev_container&, const ev_iterator&, size_type);
   K_mf(void)  OnContainerBfErase    (const ev_container&, const ev_iterator&, size_type);
   K_mf(void)  OnContainerAfErase    (const ev_container&, const ev_iterator&, size_type);

// K_mf(void)  OnContainerBeginUpdate  (const ev_updater_impl&);
   K_mf(void)  OnContainerAfEndUpdate  (const ev_updater_impl&);

///> �j����s�e��,�Y��Setting,�h�|�i�J�����ʪ����A, FevcViewer::bbApply �|�ܦ� enabled.
   K_mf(void)  Repaint  ();

   TViewArea*  Parent_;
public:
   inline void       SetParentVA(TViewArea* parent)   { Parent_ = parent; }
   inline TViewArea* GetParentVA() const              { return Parent_; }
};
//---------------------------------------------------------------------------
typedef std::auto_ptr<TViewArea>    TaViewArea;
/////////////////////////////////////////////////////////////////////////////
struct TVAMkKey
{
template <class KeyType>
   inline K_mf(void)  operator()  (KeyType& akey, const char* ckey) const
                                  { akey.assign(ckey); }
};
//---------------------------------------------------------------------------
struct TVANoLog
{
template <class Container, class Iterator>
   inline K_mf(void)  WriteMod  (const Container&, const Iterator&,
                                 size_t,  //fieldno
                                 const std::string&,//af-str
                                 TViewArea*)
                                { }
template <class Container, class Iterator>
   inline K_mf(void)  WriteAdd  (const Container&, const Iterator&, TViewArea*)  { }
template <class Container, class Iterator>
   inline K_mf(void)  WriteDel  (const Container&, const Iterator&, TViewArea*)  { }
template <class Container, class Iterator>
   inline K_mf(void)  WriteCopy (const Container&, const Iterator&, const Iterator&, TViewArea*)
                                { }                    // ��from            ��to
template <class Container, class Iterator>
   inline K_mf(void)  WriteSwap (const Container&, const Iterator&, const Iterator&, TViewArea*)
                                { }
};
//---------------------------------------------------------------------------
template <class TL, class TR>
inline void VACopyRec (TL& left, const TR& right) {
   left = right;
}
template <class Container>
inline typename Container::iterator VAFetchRecIterator (Container& c, const typename Container::key_type& k) {
   return c.fetch(k);
}
//---------------------------------------------------------------------------
template <class Container, class FieldMaker, class KeyMaker = TVAMkKey,
          class VALogger = TVANoLog>
class TTViewArea : public TViewArea
{
   typedef TViewArea                                           inherited;
   typedef TTViewArea<Container,FieldMaker,KeyMaker,VALogger>  this_type;
public:
   typedef Container                                              container_type;
   typedef FieldMaker                                             field_maker_type;
   typedef KeyMaker                                               key_maker_type;
   typedef VALogger                                               valogger_type;
   typedef TTViewCursor<Container,FieldMaker,KeyMaker,VALogger>   cursor_type;

   K_ctor  TTViewArea  (container_type* c = 0, const FieldMaker& fmaker = FieldMaker(), const TViewControls& vcs = DefaultViewControls(),
                        VALogger* log = 0)
                           : inherited(c ? CursorFactory(c,fmaker,log) : 0,
                                       c ? CursorFactory(c,fmaker,log) : 0,
                                       vcs),
                             Container_(c),
                             FieldMaker_(fmaker),
                             VALog_(log)
                           { if(c)   RegTo(*c); }
   K_dtor ~TTViewArea  ()  { call_OnBfDestroy();
                             if(Container_)  UnregFrom(*Container_); }

   inline  K_mf(const cursor_type*)  GetTopRow  () const  { return static_cast<const cursor_type*>(TopRow_); }
   inline  K_mf(const cursor_type*)  GetCurRow  () const  { return static_cast<const cursor_type*>(CurRow_); }

   static  K_mf(cursor_type*)  CursorFactory  (container_type* c, const FieldMaker& fmaker, VALogger* log)
                                                   { return new cursor_type(c,fmaker,log); }

   K_mf(size_type)          GetRowCount    () const                { return Container_ ? Container_->size()  : 0;    }
   K_mf(bool)               IsEmpty        () const                { return Container_ ? Container_->empty() : true; }
   K_mf(field_n_type)       GetFieldCount  () const                { return FieldMaker_.GetFieldCount();  }
   K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const  { return FieldMaker_.GetFieldDesp(n);  }
   K_mf(std::string)        GetFieldStr    (const ev_iterator& i, field_n_type n) const
                                                                   { return FieldMaker_.Get(static_cast<const typename container_type::iterator&>(i), n); }

   virtual K_mf(bool)  Fetch  (const char* key, bool copyCurrent)
                                        { if(ViewControls_.test(vco_ReadOnly))
                                             return false;
                                          if(Container_ == 0) {
                                             if(copyCurrent)
                                                return false;
                                             container_type* c = OnCreateContainer();
                                             if(c == 0)
                                                return false;
                                             SetContainer(c);
                                          }
                                          if(copyCurrent  &&  GetCurRow()->IsInvalid())
                                             return false;
                                          typename container_type::key_type ckey;
                                          KeyMaker_(ckey,key);
                                          typename Container::iterator   icur   = GetCurRow()->GetIterator();
                                          typename Container::iterator   ifetch = VAFetchRecIterator(*Container_, ckey);
                                          if(copyCurrent) {
                                             typename Container::updater u = Container_->begin_update(ifetch);
                                             VACopyRec(*u, Container_->get_mapped(icur));
                                             if(VALog_)
                                                VALog_->WriteCopy(*Container_, icur, ifetch, this);
                                          } else {
                                             if(VALog_)
                                                VALog_->WriteAdd(*Container_, ifetch, this);
                                          }
                                          return Find(key,false); }
   virtual K_mf(bool)  Erase  (const char* key)
                                          { if(Container_==0  ||  ViewControls_.test(vco_ReadOnly))
                                              return false;
                                            typename container_type::key_type ckey;
                                            KeyMaker_(ckey,key);
                                            typename Container::iterator  i = Container_->find(ckey);
                                            if(i==Container_->end())
                                               return false;
                                            if(VALog_)
                                               VALog_->WriteDel(*Container_, i, this);
                                            return Container_->erase(i); }
   virtual K_mf(bool)  Clear  () {
      if(Container_==0  ||  ViewControls_.test(vco_ReadOnly))
         return false;
      Container_->clear();
      return true;
   }

   K_mf(void)  SetContainer  (container_type* c)
                              { if(c == Container_)
                                   return;
                                container_type* bf = Container_;
                                Container_ = c;
                                AfChangeContainer(bf, c,
                                                  c ? CursorFactory(c,FieldMaker_,VALog_) : 0,
                                                  c ? CursorFactory(c,FieldMaker_,VALog_) : 0);
                              }
   inline K_mf(container_type*)  GetContainer  ()  { return Container_; }
   inline K_mf(const FieldMaker&) GetFieldMaker() const { return FieldMaker_; }
protected:
   container_type*   Container_;
   FieldMaker        FieldMaker_;
   KeyMaker          KeyMaker_;
   VALogger*         VALog_;

//override virtual ev handler
   K_mf(void)  OnContainerBfDestroy  (const ev_container& c)
   {
      Container_ = 0;
      inherited::OnContainerBfDestroy(c);
   }

private:
   virtual K_mf(container_type*)  OnCreateContainer  ()   { return 0; }
};
/////////////////////////////////////////////////////////////////////////////
struct TSwapMapped
{
   template <class T>
      static inline K_mf(void)  swap_mapped  (T& a, T& b)    { a.swap(b); }
   template <class T>
      static inline K_mf(void)  swap_mapped  (T*& a, T*& b)  { T* t=a;  a=b;  b=t; }
};
//---------------------------------------------------------------------------
template <class Container, class FieldMaker, class KeyMaker = TVAMkKey>
class TTViewArea2 : public TTViewArea<Container, FieldMaker, KeyMaker>
{  //TTViewArea2 : �䴩 SwapCurrent()
   // ���ɱz�� FieldMaker �������� swap_mapped() �� method,
   //                     �άO�~�� class TSwapMapped,��class ���Ѥ@�ӳq�Ϋ��� swap_mapped()
   typedef TTViewArea<Container, FieldMaker, KeyMaker>   inherited;

#ifndef __BORLANDC__
protected:
   using inherited::ViewControls_;
   using inherited::Container_;
   using inherited::FieldMaker_;
   using inherited::VALog_;
public:
   using inherited::GetCurRow;
   using inherited::MoveCurRow;
   using inherited::DefaultViewControls;
#endif

public:
   K_ctor  TTViewArea2  (Container* c = 0, const FieldMaker& fmaker = FieldMaker(), const TViewControls& vcs = DefaultViewControls())
               : inherited(c,fmaker,vcs)
               { }

   virtual K_mf(bool)  SwapCurrent  (int direction)
                                          { if(ViewControls_.test(vco_ReadOnly))
                                               return false;
                                            if(direction==0)//�߰ݬO�_�䴩 SwapCurrent()
                                               return true;
                                            if(Container_==0)
                                               return false;
                                            typename Container::iterator   i = GetCurRow()->GetIterator();
                                            typename Container::iterator   inxt = i;
                                            if(direction > 0) {
                                               typename Container::iterator iend = Container_->end();
                                               if(i == iend)
                                                  return false;
                                               if(++inxt == iend)
                                                  return false;
                                               direction = 1;
                                            } else {// direction < 0
                                               if(i == Container_->begin())
                                                  return false;
                                               --inxt;
                                               direction = -1;
                                            }
                                            typename Container::updater ucur = Container_->begin_update(i);
                                            typename Container::updater unxt = Container_->begin_update(inxt);
                                            FieldMaker_.swap_mapped(*ucur,*unxt);
                                            MoveCurRow(direction, vcmfCurrent);
                                            if(VALog_)
                                               VALog_->WriteSwap(*Container_, i, inxt, this);
                                            return true; }
};
/////////////////////////////////////////////////////////////////////////////
template <class TVAMaster,  class TVADetail, class TDetailMaker>
class  TVADetailLinker : public TVADetail
                       , private TVAEvHandler//for handle TVAMaster event
{
/*
struct TDetailMaker {
   K_mf(TVADetail::container_type*)  operator()  (const TMasterCursor::iterator& i, bool isCreateNeeded);
};
*/
   typedef typename TVAMaster::cursor_type   TMasterCursor;

#ifndef __BORLANDC__
public:
   using TVADetail::SetContainer;
#endif

public:
   K_ctor  TVADetailLinker  (TVAMaster* vaMaster = 0, const TDetailMaker& detailMaker = TDetailMaker())
            : TVADetail(0),//0=container
              VAMaster_(0),
              DetailMaker_(detailMaker)
            { SetMasterVA(vaMaster); }
   K_ctor  TVADetailLinker  ( TVAMaster*           vaMaster
                            , const TDetailMaker&  detailMaker
                            , const typename TVADetail::field_maker_type& fmaker
                            , const TViewControls&                        vcs
                            , typename TVADetail::valogger_type*          log)
            : TVADetail(0, fmaker, vcs, log)
            , VAMaster_(0)
            , DetailMaker_(detailMaker)
            { SetMasterVA(vaMaster); }
   K_ctor  ~TVADetailLinker  ()
            { SetMasterVA(0); }

   inline K_mf(void)  SetDetailMaker  (const TDetailMaker& a)  { DetailMaker_ = a;
                                                                 ReloadDetailContainer(); }
   K_mf(void)  SetMasterVA  (TViewArea* vaMaster)
      { if(VAMaster_)   VAMaster_->DelEvHandler(this);
        VAMaster_ = static_cast<TVAMaster*>(vaMaster);
        if(vaMaster)    vaMaster->AddEvHandler(this); }

private:
   TVAMaster*     VAMaster_;
   TDetailMaker   DetailMaker_;

   K_mf(void)  ReloadDetailContainer  ()
         {
            const TMasterCursor* cur = static_cast<const TMasterCursor*>(VAMaster_->GetCurRow());
            SetContainer( (cur==0 || cur->IsInvalid()) ? 0 : DetailMaker_(cur->GetIterator(),false) );
         }

   // override VADetail virtual functions
   K_mf(typename TVADetail::container_type*)  OnCreateContainer  ()
         {
            if(VAMaster_->IsEmpty())
               return 0;
            const TMasterCursor* cur = static_cast<const TMasterCursor*>(VAMaster_->GetCurRow());
            if(cur==0  &&  cur->IsInvalid())
               return 0;
            return DetailMaker_(cur->GetIterator(),true);
         }

   // handle TVAMaster event
   K_mf(void)  OnViewRowMoved   (TViewArea&, const TViewCursor*, const TViewCursor*)
                                              { ReloadDetailContainer(); }
   K_mf(void)  OnViewBfDestroy  (TViewArea&)  { VAMaster_ = 0;
                                                SetContainer(0); }
   K_mf(void)  OnViewAfEndUpdate (TViewArea& va, TViewArea::size_type fromBegin, const ev_updater_impl&)
                                              { const TViewCursor*   cur = va.GetCurRow();
                                                if(cur  &&  !cur->IsInvalid()
                                                && cur->DistanceBegin()==fromBegin)
                                                   ReloadDetailContainer(); }
};
//---------------------------------------------------------------------------
///> �N src �����e��J dst, ��J�� dst ���i��|[�s�W/�R��/����]
///> src, dst �����ƶq�����@��, �Y���ƶq���P�h�Ǧ^ false.
LIBNS_fn(bool) AssignVA (TViewArea& dst, TViewArea& src);
//---------------------------------------------------------------------------
template <class TBaseVA>
class TSettingVA : public TBaseVA
{
   typedef TBaseVA inherited;
protected:
   typename inherited::valogger_type*  Logger_;
   typename inherited::container_type  TempRecs_;
   typename inherited::container_type* OrigRecs_;
   K_mf(bool) SettingCancel () {
      TempRecs_  = *OrigRecs_;
      return true;
   }
   K_mf(bool) SettingApply ()  {
      std::auto_ptr<TBaseVA> va(new TBaseVA(this->OrigRecs_, this->FieldMaker_, TViewArea::DefaultViewControls(), this->Logger_));
      AssignVA(*va, *this);
      return true;
   }
public:
   K_ctor TSettingVA ( typename inherited::container_type*         recs
                     , const typename inherited::field_maker_type& fieldMaker
                     , const TViewControls&                        vcs
                     , typename inherited::valogger_type*          logger)
      : inherited(NULL, fieldMaker, vcs, NULL)
      , Logger_(logger)
      , TempRecs_(*recs)
      , OrigRecs_(recs)
      { this->SetContainer(&TempRecs_);
      }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
