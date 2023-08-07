CREATE TABLE IF NOT EXISTS "users"
(
    id       INTEGER PRIMARY KEY,
    name     TEXT NOT NULL,
    password TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS "rooms"
(
    id       INTEGER PRIMARY KEY,
    name     TEXT    NOT NULL,
    password TEXT    NOT NULL,
    user_id  INTEGER NOT NULL,
    FOREIGN KEY (user_id) REFERENCES "users" (id)
);

CREATE TABLE IF NOT EXISTS "temp_measures"
(
    id        INTEGER PRIMARY KEY,
    value     REAL    NOT NULL,
    timestamp TEXT    NOT NULL,
    room_id   INTEGER NOT NULL,
    FOREIGN KEY (room_id) REFERENCES "rooms" (id)
);
