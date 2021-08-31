#ifndef SOURCE_ERR__H
#define SOURCE_ERR__H

typedef int t_err_t;

#define ERR_OK    0      /* No error, everything OK. */
#define ERR_MEM  -1      /* Out of memory error.     */
#define ERR_BUF  -2      /* Buffer error.            */


#define ERR_ABRT -3      /* Connection aborted.      */
#define ERR_RST  -4      /* Connection reset.        */
#define ERR_CLSD -5      /* Connection closed.       */
#define ERR_CONN -6      /* Not connected.           */

#define ERR_VAL  -7      /* Illegal value.           */

#define ERR_ARG  -8      /* Illegal argument.        */

#define ERR_RTE  -9      /* Routing problem.         */

#define ERR_USE  -10     /* Address in use.          */

#define ERR_IF   -11     /* Low-level netif error    */
#define ERR_ISCONN -12   /* Already connected.       */

#endif
