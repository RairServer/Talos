/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *                RESOLUTION                              * */
/* *                                                        * */
/* *  $Module:   RESOLUTION                                 * */ 
/* *                                                        * */
/* *  Copyright (C) 1996, 1997, 1998, 2001                  * */
/* *  MPI fuer Informatik                                   * */
/* *                                                        * */
/* *  This program is free software; you can redistribute   * */
/* *  it and/or modify it under the terms of the FreeBSD    * */
/* *  Licence.                                              * */
/* *                                                        * */
/* *  This program is distributed in the hope that it will  * */
/* *  be useful, but WITHOUT ANY WARRANTY; without even     * */
/* *  the implied warranty of MERCHANTABILITY or FITNESS    * */
/* *  FOR A PARTICULAR PURPOSE.  See the LICENCE file       * */
/* *  for more details.                                     * */
/* *                                                        * */
/* *                                                        * */
/* $Revision: 1.2 $                                     * */
/* $State: Exp $                                            * */
/* $Date: 2010-02-22 14:09:59 $                             * */
/* $Author: weidenb $                                         * */
/* *                                                        * */
/* *             Contact:                                   * */
/* *             Christoph Weidenbach                       * */
/* *             MPI fuer Informatik                        * */
/* *             Stuhlsatzenhausweg 85                      * */
/* *             66123 Saarbruecken                         * */
/* *             Email: spass@mpi-inf.mpg.de                * */
/* *             Germany                                    * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/* $RCSfile: resolution.c,v $ */

#include "resolution.h"


/**************************************************************/
/* Functions                                                  */
/**************************************************************/


void res_InsertClauseIndex(CLAUSE clause, st_INDEX stindex)
/**********************************************************
  INPUT:   A st_INDEX and a clause.
  RETURNS: Inserts the clause in the st_INDEX stindex.
  CAUTION: None.
***********************************************************/
{
  int n,j;

  n = clause_Length(clause);
  for (j = 0; j < n; j++)
    st_EntryCreate(stindex,
		   clause_GetLiteral(clause,j),
		   clause_GetLiteralTerm(clause,j),
		   cont_LeftContext());
}


void res_DeleteClauseIndex(CLAUSE clause, st_INDEX stindex)
/**********************************************************
  INPUT:   A st_INDEX and a clause.
  RETURNS: Deletes the clause from the st_INDEX stindex.
  CAUTION: None.
***********************************************************/
{
  int n, j;

  n = clause_Length(clause);
  for (j = 0; j < n; j++)
    if (!st_EntryDelete(stindex,
			clause_GetLiteral(clause,j), 
			clause_GetLiteralTerm(clause,j),
			cont_LeftContext()))
      misc_DumpCore();
}



CLAUSE res_SelectLightestClause(LIST clauselist)
/**********************************************************
  INPUT:   A list of clauses.
  RETURNS: The lightest clause of the clauselist. 
  CAUTION: None.
***********************************************************/
{
  CLAUSE clause;
  LIST   scan;
  int    min;

  clause = list_Car(clauselist);
  min    = clause_Weight(clause);
  
  for (scan=list_Cdr(clauselist); !list_Empty(scan); scan=list_Cdr(scan)) {
    if (clause_Weight(list_Car(scan)) < min) {
      clause = list_Car(scan);
      min    = clause_Weight(clause);
    }
  }
  return clause;
}


BOOL res_HasTautology(CLAUSE clause)
/**********************************************************
  INPUT:   A clauses.
  RETURNS: TRUE if the clause contains a complementary 
           literal pair and FALSE otherwise. 
  CAUTION: None.
***********************************************************/
{
  BOOL found;
  TERM literal1;
  int  i, j, n;

  found = FALSE;
  n     = clause_Length(clause);
  
  for (i = 0; i < n && !found; i++) {
    literal1 = fol_ComplementaryTerm(clause_GetLiteralTerm(clause,i));
    for (j = 0; j < n && !found; j++)
      if (j != i && term_Equal(literal1, clause_GetLiteralTerm(clause,j)))
	found = TRUE;

    term_Delete(literal1);
  }
  return found;
}



BOOL res_BackSubWithLength(CLAUSE clause, st_INDEX stindex)
/**********************************************************
  INPUT:   A clauses and an index.
  RETURNS: TRUE if a clause of the index subsumes the clause clause
           and length(clause) >= length(clause of index).
  CAUTION: None.
***********************************************************/
{
  int     n,i; 
  LIST    scan,generals;
  TERM    term;
  LITERAL litres;

  n = clause_Length(clause);
  for (i = 0; i < n; i++) {
    term      = clause_GetLiteralTerm(clause,i);
    generals  = st_GetGen(cont_LeftContext(), stindex, term);
    for (scan = generals; !list_Empty(scan); scan = list_Cdr(scan)) {
      litres = (LITERAL) list_Car(scan);
      if (litres == clause_GetLiteral(clause_LiteralOwningClause(litres),0) &&
	  clause_Length(clause) >= clause_Length(clause_LiteralOwningClause(litres)) &&
	  clause_Weight(clause) >= clause_Weight(clause_LiteralOwningClause(litres)) &&
	  subs_Idc(clause_LiteralOwningClause(litres),clause)) {
	    list_Delete(generals);
	    return TRUE;
      }
    }
    list_Delete(generals);
  }
  return FALSE;
}


