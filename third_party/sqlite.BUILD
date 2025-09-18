load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "sqlite3",
    srcs = [
        "sqlite3.c",
    ],
    hdrs = [
        "sqlite3.h",
        "sqlite3ext.h",
    ],
    copts = [
        "-DSQLITE_THREADSAFE=1",
        "-DSQLITE_ENABLE_FTS3",
        "-DSQLITE_ENABLE_FTS4",
        "-DSQLITE_ENABLE_FTS5",
        "-DSQLITE_ENABLE_JSON1",
        "-DSQLITE_ENABLE_RTREE",
        "-DSQLITE_ENABLE_EXPLAIN_COMMENTS",
        "-DSQLITE_ENABLE_DBPAGE_VTAB",
        "-DSQLITE_ENABLE_STMTVTAB",
        "-DSQLITE_ENABLE_DBSTAT_VTAB",
    ],
    visibility = ["//visibility:public"],
)