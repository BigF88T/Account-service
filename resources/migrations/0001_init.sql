CREATE TABLE IF NOT EXISTS accounts (
    id serial primary key,
    owner_name text not null,
    balance double precision default 0.0 not null
);