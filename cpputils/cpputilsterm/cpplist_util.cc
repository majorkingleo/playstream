#include "cpplist_util.h"
#include <owrcloader.h>
#include <string_utils.h>
#include <format.h>
#include <sqltable.h>
#include <t_util.h>


using namespace Tools;

int StandardList::list_lines = 22;

void StandardList::set_list_lines( int lines )
{
	list_lines = lines;
}

extern "C" void destroy_callback_default (ListTdialog *ptListDialog)
{
	if (ptListDialog)
		free(ptListDialog);
	return;
}

StandardList::StandardList()
  : auto_free_part_record( true ),
	user_data(0),
	p_selector(0),
	selector_size(0),
	sql_stmt_printed(false)
{
}

StandardList::StandardList( const StandardList & list )
{
}

StandardList & StandardList::operator=( const StandardList & list )
{
  return *this;
}

void StandardList::my_destroy_callback( ListTdialog *ptListDialog )
{
  StandardList *sl = (StandardList*) ptListDialog->ldUserData;
  
  if( sl )
	sl->destroy_my_self( ptListDialog );
}

int StandardList::my_sel_callback( elThandle *hEh, int iSel_reason, OWidget hW, 
								selTdata *ptSelData, void *pvCalldat )
{
  StandardList *sl = (StandardList*) pvCalldat;

  if( sl )
	return sl->selection_callback( hEh, iSel_reason, hW, ptSelData );
  
  return 0;
}

#if TOOLS_VERSION < 37
int StandardList::my_cb_sqlList(lgRecords *sql, ListTdialog *list, int reason, char *stmt, void *ud)
#else
int StandardList::my_cb_sqlList(lgRecords *sql, ListTdialog *list, int reason, const char *stmt, void *ud)
#endif
{
  if( list )
	{
	  StandardList *sl = (StandardList*) list->ldUserData;	  

	  if( sl )
		return sl->sql_callback( sql, list, reason, stmt, ud );
	}

  return 0;
}

/* sis: Generator Callback */
int StandardList::my_gen_callback(void *cb, int reason)
{
   ListTXdialog            *li=(ListTXdialog *)cb;
   ListTdialog 		   *list=li->list;
 
   if( list )
   {
	  StandardList *sl = (StandardList*) list->ldUserData;
	  
	  if( sl )
		return sl->gen_callback( cb, reason );
	}
  return 0;
}

int StandardList::selection_callback( elThandle *hEh, int iSel_reason, OWidget hW, 
									   selTdata *ptSelData )
{
  LM2_DESC	*ptLd;
  CbViewRec	*ptCbv;
  long		lLine,lRecnum, lRecoffset, lType, lLiType;
  char		*tRecord;
  unsigned	iObjSize;
  
  switch (iSel_reason) 
	{
	case SEL_REASON_SEL:

//	  if( wart_dlg_name.empty() && part_names.empty() )
//	  	break;

	  iObjSize = getPartStructSize();
	  if (iObjSize < sizeof (ListTfilter))
		iObjSize = sizeof (ListTfilter);
	  
	  ptLd = hEh->ehLM2_DESC;
	  ptCbv = &ptSelData->selView;
	  lLine = ptCbv->u.input.line;
	  
	  if (lm2_lin2rec(ptLd, lLine, &lRecnum, &lRecoffset) < 0)
		return 0;
	  if (lm2_seek(ptLd, lRecnum, 0) < 0)
		return 0;

	  tRecord = (char*)malloc (iObjSize);
	  if (tRecord == NULL)
		break;
	  memset (tRecord, 0, iObjSize);

	  std::cout << format( "record address: 0x%X\n", (void*)tRecord );

	  if (lm2_read(ptLd, &lType, tRecord) < 0) {
		free (tRecord);
		return 0;
	  }

	  // create a default entry
	  if( part_names.empty() )
	    {
	      Part part( part_name, part_table_name, wart_dlg_name, atom_name );

	      if( part.table.empty() )
		{
		  std::cout << format( "part_table_name empty defaulting to table_name (%s)\n",
				       table_name );
		  part.table = table_name;
		}

	      part_names.push_back( part );
	    }

	  for( unsigned i = 0; i < part_names.size(); i++ )
	    {
	      lLiType = elPart2LM2_TYPE(hEh->ehList, elNbody, TO_CHAR( part_names[i].name ) );

	      printf( "lType: %ld == %ld\n", lType , lLiType ); 

	      if (lType == lLiType) 
		{
		  // entry_fes(GetRootShell (), &tRecord[0]);
		  std::cout << format( "start wart_menu: %s\n", part_names[i].wart_dlg_name );

		  do_start_wart_menu( GetRootShell(), &tRecord[0], 
				      part_names[i].table, 
				      part_names[i].wart_dlg_name, 
				      part_names[i].atom_name );
		  break;
		}
	    }

	  if( auto_free_part_record )
	    {
	      std::cout << format( "freeing Record: 0x%X\n", (void*)tRecord );
	      free (tRecord);
	    }

	  return SEL_RETURN_NOACT;

	case SEL_REASON_PROCESS:
		return 0;

	case SEL_REASON_END:
		return 0;

	case SEL_REASON_PRINT:
		return doPrint( hEh , iSel_reason, hW, ptSelData );

	default:
		return 0;
	}
	return 0;
}

void StandardList::destroy_my_self( ListTdialog *ptListDialog )
{
  free( ptListDialog );
  delete this;
}

int StandardList::start( OWidget parent, Value data, MskDialog *pmask_rl )
{
  user_data = data;

  std::vector<std::string> sl = split_simple( rc_files, "," );
  
  for( unsigned i = 0; i < sl.size(); i++ )
	{
	  int ret = OwrcLoadObject(ApNconfigFile,const_cast<char*>( sl[i].c_str() ) );

	  if( ret != 1 ) {
		std::cerr << "cannot load '" <<  sl[i] << std::endl;
	  }
	}


  	ListTdialog		*pListDialog;
	ListTXdialog	*pListXDialog;
	ListTaction		*pListAction;
	listSelItem		*pSelector;
	MskDialog		mask_rl;	

	if ( VaMultipleMalloc (	(int)sizeof(ListTdialog), (void**)&pListDialog,
				(int)sizeof(ListTXdialog),(void**)&pListXDialog,
				(int)sizeof(ListTaction), (void**)&pListAction,
				(int)getSelectorSize(),	  (void**)&pSelector,
				(int)-1 ) == (void*) NULL )
	  {
		return -1;
	  }

	// fillSelector( p_selector );
	plistdialog = pListDialog;

	memset(pListDialog, 0, sizeof(ListTdialog));
	pListDialog ->ldSelector			= pSelector;
	pListDialog->ldHsl				= HSL_NI;
	pListDialog->ldAction			= pListAction;
/*
	pListDialog->ldGenCallback		= lgGenSqlList;
*/
	pListDialog->ldGenCallback		= my_gen_callback;
	pListDialog->ldTitle			= TO_CHAR( title );
	pListDialog->ldGenCalldata		= pListXDialog;
	pListDialog->ldDestroyCallback	= my_destroy_callback;
	pListDialog->ldPMaskRl			= pmask_rl;
	pListDialog->ldSelMask			= TO_CHAR( sel_mask );
	pListDialog->ldUserData         = this;
	pListDialog->ldSelectPrinter    = getPrinterFunc();
	pListDialog->ldPrintButResValTable = mgPrnButResValTable;


	memcpy(pSelector, getSelector(), getSelectorSize());
	
	memset(pListXDialog, 0, sizeof(ListTXdialog));
	pListXDialog->list				= pListDialog;
	pListXDialog->cb_genSqlList		= my_cb_sqlList;
	pListXDialog->bd 				= NULL;
	pListXDialog->listLines 	    = list_lines;

	memset(pListAction, 0, sizeof(ListTaction));
	
	pListAction->sel_callback		= my_sel_callback;
	pListAction->sel_calldata       = this;

	pListAction->sel_nookbutton     = 1;

	
	if (listCreateSelDup(pListDialog) < 0) {
	  free(pListDialog);
	  return 0;
	}

	fillSelector( pListDialog->ldSelDup );
	// fillSelector( pSelector );

	mask_rl = listSelMaskOpen (pListDialog, NULL);
	if (mask_rl == (MskDialog )NULL) {
		if (pmask_rl) {
			*pmask_rl = (MskDialog )NULL;
		}
		free (pListDialog);
		return 0 ;
	}

	pListXDialog->mask_rl = mask_rl;	   

	std::string lqname = getLqMaskName();

	if( !lqname.empty() )
	  {
		pListDialog->ldSelDup->lsiSort.lsLQmask  |= listGetLQmask(plistdialog,(char*)lqname.c_str());
		//pListDialog->ldQuery.liqSort.lsLQmask = listGetLQmask(pListDialog, (char *)lqname.c_str() );
		if( pListDialog->ldQuery.liqSort.lsLQmask == 0 )
		  {
			std::cout << "lqName " << lqname << " not found\n" << std::endl;
		  }
	  }

	MskUpdateMaskVar(mask_rl);

	return listSelMask(pListDialog, parent);
}

void StandardList::fillSelector( listSelItem *selector )
{
  std::cout << "fillSelector not implemented\n";
}


std::string StandardList::getLqMaskName()
{
  return std::string();
}

unsigned long StandardList::getSelectorSize()
{
  
  if( !p_selector )
	GetSelector( p_selector, selector_size );
  
  return selector_size;
}

listSelItem* StandardList::getSelector()
{
  if( !p_selector )
	GetSelector( p_selector, selector_size );
  
  return p_selector;
}

bool StandardList::do_start_wart_menu( OWidget w, void *record, 
				       const std::string & TableName, 
				       const std::string & DlgName, 
				       const std::string & AtomName  )
{
  return true;
}


#if TOOLS_VERSION < 37
int StandardList::sql_callback( lgRecords *sql, ListTdialog *list, int reason, char *stmt, void *ud )
#else
int StandardList::sql_callback( lgRecords *sql, ListTdialog *list, int reason, const char *stmt, void *ud )
#endif
{
  if( !sql_stmt_printed )
	{
	  std::cerr << "=> GO stmt:\n" << sql->SqlStmt << std::endl;
  	  sql_stmt_printed = true;
	}

  return cb_makeListFoot( sql, list, reason, stmt, ud );
}

/* sis: Default Generator Callback */
int StandardList::gen_callback(void *cd, int reason)
{
   return lgGenSqlList(cd, reason);
}

/* sis:
 * Replace-Funktion
 */
void StandardList::_uwc(char *s)
{
    char *p;

    if (s&&*s) {
        do {
            p=strchr(s,'*');
            if (p) {
                *p  = '%';
                memmove(p+1,p,strlen(p)+1);
                *p  = '%';
                s=p+2;
            } else {
                p=strchr(s,'?');
                if (p)
                    *p  = '_';
            }
        } while (p);
    }
}


/* sis: Diverse Tool-Funktions */
char* StandardList::AddStrF(LQ_DESC *pLq, char *sqlfilter, bool add_and_before )
{
	static char buffer[4096+1];

	memset(buffer,0,sizeof(buffer));

	const char *prefix = " ";

	if( add_and_before )
	  prefix = " and ";

	if (pLq->lq_val[0].str[0] &&
		pLq->lq_val[1].str[0])
	{
		/* Between */
		sprintf(buffer," %s %s between '%s' and '%s' ",
				prefix,
				sqlfilter, 
				pLq->lq_val[0].str,
				pLq->lq_val[1].str);
	} else if (pLq->lq_val[1].str[0])
	{
		sprintf(buffer," %s %s <= '%s' ",
				prefix,
				sqlfilter,
				pLq->lq_val[1].str);
	} else if (pLq->lq_val[0].str[0])
	{
		sprintf(buffer," %s %s like '%s' ",
				prefix,
				sqlfilter,
				pLq->lq_val[0].str);
	}
	/* Leer stürzt ab ... */
	if (buffer[0]=='\0')
	    strcpy(buffer," ");

	/* SQl-Wildcard's umwandeln ... */
	_uwc(buffer);

	return buffer;
}

/* sis: Diverse Tool-Funktions */
char* StandardList::AddTimeF(LQ_DESC *pLq, char *StmtPart)
{
	static char buffer[4096+1];
	struct tm   ptm;
    struct tm   *ptmVon;
    struct tm   *ptmBis;
    time_t      timeVon;
    time_t      timeBis;
    char        pTimeVon[50];
    char        pTimeBis[50];

	memset(buffer,0,sizeof(buffer));
            timeVon = *(pLq->lq_val[0].intnum);
            timeBis = *(pLq->lq_val[1].intnum);
            if (timeVon != 0)
            {
                if (timeBis != 0)
                {
                    ptmVon = (struct tm*)localtime ( &timeVon );
                    /* sonst ptmVon = ptmBis (Adresse)... */
                    ptm = *ptmVon;
                    ptmBis = (struct tm*)localtime ( &timeBis );
                    strftime ( pTimeVon, sizeof ( pTimeVon ),
                                "%d-%m-%Y %H.%M.%S", &ptm );
                    strftime ( pTimeBis, sizeof ( pTimeBis ),
                                "%d-%m-%Y %H.%M.%S", ptmBis );

                    sprintf(buffer,
						" %s>=TO_DATE('%s','DD-MM-YYYY HH24.MI.SS') AND"
                        " %s<=TO_DATE('%s','DD-MM-YYYY HH24.MI.SS')",
                        StmtPart, pTimeVon, StmtPart, pTimeBis);
                }
                else
                {
                    ptmVon = (struct tm*)localtime ( &timeVon );
                    strftime ( pTimeVon, sizeof ( pTimeVon ),
                                "%d-%m-%Y %H.%M.%S", ptmVon );
                    sprintf(buffer,
						" %s=TO_DATE('%s','DD-MM-YYYY HH24.MI.SS')",
                                StmtPart, pTimeVon);
                }
            }
            else
            {
                if( timeBis != 0)
                {
                    ptmBis = (struct tm*)localtime ( &timeBis );
                    strftime ( pTimeBis, sizeof ( pTimeBis ),
                                "%d-%m-%Y %H.%M.%S", ptmBis );
                    sprintf(buffer," %s<=TO_DATE('%s','DD-MM-YYYY HH24.MI.SS')",
                                StmtPart, pTimeBis);
                }
            }

	/* Leer stürzt ab ... */
	if (buffer[0]=='\0')
	    strcpy(buffer," ");

	return buffer;
}


ListSelectionCallback::~ListSelectionCallback()
{

}
