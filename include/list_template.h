/******************************************************************************
 * This file is part of 3D-ICE, version 2.1 .                                 *
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

#ifndef ListType
#error missing macro ListType
#endif

#ifndef ListName
#error missing macro ListName
#endif

#define PASTER(x,y) x ## y
#define CNT(x,y)    PASTER(x,y)

#define DEF_LIST_NODE(nodetype, datatype)                                      \
                                                                               \
    struct nodetype                                                            \
    {                                                                          \
        datatype Data ;                                                        \
                                                                               \
        struct nodetype *Prev ;                                                \
                                                                               \
        struct nodetype *Next ;                                                \
    } ;                                                                        \
                                                                               \
    typedef struct nodetype nodetype ;


#define DEF_LIST(listtype, nodetype)                                           \
                                                                               \
    struct listtype                                                            \
    {                                                                          \
        Quantity_t Size ;                                                      \
                                                                               \
        nodetype *First ;                                                      \
                                                                               \
        nodetype *Last ;                                                       \
    } ;                                                                        \
                                                                               \
    typedef struct listtype listtype ;



#define FPROTO1(return, fid, param)                                            \
                                                                               \
    return fid (param)

#define FPROTO2(return, fid, param1, param2)                                   \
                                                                               \
    return fid (param1, param2)

#define FPROTO3(return, fid, param1, param2, param3)                           \
                                                                               \
    return fid (param1, param2, param3)

#define FIMP(prototype, body) prototype { body }



#define TTT_t         CNT(ListType, _t)
#define TTTList_t     CNT(ListType, List_t)
#define TTTListNode_t CNT(ListType, ListNode_t)

#define TTT_list_init         CNT(ListName, _list_init)
#define TTT_list_destroy      CNT(ListName, _list_destroy)
#define TTT_list_copy         CNT(ListName, _list_copy)
#define TTT_list_begin        CNT(ListName, _list_begin)
#define TTT_list_end          CNT(ListName, _list_end)
#define TTT_list_next         CNT(ListName, _list_next)
#define TTT_list_prev         CNT(ListName, _list_prev)
#define TTT_list_data         CNT(ListName, _list_data)
#define TTT_list_insert_begin CNT(ListName, _list_insert_begin)
#define TTT_list_insert_end   CNT(ListName, _list_insert_end)
#define TTT_list_find         CNT(ListName, _list_find)
#define TTT_list_print        CNT(ListName, _list_print)

/******************************************************************************/

DEF_LIST_NODE (TTTListNode_t, TTT_t)

DEF_LIST (TTTList_t, TTTListNode_t)

/******************************************************************************/


FPROTO1 (

    void, TTT_list_init, TTTList_t *list) ;



FPROTO1 (

    void, TTT_list_destroy, TTTList_t *list) ;




FPROTO2 (

    void, TTT_list_copy, TTTList_t *dst, TTTList_t *src) ;



FPROTO1 (

    TTTListNode_t *, TTT_list_begin, TTTList_t *list) ;



FPROTO1 (

    TTTListNode_t *, TTT_list_end, TTTList_t *list) ;



FPROTO1 (

    TTTListNode_t *, TTT_list_next, TTTListNode_t *node) ;



FPROTO1 (

    TTTListNode_t *, TTT_list_prev, TTTListNode_t *node) ;



FPROTO1 (

    TTT_t *, TTT_list_data, TTTListNode_t *node) ;



FPROTO2 (

    void, TTT_list_insert_begin, TTTList_t *list, TTT_t *data) ;



FPROTO2 (

    void, TTT_list_insert_end, TTTList_t *list, TTT_t *data) ;



FPROTO2 (

    TTT_t *, TTT_list_find, TTTList_t *list, TTT_t *data) ;



FPROTO3 (

    void, TTT_list_print, TTTList_t *list, FILE *stream, String_t prefix) ;



/******************************************************************************/
