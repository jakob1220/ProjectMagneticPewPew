const mysql = require("mysql2");

/**
 * Connects to the database and returns a connection object.
 * @returns A connection object.
 */
const db = mysql.createConnection({
    host: "localhost",
    user: "dbadmin",
    password: "GbdgBvfO1!",
    database: "ProjectMagneticPewPew"
});

module.exports = db; // Export the database instance to the module.