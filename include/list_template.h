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

/*! \file list_template.h */

#ifndef ListType
#error missing macro ListType
#endif

#ifndef ListName
#error missing macro ListName
#endif

#define PASTER(x,y) x ## y
#define CNT(x,y)    PASTER(x,y)

#define LIST_NODE(nodetype, datatype)                                          \
                                                                               \
    /*! The node od a double-linked list */                                    \
                                                                               \
    struct nodetype                                                            \
    {                                                                          \
        /*! The item (data) stored by the node of the list */                  \
                                                                               \
        datatype Data ;                                                        \
                                                                               \
        /*! The poiner to the following node in the list */                    \
                                                                               \
        struct nodetype *Prev ;                                                \
                                                                               \
        /*! The poiner to the previous node in the list */                     \
                                                                               \
        struct nodetype *Next ;                                                \
    } ;                                                                        \
                                                                               \
    /*! Definition of the type nodetype */                                     \
                                                                               \
    typedef struct nodetype nodetype ;


#define LIST(listtype, nodetype)                                               \
                                                                               \
    /*! A double-linked list */                                                \
                                                                               \
    struct listtype                                                            \
    {                                                                          \
        /*! The number of nodes in the list */                                 \
                                                                               \
        Quantity_t Size ;                                                      \
                                                                               \
        /*! The poiner to the first node in the list */                        \
                                                                               \
        nodetype *First ;                                                      \
                                                                               \
        /*! The poiner to the last node in the list */                         \
                                                                               \
        nodetype *Last ;                                                       \
    } ;                                                                        \
                                                                               \
    /*! Definition of the type nodetype */                                     \
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

LIST_NODE (TTTListNode_t, TTT_t)

LIST (TTTList_t, TTTListNode_t)

/******************************************************************************/



    /*! Inits the fields of the \a list structure with default values
     *
     * \param list the address of the structure to initalize
     */

FPROTO1 (

    void, TTT_list_init, TTTList_t *list) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

FPROTO2 (

    void, TTT_list_copy, TTTList_t *dst, TTTList_t *src) ;



    /*! Destroys the content of the fields of the structure \a list
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a TTT_list_init .
     *
     * \param list the address of the structure to destroy
     */

FPROTO1 (

    void, TTT_list_destroy, TTTList_t *list) ;



    /*! Returns the first node in the list
     *
     * \param list the address of the list
     *
     * \return a pointer to the first node in the list
     * \return \c NULL if the list is empty
     */

FPROTO1 (

    TTTListNode_t *, TTT_list_begin, TTTList_t *list) ;



    /*! Returns the last node in the list
     *
     * \param list the address of the list
     *
     * \return a pointer to the last node in the list
     * \return \c NULL if the list is empty
     */

FPROTO1 (

    TTTListNode_t *, TTT_list_end, TTTList_t *list) ;



    /*! Returns the next node in the list
     *
     * \param node the pointer to the current position
     *
     * \return a pointer to the node in the list that follows \a node
     * \return \c NULL if the next node does not exist
     */

FPROTO1 (

    TTTListNode_t *, TTT_list_next, TTTListNode_t *node) ;



    /*! Returns the previous node in the list
     *
     * \param node the pointer to the current position
     *
     * \return a pointer to the node in the list that precede \a node
     * \return \c NULL if the previous node does not exist
     */

FPROTO1 (

    TTTListNode_t *, TTT_list_prev, TTTListNode_t *node) ;



    /*! Returns the data stored by a node in the list
     *
     * \param node the pointer to the node
     *
     * \return a pointer to the data stored in \a node
     */

FPROTO1 (

    TTT_t *, TTT_list_data, TTTListNode_t *node) ;



    /*! Inserts new data at the beginning of the list
     *
     * The function creates a new node to be added at the beginning of
     * the \a list. Then, it makes a local copy of \a data .
     *
     * \param list the address of the list
     * \param data the address of the data to insert
     */

FPROTO2 (

    void, TTT_list_insert_begin, TTTList_t *list, TTT_t *data) ;



    /*! Inserts new data at the end of the list
     *
     * The function creates a new node to be added at the end of
     * the \a list. Then, it makes a local copy of \a data .
     *
     * \param list the address of the list
     * \param data the address of the data to insert
     */

FPROTO2 (

    void, TTT_list_insert_end, TTTList_t *list, TTT_t *data) ;



    /*! Finds an elemnt in the list
     *
     * \param list the address of the list
     * \param data the address of the data to find
     *
     * \return a pointer to the data, if found
     * \return \c NULL if the list does not contain \a data
     */

FPROTO2 (

    TTT_t *, TTT_list_find, TTTList_t *list, TTT_t *data) ;



    /*! Prints the content of the list
     *
     * \param list the address of the list to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

FPROTO3 (

    void, TTT_list_print, TTTList_t *list, FILE *stream, String_t prefix) ;



/******************************************************************************/
