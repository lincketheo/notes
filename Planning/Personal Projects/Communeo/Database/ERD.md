# user
- firebase_id: VARCHAR(100) Primary Key
- name: VARCHAR(100) 
- email: VARCHAR(100)
- profile_picture_url VARCHAR(150)
# event 
- id: VARCHAR(100) Primary Key
- created_timestamp_utc: TIMESTAMP
- last_updated_timestamp_utc: TIMESTAMP
- firebase_owner_id: VARCHAR(100) 
- starting_timestamp_utc: TIMESTAMP 
- ending_timestamp_utc: TIMESTAMP 
- title: VARCHAR(100) 
- description: VARCHAR(1000)

- **Checks:**
- starting_timestamp_utc >= created_timestamp_utc 
- ending_timestamp_utc > starting_timestamp_utc

- **Constraints:**
- fk_owner FOREIGN KEY(firebase_owner_id) REFERENCES commune_user(firebase_id)

# friendship 
- requester: VARCHAR(100)
- recipient: VARCHAR(100) 
- created_timestamp_utc: TIMESTAMP 

- PRIMARY KEY(requester, recipient),

- **Constraints:**
- fk_requester FOREIGN KEY(requester) REFERENCES commune_user(firebase_id)
- fk_recipient FOREIGN KEY(recipient) REFERENCES commune_user(firebase_id)

CREATE TYPE invitation_status_type AS ENUM ('ACCEPTED', 'PENDING');

# invitation 
- id: VARCHAR(100) PRIMARY KEY,
- created_timestamp_utc: TIMESTAMP NOT NULL,
- last_updated_timestamp_utc: TIMESTAMP NOT NULL,
- event_id: VARCHAR(100) NOT NULL,
- sender_id: VARCHAR(100) NOT NULL,
- recipient_id: VARCHAR(100) NOT NULL,
- status invitation_status_type: NOT NULL,
- expiration_timestamp_utc: TIMESTAMP NOT NULL,

- **Constraints:**
- fk_invitation_event FOREIGN KEY(event_id) REFERENCES event(id)
- fk_invitation_sender FOREIGN KEY(sender_id) REFERENCES commune_user(firebase_id)
- fk_invitation_recipient FOREIGN KEY(recipient_id) REFERENCES commune_user(firebase_id)