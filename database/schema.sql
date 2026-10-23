PRAGMA foreign_keys = ON;
PRAGMA journal_mode = WAL;
PRAGMA busy_timeout = 5000;

BEGIN;

CREATE TABLE IF NOT EXISTS price_days (
    delivery_date TEXT PRIMARY KEY NOT NULL,
    complete      INTEGER NOT NULL DEFAULT 0 CHECK (complete IN (0, 1)),
    updated_at    TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS hourly_prices (
    delivery_date TEXT NOT NULL,
    time_period   TEXT NOT NULL,
    price         REAL,

    PRIMARY KEY (delivery_date, time_period),
    FOREIGN KEY (delivery_date)
        REFERENCES price_days(delivery_date)
        ON DELETE CASCADE
);

PRAGMA user_version = 1;

COMMIT;
