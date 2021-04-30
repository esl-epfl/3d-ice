
#ifndef _CONNECTION_LIST_H_
#define _CONNECTION_LIST_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include "types.h"
#include "connection.h"

#define ListType Connection
#define ListName connection

#include "list_template.h"

#undef ListType
#undef ListName

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif    /* _CONNECTION_LIST_H_ */
