CREATE TABLE t_log_unique (
    access_date        DATE,
    access_total       int,
    access_unique      int,
    reg_date           DATE,
    reg_mach           varchar(64)
);

CREATE TABLE t_log_domain (
    access_date        DATE,
    access_domain      varchar(64),
    access_count       int,
    reg_date           DATE,
    reg_mach           varchar(64)
);
