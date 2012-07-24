/******************************************************************************
 * This file is part of 3D-ICE, version 2.2 .                                 *
 *                                                                            *
 * 3D-ICE is free software: you can  redistribute it and/or  modify it  under *
 * the terms of the  GNU General  Public  License as  published by  the  Free *
 * Software  Foundation, either  version  3  of  the License,  or  any  later *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICE is  distributed  in the hope  that it will  be useful, but  WITHOUT *
 * ANY  WARRANTY; without  even the  implied warranty  of MERCHANTABILITY  or *
 * FITNESS  FOR A PARTICULAR  PURPOSE. See the GNU General Public License for *
 * more details.                                                              *
 *                                                                            *
 * You should have  received a copy of  the GNU General  Public License along *
 * with 3D-ICE. If not, see <http://www.gnu.org/licenses/>.                   *
 *                                                                            *
 *                             Copyright (C) 2010                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Giseong Bak                                                       *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#ifndef node_data_init
#error missing macro node_data_init
#endif

#ifndef node_data_destroy
#error missing macro node_data_destroy
#endif

#ifndef node_data_copy
#error missing macro node_data_copy
#endif

#ifndef node_data_equal
#error missing macro node_data_equal
#endif

#ifndef node_data_print
#error missing macro node_data_print
#endif

/******************************************************************************/

FIMP (FPROTO1 (

void, TTT_list_init, TTTList_t *list),

    list->Size  = (Quantity_t) 0u ;
    list->First = NULL ;
    list->Last  = NULL ;
)

/******************************************************************************/

FIMP (FPROTO2 (

void, TTT_list_copy, TTTList_t *dst, TTTList_t *src),

    TTT_list_destroy (dst) ;

    TTTListNode_t *node ;

    for (node  = TTT_list_begin (src) ;
         node != NULL ;
         node  = TTT_list_next (node))

        TTT_list_insert_end (dst, TTT_list_data(node)) ;
)

/******************************************************************************/

FIMP (FPROTO1 (

void, TTT_list_destroy, TTTList_t *list),

    TTTListNode_t* next = NULL ;

    while (list->First != NULL)
    {
        next = TTT_list_next (list->First) ;

        node_data_destroy (TTT_list_data (list->First)) ;

        free (list->First) ;

        list->First = next ;
    }

    TTT_list_init (list) ;
)

/******************************************************************************/

FIMP( FPROTO1 (

TTTListNode_t *, TTT_list_begin, TTTList_t *list),

    return list->First ;
)

/******************************************************************************/

FIMP( FPROTO1 (

TTTListNode_t *, TTT_list_end, TTTList_t *list),

    return list->Last ;
)

/******************************************************************************/

FIMP( FPROTO1 (

TTTListNode_t *, TTT_list_next, TTTListNode_t *node),

    return node->Next ;
)

/******************************************************************************/

FIMP( FPROTO1 (

TTTListNode_t *, TTT_list_prev, TTTListNode_t *node),

    return node->Prev ;
)

/******************************************************************************/

FIMP( FPROTO1 (

TTT_t *, TTT_list_data, TTTListNode_t *node),

    return &node->Data ;
)

/******************************************************************************/

FIMP( FPROTO2 (

void, TTT_list_insert_begin, TTTList_t *list, TTT_t *data),

    TTTListNode_t *newnode = (TTTListNode_t *) malloc (sizeof(TTTListNode_t)) ;

    if (newnode == NULL)

        return ;

    node_data_init (TTT_list_data(newnode)) ;

    node_data_copy (TTT_list_data(newnode), data) ;

    newnode->Next = NULL ;
    newnode->Prev = NULL ;

    if (list->First == NULL && list->Last == NULL)
    {
        list->First = newnode ;
        list->Last  = newnode ;

        list->Size++ ;

        return ;
    }

    if (list->First == NULL || list->Last == NULL)
    {
        fprintf (stderr, "ERROR: bad list status\n") ;

        return ;
    }

    newnode->Next = list->First ;

    list->First->Prev = newnode ;

    list->First = newnode ;

    list->Size++ ;
)

/******************************************************************************/

FIMP( FPROTO2 (

void, TTT_list_insert_end, TTTList_t *list, TTT_t *data),

    TTTListNode_t *newnode = (TTTListNode_t *) malloc (sizeof(TTTListNode_t)) ;

    if (newnode == NULL)

        return ;

    node_data_init (TTT_list_data(newnode)) ;

    node_data_copy (TTT_list_data(newnode), data) ;

    newnode->Next = NULL ;
    newnode->Prev = NULL ;

    if (list->First == NULL && list->Last == NULL)
    {
        list->First = newnode ;
        list->Last  = newnode ;

        list->Size++ ;

        return ;
    }

    if (list->First == NULL && list->Last == NULL)
    {
        fprintf (stderr, "ERROR: bad list status\n") ;

        return ;
    }

    newnode->Prev = list->Last ;

    list->Last->Next = newnode ;

    list->Last = newnode ;

    list->Size++ ;
)

/******************************************************************************/

FIMP( FPROTO2 (

TTT_t *, TTT_list_find, TTTList_t *list, TTT_t *data),

    TTTListNode_t *node ;

    for (node  = TTT_list_begin(list) ;
         node != NULL ;
         node  = TTT_list_next (node))

        if (node_data_equal (TTT_list_data (node), data) == true)

            break ;

    if (node == NULL)

        return NULL ;

    return TTT_list_data (node) ;
)

/******************************************************************************/

FIMP( FPROTO3 (

void, TTT_list_print, TTTList_t *list, FILE *stream, String_t prefix),

    TTTListNode_t *node ;

    for (node  = TTT_list_begin(list) ;
         node != NULL ;
         node  = TTT_list_next (node))
    {
        node_data_print (TTT_list_data(node), stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }
)

/******************************************************************************/
