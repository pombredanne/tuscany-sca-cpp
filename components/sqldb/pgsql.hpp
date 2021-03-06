/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/* $Rev$ $Date$ */

#ifndef tuscany_pgsql_hpp
#define tuscany_pgsql_hpp

/**
 * PostgreSQL access functions.
 */

#include <libpq-fe.h>

#include "string.hpp"
#include "list.hpp"
#include "value.hpp"
#include "monad.hpp"
#include "../../modules/scheme/eval.hpp"

namespace tuscany {
namespace pgsql {

/**
 * Return and clear a Postgres result failure.
 */
const string pgfailure(PGresult* r, PGconn* conn) {
    const string re = PQresultErrorMessage(r);
    PQclear(r);
    if (length(re) != 0)
        return re;
    const string ce = PQerrorMessage(conn);
    return ce;
}

/**
 * Represents a PGSql connection.
 */
class PGSql {
public:
    PGSql() : owner(false) {
        debug("pgsql::pgsql");
    }

    PGSql(const string& conninfo, const string& table) : owner(true), conn(NULL), conninfo(conninfo), table(table) {
        debug(conninfo, "pgsql::pgsql::conninfo");
        debug(table, "pgsql::pgsql::table");
        conn = PQconnectdb(c_str(conninfo));
        if (PQstatus(conn) != CONNECTION_OK) {
            mkfailure<bool>(string("Could not connect to postgresql database: ") + PQerrorMessage(conn));
            return;
        }
        setup(true);
    }

    PGSql(const PGSql& c) : owner(false), conn(c.conn), conninfo(c.conninfo), table(c.table) {
        debug("pgsql::pgsql::copy");
    }

    ~PGSql() {
        debug("pgsql::~pgsql");
        if (!owner)
            return;
        if (conn == NULL)
            return;
        PQfinish(conn);
    }

private:
    bool owner;
    PGconn *conn;
    string conninfo;
    string table;

    friend const failable<bool> setup(const PGSql& pgsql);
    friend const failable<bool> post(const value& key, const value& val, const PGSql& pgsql);
    friend const failable<bool> put(const value& key, const value& val, const PGSql& pgsql);
    friend const failable<value> get(const value& key, const PGSql& pgsql);
    friend const failable<bool> del(const value& key, const PGSql& pgsql);

    /**
     * Setup the database connection.
     */
    const failable<bool> setup(const bool init) const {

        // Check the status of the connection and reconnect if necessary
        if (!init) {
            if (PQstatus(conn) == CONNECTION_OK)
                return true;
            debug("pgsql::setup::reset");
            PQreset(conn);
            if (PQstatus(conn) != CONNECTION_OK)
                return mkfailure<bool>(string("Could not reconnect to postgresql database: ") + PQerrorMessage(conn));
        }
        debug("pgsql::setup::init");

        // Find the name of the first column in the target table
        // Assume that's the key we need to use
        string ks = string("select a.attname from pg_attribute a, pg_class c where a.attrelid = c.relfilenode and c.relname = '") + table + string("' and a.attnum in (1, 2) order by a.attnum;");
        PGresult* kr = PQexec(conn, c_str(ks));
        if (PQresultStatus(kr) != PGRES_TUPLES_OK)
            return mkfailure<bool>(string("Could not execute postgresql column select statement: ") + pgfailure(kr, conn));
        if (PQntuples(kr) != 2) {
            PQclear(kr);
            return mkfailure<bool>(string("Could not find postgresql table key and value column names"));
        }
        const string kname = PQgetvalue(kr, 0, 0);
        const string vname = PQgetvalue(kr, 1, 0);
        PQclear(kr);

        // Prepare the post, put, get and delete statements
        {
            PGresult* r = PQprepare(conn, "post", c_str(string("insert into ") + table + string(" values($1, $2);")), 2, NULL);
            if (PQresultStatus(r) != PGRES_COMMAND_OK)
                return mkfailure<bool>(string("Could not prepare post postgresql SQL statement: ") + pgfailure(r, conn));
            PQclear(r);
        }
        {
            PGresult* r = PQprepare(conn, "put", c_str(string("update ") + table + string(" set ") + vname + string(" = $2 where ") + kname + string(" = $1;")), 2, NULL);
            if (PQresultStatus(r) != PGRES_COMMAND_OK)
                return mkfailure<bool>(string("Could not prepare put postgresql SQL statement: ") + pgfailure(r, conn));
            PQclear(r);
        }
        {
            PGresult* r = PQprepare(conn, "get", c_str(string("select * from ") + table + string(" where ") + kname + string(" = $1;")), 1, NULL);
            if (PQresultStatus(r) != PGRES_COMMAND_OK)
                return mkfailure<bool>(string("Could not prepare get postgresql SQL statement: ") + pgfailure(r, conn));
            PQclear(r);
        }
        {
            PGresult* r = PQprepare(conn, "delete", c_str(string("delete from ") + table + string(" where ") + kname + string(" = $1;")), 1, NULL);
            if (PQresultStatus(r) != PGRES_COMMAND_OK)
                return mkfailure<bool>(string("Could not prepare delete postgresql SQL statement: ") + pgfailure(r, conn));
            PQclear(r);
        }
        return true;
    }
};

/**
 * Setup the database connection if necessary.
 */
const failable<bool> setup(const PGSql& pgsql) {
    return pgsql.setup(false);
}

/**
 * Post a new item to the database.
 */
const failable<bool> post(const value& key, const value& val, const PGSql& pgsql) {
    debug(key, "pgsql::post::key");
    debug(val, "pgsql::post::value");
    debug(pgsql.conninfo, "pgsql::post::conninfo");
    debug(pgsql.table, "pgsql::post::table");
    setup(pgsql);

    const string ks(scheme::writeValue(key));
    const string vs(scheme::writeValue(val));
    const char* params[2] = { c_str(ks), c_str(vs) };
    PGresult* r = PQexecPrepared(pgsql.conn, "post", 2, params, NULL, NULL, 0);
    if (PQresultStatus(r) != PGRES_COMMAND_OK)
        return mkfailure<bool>(string("Could not execute insert postgresql SQL statement: ") + pgfailure(r, pgsql.conn));
    PQclear(r);

    debug(true, "pgsql::post::result");
    return true;
}

/**
 * Update an item in the database. If the item doesn't exist it is added.
 */
const failable<bool> put(const value& key, const value& val, const PGSql& pgsql) {
    debug(key, "pgsql::put::key");
    debug(val, "pgsql::put::value");
    debug(pgsql.conninfo, "pgsql::put::conninfo");
    debug(pgsql.table, "pgsql::put::table");
    setup(pgsql);

    const string ks(scheme::writeValue(key));
    const string vs(scheme::writeValue(val));
    const char* params[2] = { c_str(ks), c_str(vs) };
    PGresult* r = PQexecPrepared(pgsql.conn, "put", 2, params, NULL, NULL, 0);
    if (PQresultStatus(r) != PGRES_COMMAND_OK)
        return mkfailure<bool>(string("Could not execute update postgresql SQL statement: ") + pgfailure(r, pgsql.conn));
    const string t = PQcmdTuples(r);
    if (t != "0") {
        PQclear(r);
        debug(true, "pgsql::put::result");
        return true;
    }
    PQclear(r);

    PGresult* pr = PQexecPrepared(pgsql.conn, "post", 2, params, NULL, NULL, 0);
    if (PQresultStatus(pr) != PGRES_COMMAND_OK)
        return mkfailure<bool>(string("Could not execute insert postgresql SQL statement: ") + pgfailure(pr, pgsql.conn));
    PQclear(pr);

    debug(true, "pgsql::put::result");
    return true;
}

/**
 * Get an item from the database.
 */
const failable<value> get(const value& key, const PGSql& pgsql) {
    debug(key, "pgsql::get::key");
    debug(pgsql.conninfo, "pgsql::get::conninfo");
    debug(pgsql.table, "pgsql::get::table");
    setup(pgsql);

    const string ks(scheme::writeValue(key));
    const char* params[1] = { c_str(ks) };
    PGresult* r = PQexecPrepared(pgsql.conn, "get", 1, params, NULL, NULL, 0);
    if (PQresultStatus(r) != PGRES_TUPLES_OK)
        return mkfailure<value>(string("Could not execute select postgresql SQL statement: ") + pgfailure(r, pgsql.conn));
    if (PQntuples(r) < 1) {
        PQclear(r);
        return mkfailure<value>(string("Could not get postgresql entry: ") + PQerrorMessage(pgsql.conn));
    }
    const char* data = PQgetvalue(r, 0, 1);
    const value val(scheme::readValue(string(data)));
    PQclear(r);

    debug(val, "pgsql::get::result");
    return val;
}

/**
 * Delete an item from the database
 */
const failable<bool> del(const value& key, const PGSql& pgsql) {
    debug(key, "pgsql::delete::key");
    debug(pgsql.conninfo, "pgsql::delete::conninfo");
    debug(pgsql.table, "pgsql::delete::table");
    setup(pgsql);

    const string ks(scheme::writeValue(key));
    const char* params[1] = { c_str(ks) };
    PGresult* r = PQexecPrepared(pgsql.conn, "delete", 1, params, NULL, NULL, 0);
    if (PQresultStatus(r) != PGRES_COMMAND_OK)
        return mkfailure<bool>(string("Could not execute delete postgresql SQL statement: ") + pgfailure(r, pgsql.conn));
    PQclear(r);

    debug(true, "pgsql::delete::result");
    return true;
}

}
}

#endif /* tuscany_pgsql_hpp */
