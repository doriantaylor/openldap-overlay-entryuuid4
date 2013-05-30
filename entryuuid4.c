/* seqmod.c - sequenced modifies */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 2004-2013 The OpenLDAP Foundation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */
/* ACKNOWLEDGEMENTS: This work was initially adapted by Dorian Taylor
 * <http://doriantaylor.com/> for inclusion in OpenLDAP Software.
 */

#include "portable.h"

#if defined(SLAPD_OVER_ENTRYUUID4) && defined(HAVE_UUID_GENERATE)

#include "slap.h"
#include "config.h"

#include <uuid/uuid.h>

/* This overlay makes the entryUUID attribute a V4, rather than V1 UUID. */

static int
entryuuid4_hello_world(BackendDB *be)
{
    Debug(LDAP_DEBUG_TRACE, "entryuuid4: Alive and well!\n", 0, 0, 0);

    return 0;
}

static int
entryuuid4_add_attr(Operation *op, SlapReply *rs)
{
    Attribute *a;

    /* This is totally cribbed from servers/slapd/add.c */
  
    a = attr_find( op->ora_e->e_attrs,
                   slap_schema.si_ad_entryUUID );

    if (a) {
        Debug(LDAP_DEBUG_TRACE,
              "entryuuid4: entryUUID already exists in request to add %s\n",
              op->o_req_dn.bv_val, 0, 0);
    }
    else {
        struct berval tmp;
        uuid_t uu;
        char uuidbuf[40];

        uuid_generate_random(uu);
        uuid_unparse_lower(uu, (char *)uuidbuf);
    
        tmp.bv_len = strlen(uuidbuf);
        tmp.bv_val = uuidbuf;
        
        attr_merge_normalize_one(op->ora_e, slap_schema.si_ad_entryUUID,
                                 &tmp, op->o_tmpmemctx);

        Debug(LDAP_DEBUG_TRACE,
              "entryuuid4: Injected entryUUID %s when adding %s\n",
              uuidbuf, op->o_req_dn.bv_val, 0);
    }

    return SLAP_CB_CONTINUE;
}


/* This overlay is set up for dynamic loading via moduleload. For static
 * configuration, you'll need to arrange for the slap_overinst to be
 * initialized and registered by some other function inside slapd.
 */

static slap_overinst 		entryuuid4;

int
entryuuid4_initialize()
{
	entryuuid4.on_bi.bi_type    = "entryuuid4";
	entryuuid4.on_bi.bi_db_init = entryuuid4_hello_world;
    entryuuid4.on_bi.bi_op_add  = entryuuid4_add_attr;

	return overlay_register( &entryuuid4 );
}

#if SLAPD_OVER_ENTRYUUID4 == SLAPD_MOD_DYNAMIC
int
init_module( int argc, char *argv[] )
{
	return entryuuid4_initialize();
}
#endif /* SLAPD_OVER_ENTRYUUID4 == SLAPD_MOD_DYNAMIC */

#endif /* defined(SLAPD_OVER_ENTRYUUID4) && defined(HAVE_UUID_GENERATE) */
