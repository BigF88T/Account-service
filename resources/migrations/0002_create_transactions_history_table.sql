CREATE TABLE IF NOT EXISTS transactions_history (
    id serial primary key,
    from_id int not null,
    to_id int not null,
    amount double precision not null,
    created_at timestamptz DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT fk_from_id FOREIGN KEY (from_id) REFERENCES accounts(id),
    CONSTRAINT fk_to_id FOREIGN KEY (to_id) REFERENCES accounts(id),

    CONSTRAINT chk_different_ids CHECK ( from_id != to_id )
);