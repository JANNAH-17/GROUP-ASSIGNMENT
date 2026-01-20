-- Create the database
CREATE DATABASE IF NOT EXISTS project_db;
USE project_db;

[cite_start]-- Create the table structure [cite: 32, 33]
CREATE TABLE IF NOT EXISTS scores (
    user VARCHAR(50) PRIMARY KEY,
    points INT DEFAULT 0,
    datetime_stamp DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

[cite_start]-- Insert default users for C and Python clients [cite: 36]
INSERT IGNORE INTO scores (user, points) VALUES ('Jannah', 0);
INSERT IGNORE INTO scores (user, points) VALUES ('Aizat', 0);