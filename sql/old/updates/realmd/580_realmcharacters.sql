ALTER TABLE realmcharacters DROP INDEX acctid;
ALTER TABLE realmcharacters ADD KEY (acctid);