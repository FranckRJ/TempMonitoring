CREATE TABLE IF NOT EXISTS "users"
(
    id       INTEGER PRIMARY KEY,
    name     TEXT NOT NULL,
    password TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS "rooms"
(
    id      INTEGER PRIMARY KEY,
    name    TEXT    NOT NULL,
    user_id INTEGER NOT NULL,
    FOREIGN KEY (user_id) REFERENCES "users" (id)
);

CREATE TABLE IF NOT EXISTS "temp_measures"
(
    id        INTEGER primary key,
    value     REAL    not null,
    timestamp TEXT    not null,
    room_id   INTEGER not null,
    FOREIGN KEY (room_id) REFERENCES "rooms" (id)
);
