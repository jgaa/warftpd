DROP TABLE IF EXISTS "user";
DROP TABLE IF EXISTS "permission";
DROP TABLE IF EXISTS "path";
DROP TABLE IF EXISTS "interface";
DROP TABLE IF EXISTS "protocol";
DROP TABLE IF EXISTS "host";
DROP TABLE IF EXISTS "server";

CREATE TABLE "server" (
    "id"   VARCHAR(38) NOT NULL PRIMARY KEY,
    "name"  VARCHAR(255) NOT NULL,
    "enabled" BOOLEAN NOT NULL DEFAULT(1)
);

CREATE TABLE "host" (
    "id"   VARCHAR(38) NOT NULL PRIMARY KEY,
    "parent"VARCHAR(38) NOT NULL,
    "name"  VARCHAR(255) NOT NULL,
    "long_name" VARCHAR(255) NOT NULL,
    "enabled" BOOLEAN NOT NULL DEFAULT(1)
);

CREATE TABLE "protocol" (
    "id"   VARCHAR(38) NOT NULL PRIMARY KEY,
    "parent"VARCHAR(38) NOT NULL,
    "name"  VARCHAR(255) NOT NULL,
    "enabled" BOOLEAN NOT NULL DEFAULT(1)
);

CREATE TABLE "interface" (
    "id"   VARCHAR(38) NOT NULL PRIMARY KEY,
    "parent"VARCHAR(38) NOT NULL,
    "name"  VARCHAR(255) NOT NULL,
    "enabled" BOOLEAN NOT NULL DEFAULT(1),
    "hostname"  VARCHAR(255) NOT NULL,
    "port"  VARCHAR(16) NOT NULL
);

CREATE TABLE "permission" (
    "id"   VARCHAR(38) NOT NULL PRIMARY KEY,
    "type" INTEGER NOT NULL,
    "parent" VARCHAR(38) NOT NULL
);

CREATE TABLE "path" (
    "id"   VARCHAR(38) NOT NULL PRIMARY KEY,
    "permission_id"VARCHAR(38),
    "vpath" VARCHAR(255) NOT NULL,
    "ppath" VARCHAR(255) NOT NULL,
    "permissions" UNSIGNED BIG INT NOT NULL
);

CREATE TABLE "user" (
    "id"   VARCHAR(38) NOT NULL PRIMARY KEY,
    "parent" VARCHAR(38) NOT NULL,
    "login_name"  VARCHAR(64) NOT NULL,
    "full_name"  VARCHAR(255) NOT NULL,
    "auth_type" INTEGER NOT NULL DEFAULT(0), 
    "passwd" VARCHAR(255) NOT NULL,
    "enabled" BOOLEAN NOT NULL DEFAULT(1)
);

CREATE UNIQUE INDEX "ix_svr_name" on server (name ASC);
CREATE UNIQUE INDEX "ix_user_login" on user (parent, login_name);

