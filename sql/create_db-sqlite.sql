DROP TABLE IF EXISTS "user";
DROP TABLE IF EXISTS "permission";
DROP TABLE IF EXISTS "path";
DROP TABLE IF EXISTS "interface";
DROP TABLE IF EXISTS "protocol";
DROP TABLE IF EXISTS "host";
DROP TABLE IF EXISTS "server";

CREATE TABLE "server" (
    "id"    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "name"  VARCHAR(255) NOT NULL,
    "enabled" INTEGER NOT NULL DEFAULT(1)
);

CREATE TABLE "host" (
    "id"    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "server_id" INTEGER NOT NULL,
    "name"  VARCHAR(255) NOT NULL,
    "long_name" VARCHAR(255) NOT NULL,
    "enabled" INTEGER NOT NULL DEFAULT(1)
);

CREATE TABLE "protocol" (
    "id"    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "server_id" INTEGER NOT NULL,
    "host_id" INTEGER NOT NULL,
    "name"  VARCHAR(255) NOT NULL,
    "enabled" INTEGER NOT NULL DEFAULT(1)
);

CREATE TABLE "interface" (
    "id"    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "server_id" INTEGER NOT NULL,
    "host_id" INTEGER NOT NULL,
    "protocol_id" INTEGER NOT NULL,
    "name"  VARCHAR(255) NOT NULL,
    "enabled" INTEGER NOT NULL DEFAULT(1),
    "hostname"  VARCHAR(255) NOT NULL,
    "port"  VARCHAR(16) NOT NULL
);

CREATE TABLE "permission" (
    "id"    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "server_id" INTEGER,
    "host_id" INTEGER,
    "protocol_id" INTEGER,
    "user_id" INTEGER
);

CREATE TABLE "path" (
    "id"    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "permission_id" INTEGER,
    "vpath" VARCHAR(255) NOT NULL,
    "ppath" VARCHAR(255) NOT NULL,
    "permissions" UNSIGNED BIG INT NOT NULL
);

CREATE TABLE "user" (
    "id"    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "host_id" INTEGER NOT NULL,
    "login_name"  VARCHAR(64) NOT NULL,
    "full_name"  VARCHAR(255) NOT NULL,
    "auth_type" INTEGER NOT NULL DEFAULT(0), 
    "passwd" VARCHAR(255) NOT NULL
);

CREATE UNIQUE INDEX "ix_svr_name" on server (name ASC);
CREATE UNIQUE INDEX "ix_user_login" on user (host_id, login_name);

