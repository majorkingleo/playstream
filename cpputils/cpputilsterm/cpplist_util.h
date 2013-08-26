#ifndef CPPLIST_UTIL_H
#define CPPLIST_UTIL_H

/*
 *   $Log: cpplist_util.h,v $
 *   Revision 1.11  2011/04/28 12:33:10  wamas
 *   Toolsbox 2010 Portierung mit Interix
 *
 *   Revision 1.1  2011/04/27 07:52:35  wamas
 *   added to WEIHS
 *
 *   Revision 1.10  2010/12/13 22:40:28  wamas
 *   Funktion fÃ¼r das definieren eines eiegen Drucke Callbaks defiiert
 *
 *   Revision 1.9  2010/04/08 15:39:35  wamas
 *   Anzahl der Zeilen, die Standardmaesig angezeigt werden sollen
 *   Einstellbar machen.
 *
 *   Revision 1.8  2010/03/02 15:43:17  wamas
 *   Bugfix Liste Inventurstatus
 *
 *   Revision 1.7  2010/02/15 08:15:52  wamas
 *   Bugfixes
 *
 *   Revision 1.6  2010/02/09 12:53:19  wamas
 *   Serverportierung
 *
 *   Revision 1.5  2010/02/08 14:01:28  wamas
 *   Bugfixes
 *
 *   Revision 1.4  2010/02/08 10:40:28  wamas
 *   Bugfixes
 *
 *   Revision 1.3  2010/02/08 05:51:13  wamas
 *   WVZ Anpassungen
 *
 *   Revision 1.2  2009/10/21 12:23:53  wamas
 *   Toolsbox Port
 *
 *   Revision 1.1  2008/09/26 14:17:55  wamas
 *   Umbenennung von list_util.h in cpplist_util.h
 *
 *   Revision 1.1.1.1  2008/08/14 14:11:32  moberza
 *   Initial import
 *
 *   Revision 1.5  2008/06/23 10:54:45  wamas
 *   CLP Checks
 *
 *   Revision 1.4  2008/05/27 07:22:22  wamas
 *   Merged with before_sap_branch
 *
 *   Revision 1.3  2007/03/30 13:11:54  wamas
 *   sis: Bugfixes + Wildcard's als Filterkriterium erlauben.
 *
 *   Revision 1.2  2007/02/22 11:11:33  wamas
 *   sis: Bugfixing Listen ...
 *
 *   Revision 1.1  2007/02/19 07:14:03  wamas
 *   sis: Eingecheckt
 *
 *   Revision 1.5.4.6  2006/05/29 12:58:59  wamas
 *   MOBERZA Wenn man ein Listenlayout auswaelt soll, der Focus nicht
 *   auf den "Editieren" Button springen.
 *
 *   Revision 1.5.4.5  2006/05/22 06:23:19  wamas
 *   Updates
 *
 *   Revision 1.5.4.4  2006/05/19 07:31:56  wamas
 *   MOBERZA Bugfixes
 *
 *   Revision 1.5.4.3  2006/05/17 15:12:57  wamas
 *   AKP Protokoll hinzugefuegt, Codereduktion durch ListStarter Template
 *   Und den OPM Callback bei den Filter fuer die Protokollzeit herausgenommen
 *
 *   Revision 1.5.4.2  2006/05/17 09:29:34  wamas
 *   MOBERZA Inbetriebnahme
 *
 *   Revision 1.5.4.1  2006/05/11 07:54:09  wamas
 *   Toolsbox Umstellung
 *
 *
 */

#include <logtool.h>
#include <sqllist.h>
#include <mumalloc.h>
#include <owil.h>
#include <li_util.h>
#include <ml.h>
#include <sqltable.h>
#include <vector>
#include <string>
//#include <me_util.h>
#include <cpp_util.h>
#include <ref.h>

/*
 * template dass ein WAMAS-M/M Wartungsmenü anhand eines Listenrecords öffnet
 */
#if 0
template <typename TABLE>
bool start_wart_menu( OWidget w, TABLE *record, 
					  const std::string & TableName, 
					  const std::string & DlgName, 
					  const std::string & AtomName  )
{
  REC_DB *mc = NULL;
  TABLE *pTableNow, *pTableBefore, *pTableInDb;
  OWidget shell;
  MskDialog mask;

  shell = ApShellModalCreate(w, AP_CENTER, AP_CENTER);
  mask = (MskDialog )MskOpenMask(NULL, const_cast<char*>(DlgName.c_str()) );

  if( !mask )
	{
	  printf( "mask %s not found\n", DlgName.c_str() );
	  return false;
	}
  
  if( VaMultipleMalloc(   (int)sizeof(REC_DB),	  (void**)&mc,
						  (int)sizeof(TABLE),     (void**)&pTableBefore,
						  (int)sizeof(TABLE),     (void**)&pTableNow,
						  (int)sizeof(TABLE),     (void**)&pTableInDb,
						  (int)(-1)) == NULL )
	{
	  printf( "VaMultipleMalloc failed!\n" );
	  return false;
	}

  memset( mc, 0, sizeof(REC_DB) );
  memset( pTableBefore, 0, sizeof(TABLE) );
  memset( pTableNow, 0, sizeof(TABLE) );
  memset( pTableInDb, 0, sizeof(TABLE) );

  mc->pRecBefore = pTableBefore;
  mc->pRecNow    = pTableNow;
  mc->pRecInDb   = pTableInDb;

  MskAtomAdd(mask, NULL, const_cast<char*>(AtomName.c_str()), NULL, 
			 (Value )(REC_DB *)mc);
  
  Tdb_FillTableAtom(mask,const_cast<char*>(TableName.c_str()),record,1);
  
  memcpy(pTableNow, record, sizeof(TABLE));

  WdgSet(shell, WdgNshellAttrSizeable, (Value )1);
  if (MskRcCreateDialog(shell, mask)) {
	WdgMainLoop();
  }		     

  return true;
}
#endif

// default destroy callback für listen
extern "C" void destroy_callback_default (ListTdialog *ptListDialog);

class StandardList
{
 public:
  typedef printerCap  *(*ldSelectPrinterFunc)(OWidget, Value);

protected:
   static int list_lines;

  std::string rc_files;       // Kommaseparierte liste
  std::string title;          // Maskentitel
  std::string id_name;        // id name (fürs lists file)
  std::string sel_mask;       // Name des Filters im se_foo.rc file

  // die folgenden variablen sind nur dann notwendig, wenn ein 
  // wartungsmenü aufgerufen werden soll

  std::string table_name;      // Tabellenname des Wartungsmenü
  std::string part_name;       // PN_NAME auf den der Callback horcht
  std::string part_table_name; // Tabellenname des Wartungsmenü, 
                               // wenn dieser eventuell ein anderer ist, als der table_name

  std::string atom_name;       // atom name zb ATOM_MAK
  std::string wart_dlg_name;   // name des Wartungdialogs zb: WamDlgATOM

  bool auto_free_part_record;  // eventuell wenns speicherprobleme gibt würgaround

  /* Sollen mehrere Wartungsmenüs aufgerufen werden können,
   * so ist die Struktur entsprechend zu füllen und ein part_name.push_back() zu machen 
   * Die Variablen da oben, werden dann alle ignoriert.
   */
  struct Part 
  {
    std::string name;            // PN_NAME
    std::string table;           // tabellenname des Wartungsmenüs
    std::string wart_dlg_name;   // name des Wartungdialogs zb: WamDlgATOM
    std::string atom_name;       // atom name zb ATOM_MAK

    Part( const std::string &name, 
	  const std::string &table, 
	  const std::string &dlg,
	  const std::string &atom_name )
      : name(name), 
	 table(table), 
	 wart_dlg_name(dlg), 
	 atom_name(atom_name)
    {}
  };

  std::vector<Part> part_names;

  Value user_data;

  ListTdialog *plistdialog;

private:
  listSelItem * p_selector;
  unsigned long selector_size;

  StandardList( const StandardList & list );
  StandardList & operator=( const StandardList & list );
  
  bool sql_stmt_printed;

public:  
  StandardList();
  virtual ~StandardList() {}


  // Startfunktion
  virtual int start( OWidget parent, Value data, MskDialog *mask_rl );


  /* callback fürs Wartungsmenü
   * Wenn kein Wartungsmenü verwendet wird, braucht die Funktion nicht überladen werden 
   */
  virtual bool do_start_wart_menu( OWidget w, void *record, 
				   const std::string & TableName, 
				   const std::string & DlgName, 
				   const std::string & AtomName  );

  /* größe der großten Struktur fuers Wartungsmenue
   * muß eventuell überladen werden.
   */
  virtual unsigned getPartStructSize()
  {
	return getStructSize( TO_CHAR(table_name) );
  }

  virtual void GetSelector( listSelItem * & selector, unsigned long & size ) = 0;

  // dann überladen, wenn man sich zb seinen eigenen Footer bastelt
#if TOOLS_VERSION < 37
  virtual int sql_callback( lgRecords *sql, ListTdialog *list, int reason, char *stmt, void *ud );
#else
  virtual int sql_callback( lgRecords *sql, ListTdialog *list, int reason, const char *stmt, void *ud );
#endif

  // dann überladen, wenn man eingenes SQL-Statement braucht
  virtual int gen_callback(void *cd, int reason);

static char *AddStrF(LQ_DESC *pLq, char *sqlfilter, bool add_and_before = true );
static char *AddTimeF(LQ_DESC *pLq, char *StmtPart);
  static void _uwc(char *s);

  /*
   *  Die nächsten Funktionen brauch in der Regel nicht überladen zu werden.
   *  Also eigentlich nie.
   */

  // destroy callback
  virtual void destroy_my_self( ListTdialog *ptListDialog );

  /* selection callback
   * ruft do_start_wart_menü auf
   */
  virtual int selection_callback( elThandle *hEh, int iSel_reason, OWidget hW, 
								   selTdata *ptSelData );
  
  virtual listSelItem* getSelector();
  virtual unsigned long getSelectorSize();

  virtual ldSelectPrinterFunc getPrinterFunc() { return NULL; }

  void setUserSata( Value ud ) { user_data = ud; }
  Value getUserData() const { return user_data; }

  // hier den Selektor vorbefüllen
  virtual void fillSelector( listSelItem *selector );

  virtual std::string getLqMaskName();

  static void my_destroy_callback( ListTdialog *ptListDialog );
  static int my_sel_callback( elThandle *hEh, int iSel_reason, OWidget hW, 
							  selTdata *ptSelData, void *pvCalldat );

#if TOOLS_VERSION < 37
  static int my_cb_sqlList(lgRecords *, ListTdialog *, int, char *, void *ud);
#else
  static int my_cb_sqlList(lgRecords *, ListTdialog *, int, const char *, void *ud);
#endif

  static int my_gen_callback(void *cd, int reason);  

  static void set_list_lines( int lines );

  // Wird zum Drucken aufgerufen. Ab Owil 7 nur noch ueber diese Funktion
  virtual int doPrint( elThandle *hEh, int iSel_reason, OWidget hW, selTdata *ptSelData ) { return 0; }
};

class ListSelectionCallback
{
 public:

  virtual ~ListSelectionCallback();

  virtual void do_start_wart_menu( OWidget w, void *record, 
				   const std::string & TableName, 
				   const std::string & DlgName, 
				   const std::string & AtomName  )=0;
};

// Template Funktor zum Starten der Listen.
template<class List> class ListStarter
{
public:
  ListStarter()
  {}

  int operator()(OWidget parent, Value ud)
  {
	/* das geht nur deswegen, weil dies eben ein template ist
	 * und warum das Ganze?: damit man eine Maske nur einmal aufmachen kann
	 */
	static MskDialog	mask_rl = (MskDialog )NULL;
	
	if (mask_rl  &&  SHELL_OF(mask_rl)) {
		WdgGuiSet (GuiNactiveShell, (Value)SHELL_OF(mask_rl));
		return RETURN_ACCEPTED;
	}

	/* Das ist so ok, der destruktor wird über einen Callback aufgerufen,
	 * und die Liste zerstört sich dann selbst.
	 */
	List *list = new List();

    return list->start(parent, ud, &mask_rl);
  }

  int operator()(OWidget parent, Value ud, Tools::Ref<ListSelectionCallback> cb )
  {
	/* das geht nur deswegen, weil dies eben ein template ist
	 * und warum das Ganze?: damit man eine Maske nur einmal aufmachen kann
	 */
	static MskDialog	mask_rl = (MskDialog )NULL;
	
	if (mask_rl  &&  SHELL_OF(mask_rl)) {
		WdgGuiSet (GuiNactiveShell, (Value)SHELL_OF(mask_rl));
		return RETURN_ACCEPTED;
	}

	/* Das ist so ok, der destruktor wird über einen Callback aufgerufen,
	 * und die Liste zerstört sich dann selbst.
	 */
	List *list = new List(cb);

	return list->start(parent, ud, &mask_rl);
  }
};

extern "C" void CbSetFocusTo( MskDialog mask, 
							  MskStatic ef,
							  MskElement el,
							  int reason,
							  void *cbc );

#endif

