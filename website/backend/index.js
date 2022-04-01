const express = require("express"); // Import the express module.
const http = require("http"); // Import the http module.
const socketio = require("socket.io"); // Import the socket.io module.

const app = express(); // The main express application.
app.use((req, res, next) => {
  res.header("Access-Control-Allow-Origin", "*");
  res.header(
    "Access-Control-Allow-Headers",
    "Origin, X-Requested-With, Content-Type, Accept"
  );
  next();
});
app.use(express.json()); // Middleware that parses incoming JSON data.
const server = http.createServer(app); // Create a http server for the app to listen on.
const io = socketio(server); // Creates a socket.io server that listens on the given server.

const router = require("./routes/routes.js")(io); // Import the routes module.
app.use(router); // The main router for the application.  This is the entry point for the application.

const cors = require("cors"); // Import the cors module.
app.use(cors);

const port = process.env.PORT || 3000;
server.listen(port, () => {
  // Start the server and listen on port 3000.
  console.log("server is up on port", port);
});

const db = require("./config/database.js"); // Import the database module.
db.connect((error) => {
  // Connect to the database
  if (error) throw error; // If there is an error, throw it.  Otherwise, do nothing.
  console.log("Connected to Database!");
});
